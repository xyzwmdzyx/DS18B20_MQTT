/********************************************************************************
 *      Copyright:  (C) 2024 Company
 *                  All rights reserved.
 *
 *       Filename:  process.h
 *    Description:  This file is a process fuction declare file.
 *
 *        Version:  1.0.0(2024年03月28日)
 *         Author:  WangMingda <wmd.de.zhanghu@gmail.com>
 *      ChangeLog:  1, Release initial version on "2024年03月28日 21时28分47秒"
 *                 
 ********************************************************************************/


#ifndef _PROCESS_H_
#define _PROCESS_H_

#include <signal.h>
#include <time.h>

#define PID_ASCII_SIZE  11

typedef struct proc_signal
{
    int       signal;
    unsigned  stop;     /* 0: Not term  1: Stop  */
}proc_signal_t;

typedef void* (*threadFunc)(void *thread_arg);

extern proc_signal_t    g_signal;

/* description: install default signal process functions */
extern void installDefaultSignal(void);


/*	description:	set process runs as a daemon process in background
 *	 input args:	
 *                  $nodir   : change work directory to / or not,       1: nochange 0: change
 *                  $noclose : close opened file descrtipion or not,    1: noclose 0: close
 */
extern void daemonize(int nochdir, int noclose);


/*	description:	check process already running or not, if not then run it and
 *                  record pid into $pidfile
 *	 input args:	
 *					$daemon : set process running in daemon or not
 *					$pidfile: file path whitch record PID
 * return value:    <0: failure  0: success
 */
extern int checkSetProgramRunning(int daemon, char *pidfile);


/*	description:	record running daemon process PID to file "pid_file"
 *	 input args:	
 *					$pidfile: file path whitch record PID
 * return value:    <0: failure  0: success
 */
extern int recordDaemonPid(const char *pidfile);


/*	description:	get daemon process PID from PID record file "pid_file"
 *	 input args:	
 *					$pidfile: file path whitch record PID
 * return value:    pid_t: The daemon process PID, if faliure, return -1
 */
extern pid_t getDaemonPid(const char *pidfile);


/*	description:	check daemon process running or not
 *	 input args:	
 *					$pidfile: file path whitch record PID
 * return value:    =1: already running  =0: not running yet
 */
extern int checkDaemonRunning(const char *pidfile);


/*	description:	stop daemon process running
 *	 input args:	
 *					$pidfile: file path whitch record PID
 * return value:    =0: success
 */
extern int stopDaemonRunning(const char *pidfile);


/*	description:	set process running as daemon (if it's not already running) and record
 *                  it's PID to pidfile.
 *	 input args:	
 *					$pidfile: file path whitch record PID
 * return value:    =0: success     =-1:faliure
 */
extern int setDaemonRunning(const char *pidfile);


/*	description:	start a new thread to run $thread_workbody point function
 *	 input args:	
 *					$thread_id       : store new thread id
 *                  $thread_workbody : function whitch new thread will run  
 *                  $thread_arg      : function input args
 * return value:    =0: success     <0: faliure
 */
extern int threadStart(pthread_t *thread_id, threadFunc thread_workbody, void *thread_arg);


/*	description:	excute a linux command by function system()
 *	 input args:	
 *					$format: command args
 */
extern void execSystemCmd(const char *format, ...);