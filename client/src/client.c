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

#include "readconf.h"
#include "logger.h"
#include "process.h"
#include "database.h"
#include "ds18b20.h"
#include "packet.h"

#define PROG_VERSION               	"v1.0.0"
#define DAEMON_PIDFILE             	"/tmp/.client_mqttd.pid"

// print help information
static void printUsage(char *progname) {

    printf("Usage: %s [OPTION]...\n", progname);
    printf(" %s is LingYun studio temperature MQTT client program running on RaspberryPi\n", progname);
    printf("\nMandatory arguments to long options are mandatory for short options too:\n");
    printf("-d(--debug)   	: running in debug mode\n");
    printf("-h(--help)    	: display this help information\n");
    printf("-v(--version) 	: display the program version\n");
    printf("\n%s version %s\n", progname, PROG_VERSION);
    return;
}

int checkSampleTime(time_t *last_time, int interval);

int main(int argc, char* argv[]) {

	extern proc_signal_t	g_signal;
	int						daemon = 1;
	int						rv = -1;
	
	char                    *progname = NULL;
	
	char					*logfile = "./log/mqttd.log";
	int						loglevel = LOG_INFO;
	int						logsize = 10;
	
	char					*dbfile = "./data/mqttd.db";
	
	char					*confile = "./client.conf";
	conf_t					cli_conf = {0};

    time_t                  last_time = 0;
    int                     sample_flag = 0;
    
    char                    pack_buf[1024] = {0};
    int                     pack_bytes = 0;
    pack_info_t             pack_info = {0};
    packFunc             	pack_function = packetJsonData;
	
	struct option           opts[] = {
                            {"debug", no_argument, NULL, 'd'},                  
                            {"version", no_argument, NULL, 'v'},
                            {"help", no_argument, NULL, 'h'},
                            {NULL, 0, NULL, 0}
                    };
	
	// parament parse
	progname = (char *)basename(argv[0]);
	while( (rv = getopt_long(argc, argv, "dvh", opts, NULL)) != -1 ) {
        switch(rv) {

            case 'd': // set running mode debug
                daemon = 0;
                logfile = "console";
                loglevel = LOG_DEBUG;
                break;

            case 'v':  // get version information
                printf("%s version %s\n", progname, PROG_VERSION);
                return 0;

            case 'h':  // get help information
                printUsage(progname);
                return 0;

            default:
                break;
        }

    }
    
    // init log system
    if( logInit(logfile, loglevel, logsize, LOG_LOCK_DISABLE) < 0 ) {
        fprintf(stderr, "Initial log system failure, program will exit\n");
        return -1;
    }
    
    // init database system
    if( databaseInit(dbfile) < 0 ) {
        logError("Initial database system faliure, program will exit\n");
        return -2;
    }
    
    // reading configure from file: ./client.conf
    if( (rv = readConf(confile, &cli_conf)) < 0 ) {
    	logError("Read configurations from %s faliure, program will exit\n", confile);
    	return -3;
    }
    
    // if ds18b20 is not aviliable, then exit this program
    if( cli_conf.ds18b20 != 1 ) {
    	logError("ds18b20 is not aviliable, program will exit\n");
    	goto Cleanup;
    }
    
    // install signal and it's defalut fuction
    installDefaultSignal();
    
    // check program already running as daemon or not. If not, then set program running in daemon mode
    if( checkSetProgramRunning(daemon, DAEMON_PIDFILE) < 0 ) {
    	goto Cleanup;
    }
    
    // continue running when g_signal.stop != 1
    while( !g_signal.stop ) {
    
    	// 采样标志至0
    	sample_flag = 0;
    	// 如果到时间就读DS18B20的温度
    	if( checkSampleTime(&last_time, cli_conf.readtime) ) {
            logDebug("start sample DS18B20 termperature\n");

            // 读取DS18B20的温度
            if( (rv = ds18b20GetTemperature(&pack_info.temper)) < 0 ) {
                logError("sample DS18B20 temperature failure, errcode = %d\n", rv);
                continue;
            }
            logInfo("sample DS18B20 termperature success, temper = %.3f oC\n", pack_info.temper);

            // 获取设备型号、当前时间
            strncpy(pack_info.devid, cli_conf.deviceid, sizeof(pack_info.devid));
            getTime(pack_info.sample_time, TIME_LEN);

            // 将数据打包成JSON格式
            pack_bytes = pack_function(&pack_info, pack_buf, sizeof(pack_buf));
            logDebug("packet sample data success, pack_buf = %s\n", pack_buf);
            // 采样标志至1
            sample_flag = 1;
        }
    }
    
 Cleanup:
    databaseTerm();
    unlink(DAEMON_PIDFILE);
    logTerm();

    return 0;
}

int checkSampleTime(time_t *last_time, int interval) {

    int                  flag = 0;
    time_t               t = time(&t);
    
	// 现在的时间 > 上次采样时间 + 时间间隔则该采样了   
    if( t >= *last_time + interval ) {
        flag = 1;
        *last_time = t;
    }

    return flag;
}
