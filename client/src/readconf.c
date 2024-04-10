/*********************************************************************************
 *      Copyright:  (C) 2024 Company
 *                  All rights reserved.
 *
 *       Filename:  readconf.c
 *    Description:  This file is a read configure function file.
 *                 
 *        Version:  1.0.0(2024年04月05日)
 *         Author:  WangMingda <wmd.de.zhanghu@gmail.com>
 *      ChangeLog:  1, Release initial version on "2024年04月05日 19时10分40秒"
 *                 
 ********************************************************************************/

#include <string.h>
#include "readconf.h"
#include "logger.h"


/*	description:	get log system configurations
 *	 input args:	
 *					$confile  : configure file path
 *                  $log_conf : log system configurations struct 
 * return value:    <0: failure   0: success
 */
int readLogConf(char *confile, log_conf_t *log_conf) {

    char    line[256];
    int     flag = 0;
    char    *key = NULL;
    char    *value = NULL;
    
    // check input args
    if( !confile || !log_conf ) {
        logError("fuction %s() gets invalid input args\n", __func__);
        flag = -1;
        goto Cleanup;
    }

    // read configurations by line
    memset(line, 0, sizeof(line));
    while(fgets(line, sizeof(line), confile)) {

        // remove '\n'
        line[strcspn(line, "\n")] = '\0';

        // check if this section is log
        if( !strcmp(line, "[log]") ) {
            flag = 1;
            continue;
        }

        // read key and value
        if( flag ) {

            // check if this line is a new section
            if(line[0] == '[') {
                flag = 0;
                break;
            }

            // parse key and value
            key = strtok(line, "=");
            value = strtok(NULL, "=");
            if( key && value ) {
                if( !strcmp(key, "logpath") ) {
                    strncpy(log_conf->logfile, value, sizeof(log_conf->logfile));
                }
                else if( !strcmp(key, "loglevel") ) {
                    log_conf->loglevel = atoi(value);
                }
                else if( !strcmp(key, "logsize") ) {
                    log_conf->logsize = atoi(value);
                }
                else {
                    logError("invalid key whitch is not been allowed\n");
                    flag = -3;
                    goto Cleanup;
                }
            }
            else {
                logError("can't read key or value form this section\n");
                flag = -2;
                goto Cleanup;
            }
        }

    }

 Cleanup:
    return flag;
}


/*	description:	get hardware configurations
 *	 input args:	
 *					$confile  : configure file path
 *                  $log_conf : hardware configurations struct 
 * return value:    <0: failure   0: success
 */
int readHardWareConf(char *confile, hardware_conf_t *hard_conf) {

    char    line[256];
    int     flag = 0;
    char    *key = NULL;
    char    *value = NULL;
    
    // check input args
    if( !confile || !hard_conf ) {
        logError("fuction %s() gets invalid input args\n", __func__);
        flag = -1;
        goto Cleanup;
    }

    // read configurations by line
    memset(line, 0, sizeof(line));
    while(fgets(line, sizeof(line), confile)) {

        // remove '\n'
        line[strcspn(line, "\n")] = '\0';

        // check if this section is hardware
        if( !strcmp(line, "[hardware]") ) {
            flag = 1;
            continue;
        }

        // read key and value
        if( flag ) {

            // check if this line is a new section
            if(line[0] == '[') {
                flag = 0;
                break;
            }

            // parse key and value
            key = strtok(line, "=");
            value = strtok(NULL, "=");
            if( key && value ) {
                if( !strcmp(key, "deviceid") ) {
                    strncpy(hard_conf->deviceid, value, sizeof(hard_conf->deviceid));
                }
                else if( !strcmp(key, "ds18b20") ) {
                    hard_conf->ds18b20 = atoi(value);
                }
                else {
                    logError("invalid key whitch is not been allowed\n");
                    flag = -3;
                    goto Cleanup;
                }
            }
            else {
                logError("can't read key or value form this section\n");
                flag = -2;
                goto Cleanup;
            }
        }

    }

 Cleanup:
    return flag;
}


/*	description:	get mosquitto mqtt configurations
 *	 input args:	
 *					$confile  : configure file path
 *                  $log_conf : mosquitto mqtt configurations struct 
 * return value:    <0: failure   0: success
 */
int readBrokerConf(char *confile, broker_conf_t *bro_conf) {

    char    line[256];
    int     flag = 0;
    char    *key = NULL;
    char    *value = NULL;
    
    // check input args
    if( !confile || !mqtt_conf ) {
        logError("fuction %s() gets invalid input args\n", __func__);
        flag = -1;
        goto Cleanup;
    }

    // read configurations by line
    memset(line, 0, sizeof(line));
    while(fgets(line, sizeof(line), confile)) {

        // remove '\n'
        line[strcspn(line, "\n")] = '\0';

        // check if this section is hardware
        if( !strcmp(line, "[broker]") ) {
            flag = 1;
            continue;
        }

        // read key and value
        if( flag ) {

            // check if this line is a new section
            if(line[0] == '[') {
                flag = 0;
                break;
            }

            // parse key and value
            key = strtok(line, "=");
            value = strtok(NULL, "=");
            if( key && value ) {
                if( !strcmp(key, "deviceid") ) {
                    strncpy(hard_conf->deviceid, value, sizeof(hard_conf->deviceid));
                }
                else if( !strcmp(key, "ds18b20") ) {
                    hard_conf->ds18b20 = atoi(value);
                }
                else {
                    logError("invalid key whitch is not been allowed\n");
                    flag = -3;
                    goto Cleanup;
                }
            }
            else {
                logError("can't read key or value form this section\n");
                flag = -2;
                goto Cleanup;
            }
        }

    }

 Cleanup:
    return flag;
}