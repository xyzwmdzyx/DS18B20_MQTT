/********************************************************************************
 *      Copyright:  (C) 2024 Company
 *                  All rights reserved.
 *
 *       Filename:  database.h
 *    Description:  This file is a database function declare file.
 *
 *        Version:  1.0.0(2024年03月25日)
 *         Author:  WangMingda <wmd.de.zhanghu@gmail.com>
 *      ChangeLog:  1, Release initial version on "2024年03月25日 20时43分31秒"
 *                 
 ********************************************************************************/

#ifndef  _DATABASE_H_
#define  _DATABASE_H_

#include "sqlite3.h"

#define DATABASE_VERSION       "v1.0"
#define SQL_COMMAND_LEN        256

/*	description:	init database system
 *	 input args:	
 *					$fname: database file name
 * return value:    <0: failure   0: success
 */
extern int databaseInit(char *fname);


/* description: terminate sqlite database */
extern void databaseTerm(void);


/* description :    push a blob packet into database
 *  input args :
 *       $pack :    blob packet data address
 *       $size :    blob packet data bytes
 * return value:    <0: failure   0: success
 */
extern int databasePushPacket(void *pack, int size);


/* description :    pop first blob packet from database
 *  input args :
 *       $pack :    blob packet output buffer address
 *       $size :    blob packet output buffer size
 *       $byte :    blob packet data bytes
 * return value:    <0: failure   0: success
 */
extern int databasePopPacket(void *pack, int size, int *bytes);


/* description :    remove first blob packet from database
 * return value:    <0: failure   0: success
 */
extern int databaseDelPacket(void);


#endif
