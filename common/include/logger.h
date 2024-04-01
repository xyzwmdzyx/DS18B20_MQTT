/********************************************************************************
 *      Copyright:  (C) 2024 Company
 *                  All rights reserved.
 *
 *       Filename:  logger.h
 *    Description:  This file is a logger function declare file.
 *
 *        Version:  1.0.0(2024年03月24日)
 *         Author:  WangMingda <wmd.de.zhanghu@gmail.com>
 *      ChangeLog:  1, Release initial version on "2024年03月24日 22时06分31秒"
 *                 
 ********************************************************************************/

#ifndef  _LOGGER_H_
#define  _LOGGER_H_

#include <stdio.h>
#include <stdarg.h>

#define LOG_VERSION             "v1.0"
#define ROLLBACK_NONE           0

// log level
enum {
    LOG_ERROR,
    LOG_WARN,
    LOG_INFO,
    LOG_DEBUG,
    LOG_TRACE,
    LOG_MAX
};

// enable lock or disable lock
enum {
    LOG_LOCK_DISABLE,
    LOG_LOCK_ENABLE,
};


/*	description:	init log system, create log file or just log to console
 *	 input args:	
 *					$fname: log file name, "NULL" / "console" / "stderr" means log to console
 *                  $level: log system level
 *                  $size : log file max size
 *                  $lock : enable lock or not
 * return value:    <0: failure   0: success
 */
int logInit(char *fname, int level, int size, int lock);


/*	description:	terminate log system */
void logTerm(void);


/*	description:	rollback log file if it's already full */
static void logRollBack(void);


/*	description:	write log message into log file(or console)
 *	 input args:	
 *                  $level: log system level
 *                  $file : current file name
 *                  $lien : current file line number
 *                  $fmt  : format string
 */
void logWrite(int level, const char *file, int line, const char *fmt, ...);

/* function: write log message into log file with different log level */
#define logTrace(...) logWrite(LOG_TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define logDebug(...) logWrite(LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define logInfo(...)  logWrite(LOG_INFO,  __FILE__, __LINE__, __VA_ARGS__)
#define logWarn(...)  logWrite(LOG_WARN,  __FILE__, __LINE__, __VA_ARGS__)
#define logError(...) logWrite(LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)


#endif
