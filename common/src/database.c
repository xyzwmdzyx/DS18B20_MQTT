/*********************************************************************************
 *      Copyright:  (C) 2024 Company
 *                  All rights reserved.
 *
 *       Filename:  database.c
 *    Description:  This file is a database function file.
 *                 
 *        Version:  1.0.0(2024年03月25日)
 *         Author:  WangMingda <wmd.de.zhanghu@gmail.com>
 *      ChangeLog:  1, Release initial version on "2024年03月25日 17时38分27秒"
 *                 
 ********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "database.h"
#include "logger.h"

// Blob packet table name
#define TABLE_NAME     "PackTable"

/* Use static global handler in order to simplify API,
 * but it will make this library not thread safe
 */
static sqlite3         *db = NULL;


/*	description:	init database system
 *	 input args:	
 *					$fname: database file name
 * return value:    <0: failure   0: success
 */
int databaseInit(char *fname) {

    char               sql[SQL_COMMAND_LEN] = {0};
    char               *errmsg = NULL;

    // check input args
    if( !fname ) {
        logError("function %s() gets invalid input arguments\n", __func__);
        return -1;
    }

    // database file already exist, then open it
    if( 0 == access(fname, F_OK) ) {
        if( SQLITE_OK != sqlite3_open(fname, &db) ) {
            logError("%s() failed: %s\n", __func__, sqlite3_errmsg(db));
            return -2;
        }
        logInfo("database system(%s) start: filename: \"%s\"\n", DATABASE_VERSION, fname);
        return 0;
    }

    // database not exist, then create and init it
    if( SQLITE_OK != sqlite3_open(fname, &db) ) {
        logError("%s() failed: %s\n", __func__, sqlite3_errmsg(db));
        return -2;
    }

    // SQLite continues without syncing as soon as it has handed data off to the operating system
    sqlite3_exec(db, "pragma synchronous = OFF; ", NULL, NULL, NULL);

    // enable full auto vacuum, Auto increase/decrease
    sqlite3_exec(db, "pragma auto_vacuum = 2 ; ", NULL, NULL, NULL);

    // create table in the database
    snprintf(sql, sizeof(sql), "CREATE TABLE %s(packet BLOB);", TABLE_NAME);
    if( SQLITE_OK != sqlite3_exec(db, sql, NULL, NULL, &errmsg) ) {
        logError("create datatable in database file '%s' failure: %s\n", fname, errmsg);
        // free errmsg
        sqlite3_free(errmsg);
        // close databse 
        sqlite3_close(db);   
        // remove database file
        unlink(fname);      
        return -3;
    }

    logInfo("database system(%s) start: filename: \"%s\"\n", DATABASE_VERSION, fname);
    return 0;
}


/* description: terminate sqlite database */
void databaseTerm(void) {

    sqlite3_close(db);
    logWarn("close database success\n");

    return ;
}


/* description :    push a blob packet into database
 *  input args :
 *       $pack :    blob packet data address
 *       $size :    blob packet data bytes
 * return value:    <0: failure   0: success
 */
int databasePushPacket(void *pack, int size) {

    char                sql[SQL_COMMAND_LEN] = {0};
    int                 rv = 0;
    sqlite3_stmt        *stat = NULL;

    // check input args
    if( !pack || size <= 0 ) {
        logError("function %s() gets invalid input arguments\n", __func__);
        return -1;
    }

    if( !db ) {
        logError("sqlite database not been opened\n");
        return -2;
    }

    // prepare SQL command
    snprintf(sql, sizeof(sql), "INSERT INTO %s(packet) VALUES(?)", TABLE_NAME);
    rv = sqlite3_prepare_v2(db, sql, -1, &stat, NULL);
    if( SQLITE_OK != rv || !stat ) {
        logError("function sqlite3_prepare_v2() failure when push blob packet\n");
        rv = -2;
        goto Cleanup;
    }

    // bind blob packet data on SQL command
    if( SQLITE_OK != sqlite3_bind_blob(stat, 1, pack, size, NULL) ) {
        logError("function sqlite3_bind_blob() failure when push blob packet\n");
        rv = -3;
        goto Cleanup;
    }

    // execute SQL command
    rv = sqlite3_step(stat);
    if( SQLITE_DONE != rv && SQLITE_ROW != rv ) {
        logError("function sqlite3_step() failure when push blob packet\n");
        rv = -4;
        goto Cleanup;
    }

 Cleanup:
    sqlite3_finalize(stat);

    if( rv < 0 ) {
        logError("add new blob packet into database failure, rv = %d\n", rv);
    } 
    else {
        logInfo("add new blob packet into database success\n");
    }
    return rv;
}


