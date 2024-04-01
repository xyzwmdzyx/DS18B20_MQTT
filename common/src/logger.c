/*********************************************************************************
 *      Copyright:  (C) 2024 Company
 *                  All rights reserved.
 *
 *       Filename:  logger.c
 *    Description:  This file is a logger function file.
 *                 
 *        Version:  1.0.0(2024年03月24日)
 *         Author:  WangMingda <wmd.de.zhanghu@gmail.com>
 *      ChangeLog:  1, Release initial version on "2024年03月24日 19时18分10秒"
 *                 
 ********************************************************************************/

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <pthread.h>

#include "logger.h"

typedef void (*log_LockFunc)(void *udata, int lock);

static struct {
    char        	file[32];   // log file name
    FILE        	*fp;        // log file pointer
    long        	size;       // log file max size
    int         	level;      // log level
    log_LockFunc  	lockfunc;   // lock function
    void        	*udata;     // lock data
} log_t;

static const char *level_names[] = {
    "ERROR",
    "WARN",
    "INFO",
    "DEBUG",
    "TRACE"
};

static const char *level_colors[] = {
    "\x1b[31m",
    "\x1b[33m",
    "\x1b[32m",
    "\x1b[36m",
    "\x1b[94m"
};


/*	description:	get now time and change it into str
 *	 input args:	
 *					$time_buf: buffer which store time str
 */
static inline void timeToStr(char *time_buf) {
    
    int              len;
    struct timeval   tv;
    struct tm       *tm;
    
    gettimeofday(&tv, NULL);
    tm = localtime(&tv.tv_sec);

    len = sprintf(time_buf, "%04d-%02d-%02d %02d:%02d:%02d.%02d",
            tm -> tm_year + 1900, tm -> tm_mon + 1, tm -> tm_mday,
            tm -> tm_hour, tm -> tm_min, tm -> tm_sec, ((int)tv.tv_usec + 5000) / 10000);
    time_buf[len] = '\0';

    return;
}


/*	description:	aquire mutex lock or release mutex lock, depend on arg "lock" 
 *	 input args:	
 *					$udata: mutex lock
 *                  $lock : verify aquire or release mutex lock
 */
static void mutexLock(void *udata, int lock) {

    int                 err;
    pthread_mutex_t     *log_lock = (pthread_mutex_t *)udata;

    // if lock == 1, then aquire mutex lock
    if( lock ) {
        if ( (err = pthread_mutex_lock(log_lock)) != 0 ) {
            logError("Unable to lock log's mutex lock: %s", strerror(err));
        }
            
    }
    // if lock == 0, then release mutex lock
    else {
        if ( (err = pthread_mutex_unlock(log_lock) != 0) ) {
            logError("Unable to unlock log's mutex lock: %s", strerror(err));
        }   
    }

    return;
}


/*	description:	init log system, create log file or just log to console
 *	 input args:	
 *					$fname: log file name, "NULL" / "console" / "stderr" means log to console
 *                  $level: log system level
 *                  $size : log file max size
 *                  $lock : enable lock or not
 * return value:    <0: failure   0: success
 */
int logInit(char *fname, int level, int size, int lock) {
    
    FILE                        *fp = NULL;
    static pthread_mutex_t      log_lock;

    // check input args
    if( !fname || !(level >= LOG_ERROR && level <= LOG_MAX) || size < 0 || !(lock >= LOG_LOCK_DISABLE && lock <= LOG_LOCK_ENABLE) ) {
        return -1;
    }

    log_t.level  = level;
    log_t.size   = size * 1024;
    
    // if enable lock, then init udata and lockfunc
    if( lock ) {
        pthread_mutex_init(&log_lock, NULL);
        log_t.udata = (void *)&log_lock;
        log_t.lockfunc = mutexLock;
    }

    // log to console
    if( !fname || !strcmp(fname, "console") || !strcmp(fname, "stderr") ) {
        strcpy(log_t.file, "console");
        log_t.fp = stderr;
        // console don't need rollback
        log_t.size = 0; 
        
        logInfo("log system(%s) start: filename: \"%s\", loglevel: %s\n",
        			LOG_VERSION, log_t.file, level_names[level]);
    }
    // log to file
    else {
        if( !(fp = fopen(fname, "a+")) ) {
            fprintf(stderr, "%s() failed: %s\n", __func__, strerror(errno));
            return -2;
        }
        log_t.fp = fp;
        strncpy(log_t.file, fname, sizeof(log_t.file));
        
        logInfo("log system(%s) start: filename: \"%s\", loglevel: %s, filemaxsize: %luKiB\n", 
        			LOG_VERSION, log_t.file, level_names[level], size);
    }

    return 0;
}


/*	description:	terminate log system */
void logTerm(void) {

    if( log_t.fp && (log_t.fp != stderr) ) {
        fclose(log_t.fp);
    }

    // destroy mutex lock
    if( log_t.udata ) {
        pthread_mutex_destroy(log_t.udata);
    }

    return;
}

/*	description:	rollback log file if it's already full */
static void logRollBack(void) {

    char       cmd[128] = {0};
    long       fsize;

    // log to console, dosen't need rollback
    if(log_t.size <= 0 ) {
        return;
    }

    fsize = ftell(log_t.fp);
    // log file not full, dosen't need rollback
    if( fsize < log_t.size ) {
        return;
    }

    // backup current log file
    snprintf(cmd, sizeof(cmd), "cp %s %s.bak", log_t.file, log_t.file);
    system(cmd);

    // rollback file
    fseek(log_t.fp, 0, SEEK_SET);
    truncate(log_t.file, 0);

    fprintf(log_t.fp, "\n");
    logInfo("log system(%s) rollback: file:\"%s\", level:%s, maxsize:%luKiB\n",
                LOG_VERSION, log_t.file, level_names[log_t.level], log_t.size / 1024);

    return ;
}


/*	description:	write log message into log file(or console)
 *	 input args:	
 *                  $level: log system level
 *                  $file : current file name
 *                  $lien : current file line number
 *                  $fmt  : format string
 */
void logWrite(int level, const char *file, int line, const char *fmt, ...) {

    va_list    args;
    char       time_str[32];

    // if write message level is bigger, then do nothing
    if ( !log_t.fp || level > log_t.level )
        return;

    // if enable lock, then acquire mutex lock
    if ( log_t.lockfunc ) {
        log_t.lockfunc(log_t.udata, 1);
    }

    // check and rollback log file if necessary
    logRollBack();

    // get time
    memset(time_str, 0, sizeof(time_str));
    timeToStr(time_str);

    // log to console
    if( log_t.fp == stderr ) {
        fprintf(log_t.fp, "[%s]%s[%s]\x1b[0m\x1b[90m[%s %d]: \x1b[0m",
                    time_str, level_colors[level], level_names[level], file, line);
    }
    // log to file
    else {
        fprintf(log_t.fp, "[%s][%s][%s %d]: ", time_str, level_names[level], file, line);
    }

    va_start(args, fmt);
    vfprintf(log_t.fp, fmt, args);
    va_end(args);

    fflush(log_t.fp);

    // if enable lock, then release mutex lock
    if ( log_t.lockfunc ) {
        log_t.lockfunc(log_t.udata, 0);
    }

    return;
}
