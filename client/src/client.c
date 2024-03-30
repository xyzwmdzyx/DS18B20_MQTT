/*********************************************************************************
 *      Copyright:  (C) 2024 Company
 *                  All rights reserved.
 *
 *       Filename:  client.c
 *    Description:  This file is a MQTT client file.
 *                 
 *        Version:  1.0.0(2024年03月29日)
 *         Author:  WangMingda <wmd.de.zhanghu@gmail.com>
 *      ChangeLog:  1, Release initial version on "2024年03月29日 20时12分02秒"
 *                 
 ********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <getopt.h>
#include <libgen.h>
#include <string.h>
#include <errno.h>

#include "logger.h"
#include "process.h"
#include "database.h"
#include "ds18b20.h"
#include "packet.h"

#define PROG_VERSION               	"v1.0.0"
#define DAEMON_PIDFILE             	"/tmp/.client_mqttd.pid"

// 打印命令行参数帮助信息
static void printUsage(char *progname) {

    printf("Usage: %s [OPTION]...\n", progname);
    printf(" %s is LingYun studio temperature MQTT client program running on RaspberryPi\n", progname);

    printf("\nMandatory arguments to long options are mandatory for short options too:\n");
    printf("-b(--broker)    : sepcify broker server\n");
    printf("-t(--readtime)	: sepcify report time interval, default 60 seconds\n");
    printf("-d(--debug)   	: running in debug mode\n");
    printf("-h(--help)    	: display this help information\n");
    printf("-v(--version) 	: display the program version\n");

    printf("\n%s version %s\n", progname, PROG_VERSION);
    return;
}

int checkSampleTime(time_t *last_time, int interval);

int main(int argc, char* argv[]) {

	int						daemon = 1;
	int						rv = -1;
	
	char                    *progname = NULL;
	char                    *logfile = "./log/client_mqttd.log";
    int                     loglevel = LOG_INFO;
    int                     logsize = 10; // 日志文件最大为10K
    char					*dbfile = "./data/client_data.db";
    
    char                    *hostname = NULL;
    int                     port = 0;
    int                     readtime = 60; // 默认每60s上报一次
    
    time_t                  last_time = 0;
    int                     sample_flag = 0;
    
    char                    pack_buf[1024];
    int                     pack_bytes = 0;
    pack_info_t             pack_info;
    packFunc             	pack_fuction = packetJsonData; // 使用JSON pack
	
	struct option           opts[] = {
                            {"broker", required_argument, NULL, 'b'},
                            {"readtime", required_argument, NULL, 't'},
                            {"debug", no_argument, NULL, 'd'},
                            {"version", no_argument, NULL, 'v'},
                            {"help", no_argument, NULL, 'h'},
                            {NULL, 0, NULL, 0}
                    };
	
	// 命令行参数解析
	progname = (char *)basename(argv[0]);
	while( (rv = getopt_long(argc, argv, "b:t:dvh", opts, NULL)) != -1 ) {
        switch(rv) {
            case 'b': // 设置代理主机名
                hostname = optarg;
                break;

            case 't': // 设置上报时间间隔
                readtime = atoi(optarg);
                break;

            case 'd': // 设置运行模式为debug模式
                daemon = 0;
                logfile="console";
                loglevel=LOG_DEBUG;
                break;

            case 'v':  // 获取软件版本
                printf("%s version %s\n", progname, PROG_VERSION);
                return 0;

            case 'h':  // 获取帮助信息
                printUsage(progname);
                return 0;

            default:
                break;
        }

    }
    
    // 检查参数
    if( !hostname || readtime <= 0 ) {
        printUsage(argv[0]);
        return 0;
    }
    
    // 初始化日志系统
    if( logInit(logfile, loglevel, logsize, LOG_LOCK_DISABLE) < 0 ) {
        fprintf(stderr, "Initial log system failure, program will exit\n");
        return -1;
    }
    
    // 安装信号及其默认动作
    installDefaultSignal();
    
    // 检查程序是否已经运行，如果没有，则将其丢到后台运行
    //if( checkSetProgramRunning(daemon, DAEMON_PIDFILE) < 0 ) {
    	//goto Cleanup;
    //}

    logInfo("program start running.\n");

    // 初始化数据库系统
    if( databaseInit(dbfile) < 0 ) {
        logError("Initial database system faliure, program will exit\n");
        return -2;
    }
    
 Cleanup:
    databaseTerm();
    unlink(DAEMON_PIDFILE);
    logTerm();

    return 0;
	
	return 0;
}
