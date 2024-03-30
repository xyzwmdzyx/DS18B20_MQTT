/*********************************************************************************
 *      Copyright:  (C) 2024 Company
 *                  All rights reserved.
 *
 *       Filename:  process.c
 *    Description:  This file is a process fuction file.
 *                 
 *        Version:  1.0.0(2024年03月28日)
 *         Author:  WangMingda <wmd.de.zhanghu@gmail.com>
 *      ChangeLog:  1, Release initial version on "2024年03月28日 21时27分38秒"
 *                 
 ********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <libgen.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "process.h"
#include "logger.h"

proc_signal_t    g_signal;

/* description:     sighandler when process catch a signal
 * input args :  
 *                  $sig: signal whitch being catched by process
 */
void procDefaultSighandler(int sig) {

    switch(sig){

        case SIGINT:
            logWarn("SIGINT - stopping\n");
            g_signal.stop = 1;
            break;

        case SIGTERM:
            logWarn("SIGTERM - stopping\n");
            g_signal.stop = 1;
            break;

        case SIGSEGV:
            logWarn("SIGSEGV - stopping\n");
            break;

        case SIGPIPE:
            logWarn("SIGPIPE - warnning\n");
            break;

        default:
            break;
    }
}


/* description: install default signal process functions */
void installDefaultSignal(void) {

    struct sigaction    sigact, sigign;

    logInfo("install default signal handler.\n");

    // initialize the catch signal structure
    sigemptyset(&sigact.sa_mask);
    sigact.sa_flags = 0;
    sigact.sa_handler = procDefaultSighandler;

    // setup the ignore signal, but this program diden't need to ignore any signal ,so just remain
    sigemptyset(&sigign.sa_mask);
    sigign.sa_flags = 0;
    sigign.sa_handler = SIG_IGN;

    sigaction(SIGTERM, &sigact, 0); // catch terminate signal "kill" command
    sigaction(SIGINT,  &sigact, 0); // catch interrupt signal CTRL+C
    sigaction(SIGSEGV, &sigact, 0); // catch segmentation faults
    sigaction(SIGPIPE, &sigact, 0); // catch broken pipe
}


/*	description:	set process runs as a daemon process in background
 *	 input args:	
 *                  $nodir   : change work directory to / or not,       1: nochange 0: change
 *                  $noclose : close opened file descrtipion or not,    1: noclose 0: close
 */
void daemonize(int nochdir, int noclose) {

    int rv, fd;
    int i;

    // if this process is already a daemon process (that means it's parent process is /init )
    if (1 == getppid()){
        return;
    }

    // fork a new process
    rv = fork();
    if (rv < 0){
        exit(1);
    }

    // parent process exit
    if (rv > 0) {
        exit(0);
    }

    // obtain a new process session group
    setsid();

    if( !noclose ) {
        // close all descriptors
        for (i = getdtablesize(); i >= 0; --i) {
                close(i);
        }

        // redirect standard input[0] to /dev/null
        fd = open("/dev/null", O_RDWR);

        // redirect standard output[1] to /dev/null
        dup(fd);

        // redirect standard error[2] to /dev/null
        dup(fd);
    }

    umask(0);

    if (!nochdir) {
        chdir("/");
    }

    return;
}


/*	description:	check process already running or not, if not then run it and
 *                  record pid into $pidfile
 *	 input args:	
 *					$daemon : set process running in daemon or not
 *					$pidfile: file path whitch record PID
 * return value:    <0: failure  0: success
 */
int checkSetProgramRunning(int daemon, char *pidfile) {

    // check input args
    if( !pidfile ) {
        return -1;
    }

    // check process already running or not
    if( checkDaemonRunning(pidfile) ) {
        logError("process already running\n");
        return -1;
    }

    // set process running as a daemon process
    if( daemon ) {
        if( setDaemonRunning(pidfile) < 0 ) {
            logError("set process running as a daemon process failure\n");
            return -2;
        }
    }
    else {
        if( recordDaemonPid(pidfile) < 0 ) {
            logError("record process running PID failure\n");
            return -3;
        }
    }

    return 0;
}


/*	description:	record running daemon process PID to file "pid_file"
 *	 input args:	
 *					$pidfile: file path whitch record PID
 * return value:    <0: failure  0: success
 */
int recordDaemonPid(const char *pidfile) {

    struct stat     fStatBuf;
    int             fd = -1;
    int             mode = S_IROTH | S_IXOTH | S_IRGRP | S_IXGRP | S_IRWXU;
    char            ipc_dir[64] = {0};
    char            pid[PID_ASCII_SIZE] = {0};

    strncpy(ipc_dir, pidfile, 64);

    // dirname() will modify ipc_dir and save the result
    dirname(ipc_dir);

    // if folder pidfile path doesn't exist, then create it
    if( stat(ipc_dir, &fStatBuf) < 0 ) {
        if( mkdir(ipc_dir, mode) < 0 ) {
            logError("cannot create %s: %s\n", ipc_dir, strerror(errno));
            return -1;
        }
        (void)chmod(ipc_dir, mode);
    }

    // create the process running PID file
    mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    if( (fd = open(pidfile, O_RDWR | O_CREAT | O_TRUNC, mode)) >= 0 ) {
        memset(pid, 0, sizeof(pid));
        snprintf(pid, sizeof(pid), "%u\n", (unsigned)getpid());
        write(fd, pid, strlen(pid));
        close(fd);
        logDebug("record PID<%u> to file %s.\n", getpid(), pidfile);
    }
    else {
        logError("cannot create %s: %s\n", pidfile, strerror(errno));
        return -2;
    }

    return 0;
}