/* description :    pop first blob packet from database
 *  input args :
 *       $pack :    blob packet output buffer address
 *       $size :    blob packet output buffer size
 *       $byte :    blob packet data bytes
 * return value:    <0: failure   0: success
 */
int databasePopPacket(void *pack, int size, int *bytes) {

    char                sql[SQL_COMMAND_LEN] = {0};
    int                 rv = 0;
    sqlite3_stmt        *stat = NULL;
    const void          *blob_ptr;

    // check input args
    if( !pack || size <= 0 || !bytes ) {
        logError("function %s() gets invalid input arguments\n", __func__);
        return -1;
    }

    if( !db ) {
        logError("sqlite database not been opened\n");
        return -2;
    }

    // only query first packet record
    snprintf(sql, sizeof(sql), "SELECT packet FROM %s WHERE rowid = (SELECT rowid FROM %s LIMIT 1);", TABLE_NAME, TABLE_NAME);
    // prepare SQL command
    rv = sqlite3_prepare_v2(db, sql, -1, &stat, NULL);
    if( SQLITE_OK != rv || !stat ) {
        logError("function sqlite3_prepare_v2() failure when pop blob packet\n");
        rv = -3;
        goto Cleanup;
    }

    // execute SQL command
    rv = sqlite3_step(stat);
    if( SQLITE_DONE != rv && SQLITE_ROW != rv ) {
        logError("function sqlite3_step() failure when pop blob packet\n");
        rv = -4;
        goto Cleanup;
    }

    // 0 means first column in this row
    blob_ptr = sqlite3_column_blob(stat, 0);
    if( !blob_ptr ) {
        rv = -6;
        goto Cleanup;
    }

    *bytes = sqlite3_column_bytes(stat, 0);

    if( *bytes > size ) {
        logError("blob packet bytes[%d] is larger than bufsize[%d]\n", *bytes, size);
        *bytes = 0;
        rv = -7;
        goto Cleanup;
    }

    memcpy(pack, blob_ptr, *bytes);
    rv = 0;

 Cleanup:
    sqlite3_finalize(stat);
    return rv;
}


/* description :    remove first blob packet from database
 *return value :    <0: failure   0: success
 */
int databaseDelPacket(void) {

    char        sql[SQL_COMMAND_LEN] = {0};
    char        *errmsg = NULL;

    if( !db ) {
        logError("sqlite database not been opened\n");
        return -1;
    }

    // remove packet from database
    memset(sql, 0, sizeof(sql));
    snprintf(sql, sizeof(sql), "DELETE FROM %s WHERE rowid = (SELECT rowid FROM %s LIMIT 1);", TABLE_NAME, TABLE_NAME);
    if( SQLITE_OK != sqlite3_exec(db, sql, NULL, 0, &errmsg) )
    {
        logError("delete first blob packet from database failure: %s\n", errmsg);
        sqlite3_free(errmsg);
        return -2;
    }
    logWarn("delete first blob packet from database success\n");

    // vacuum database
    sqlite3_exec(db, "VACUUM;", NULL, 0, NULL);

    return 0;
}
