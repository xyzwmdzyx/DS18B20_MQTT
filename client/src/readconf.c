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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "readconf.h"
#include "logger.h"


/*	description:	get configurations
 *	 input args:	
 *					$confile  : configure file path
 *					$conf	  : configure struct
 * return value:    <0: failure   0: success
 */
int readConf(char *confile, conf_t *conf) {

    FILE	*fp = NULL;
    
    char    line[256];
    int     flag = 0;
    char    *key = NULL;
    char    *value = NULL;
    
    // check input args
    if( !confile || !conf ) {
        logError("fuction %s() gets invalid input args\n", __func__);
        flag = -1;
        goto Cleanup;
    }
    
    // open configure file
    fp = fopen(confile, "r");
    if( !fp ) {
    	logError("open configure file faliure\n");
    	flag = -2;
    	goto Cleanup;
    }

    // read configurations by line
    memset(line, 0, sizeof(line));
    while(fgets(line, sizeof(line), fp)) {

        // remove '\n'
        line[strcspn(line, "\n")] = '\0';
        
        // ignore #
        if(line[0] == '#' || !strlen(line) || line[0] == '\n') {
        	continue;
        }

        // check section
        if( !strcmp(line, "[hardware]") ) {
        	flag = 1;
        	continue;
        }
        else if( !strcmp(line, "[broker]") ) {
        	flag = 2;
        	continue;
        }
        else if( !strcmp(line, "[publisher]") ) {
        	flag = 3;
        	continue;
        }

        // read key and value
        if( flag ) {

            // check if this line is a new section or an empty line
            if(line[0] == '[' || line[0] == '#' || !strlen(line) || line[0] == '\n') {
                flag = 0;
                continue;
            }

            // parse key and value
            key = strtok(line, "=");
            value = strtok(NULL, "=");
            
            printf("key = %s, value = %s\n", key, value);
            
            // read hardware config
            if( (key && value) && (flag == 1) ) {
            	if( !strcmp(key, "deviceid") ) {
                    strncpy(conf->deviceid, value, sizeof(conf->deviceid));
                }
                else if( !strcmp(key, "ds18b20") ) {
                    conf->ds18b20 = atoi(value);
                }
                else {
                    logError("invalid key whitch is not been allowed in this section\n");
                    flag = -3;
                    goto Cleanup;
                }
            }
            
            // read borker config
            else if( (key && value) && (flag == 2) ) {
            	if( !strcmp(key, "hostname") ) {
            		strncpy(conf->host, value, sizeof(conf->host));
            	}
            	else if( !strcmp(key, "port") ) {
            		conf->port = atoi(value);
            	}
            	else if( !strcmp(key, "clientid") ) {
            		strncpy(conf->clientid, value, sizeof(conf->clientid));
            	}
            	else if( !strcmp(key, "username") ) {
            		strncpy(conf->username, value, sizeof(conf->username));
            	}
            	else if( !strcmp(key, "password") ) {
            		strncpy(conf->password, value, sizeof(conf->password));
            	}
            	else {
            		logError("invalid key whitch is not been allowed in this section\n");
                    flag = -3;
                    goto Cleanup;
            	}
            }
            
            // read publisher config
            else if( (key && value) && (flag == 3) ) {
            	if( !strcmp(key, "pubtopic") ) {
            		strncpy(conf->pubtopic, value, sizeof(conf->pubtopic));
            	}
            	else if( !strcmp(key, "readtime") ) {
            		conf->readtime = atoi(value);
            	}
            }
            else {
                logError("can't read key or value form this section\n");
                flag = -2;
                goto Cleanup;
            }
        }
    }
    
    //close configure file
    fclose(fp);

 Cleanup:
    return flag;
}