/*	description:	get daemon process PID from PID record file "pid_file"
 *	 input args:	
 *					$pidfile: file path whitch record PID
 * return value:    pid_t: The daemon process PID, if faliure, return -1
 */
pid_t getDaemonPid(const char *pidfile) {

    FILE        *fp = NULL;
    pid_t       pid;
    char        pid_ascii[PID_ASCII_SIZE] = {0};

    if( (fp = fopen(pidfile, "rb")) != NULL ) {
        memset(pid_ascii, 0, sizeof(pid_ascii));
        (void)fgets(pid_ascii, PID_ASCII_SIZE, fp);
        (void)fclose(fp);
        pid = atoi(pid_ascii);
    }
    else {
        logError("can't open PID record file %s: %s\n", pidfile, strerror(errno));
        return -1;
    }
    return pid;
}


/*	description:	check daemon process running or not
 *	 input args:	
 *					$pidfile: file path whitch record PID
 * return value:    =1: already running  =0: not running yet
 */
int checkDaemonRunning(const char *pidfile) {

    int                 rv = -1;
    pid_t               pid = -1;
    struct stat         fStatBuf;

    rv = stat(pidfile, &fStatBuf);
    // pidfile does exist
    if (0 == rv) {
        logInfo("PID record file \"%s\" exist.\n", pidfile);
        pid = getDaemonPid(pidfile);
        // pid number does exsit, but we need to check if this process really running
        if(pid > 0) {
            // send signal to process gets reply, means this process really running
            if((rv = kill(pid, 0)) == 0) {
                logInfo("process with PID[%d] seems running.\n", pid);
                return 1;
            }
            // send signal to process gets no reply, means this process dosen't running
            else {
                logWarn("process with PID[%d] seems exit.\n", pid);
                remove(pidfile);
                return 0;
            }
        }
        // pid number is invalid
        else if (0 == pid) {
            logWarn("can't read process PID form record file.\n");
            remove(pidfile);
            return 0;
        }
        // read pid from file "pidfile" failure
        else {
            logError("read record file \"%s\" failure, maybe process still running.\n", pidfile);
            return 1;
        }
    }
	// pid file does not exist, directly return 0
    return 0;
}


/*	description:	stop daemon process running
 *	 input args:	
 *					$pidfile: file path whitch record PID
 * return value:    =0: success
 */
int stopDaemonRunning(const char *pidfile) {
	
    pid_t            pid = -1;
    struct stat      fStatBuf;

    // pidfile dosen't exist
    if( stat(pidfile, &fStatBuf) < 0) {
        return 0;
    }

    logInfo("PID record file \"%s\" exist.\n", pidfile);
    pid = getDaemonPid(pidfile);
    // kill this process with signal SIGTERM until success
    if(pid > 0) {
        while( (kill(pid, 0) ) == 0) {
            kill(pid, SIGTERM);
            sleep(1);
        }
        remove(pidfile);
    }

    return 0;
}


/*	description:	set process running as daemon (if it's not already running) and record
 *                  it's PID to pidfile.
 *	 input args:	
 *					$pidfile: file path whitch record PID
 * return value:    =0: success     =-1:faliure
 */
int setDaemonRunning(const char *pidfile) {

    daemonize(0, 1);
    logInfo("process running as daemon [PID:%d].\n", getpid());

    if( recordDaemonPid(pidfile) < 0 ) {
        logError("record PID to file \"%s\" failure.\n", pidfile);
        return -1;
    }

    return 0;
}



/*	description:	start a new thread to run $thread_workbody point function
 *	 input args:	
 *					$thread_id       : store new thread id
 *                  $thread_workbody : function whitch new thread will run  
 *                  $thread_arg      : function input args
 * return value:    =0: success     <0: faliure
 */
int threadStart(pthread_t *thread_id, threadFunc thread_workbody, void *thread_arg) {

    int                rv = 0;
    pthread_t          tid;
    pthread_attr_t     thread_attr;

    // check input args
    if( !thread_id || !thread_workbody || !thread_arg ) {
        return -1;
    }

    // initialize thread attribute
    rv = pthread_attr_init(&thread_attr);
    if( rv ) {
        return -2;
    }

    // set stack size of thread
    rv = pthread_attr_setstacksize(&thread_attr, 120 * 1024);
    if( rv ) {
        goto Cleanup;
    }

    // set thread detached state, that means parent don`t need pthread_join
    rv = pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
    if( rv ) {
        goto Cleanup;
    }

    // create thread
    rv = pthread_create(&tid, &thread_attr, thread_workbody, thread_arg);
    if( rv ) {
        goto Cleanup;
    }

 Cleanup:
    if( thread_id ) {
        if( rv ) {
            *thread_id = 0;
        }
        else {
            *thread_id = tid;
        }
    }

    // destroy attributes of thread
    pthread_attr_destroy(&thread_attr);
    return rv;
}


/*	description:	excute a linux command by function system()
 *	 input args:	
 *					$format: command args
 */
void execSystemCmd(const char *format, ...) {

    char                cmd[256];
    va_list             args;

    memset(cmd, 0, sizeof(cmd));

    va_start(args, format);
    vsnprintf(cmd, sizeof(cmd), format, args);
    va_end(args);

    system(cmd);
}
