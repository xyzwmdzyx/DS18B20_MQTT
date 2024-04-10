/********************************************************************************
 *      Copyright:  (C) 2024 Company
 *                  All rights reserved.
 *
 *       Filename:  readconf.h
 *    Description:  This file is a read configure fuction declare file.
 *
 *        Version:  1.0.0(2024年04月05日)
 *         Author:  WangMingda <wmd.de.zhanghu@gmail.com>
 *      ChangeLog:  1, Release initial version on "2024年04月05日 19时09分05秒"
 *                 
 ********************************************************************************/


#ifndef _READ_CONF_H_
#define _READ_CONF_H_

typedef struct log_conf_s {

    char        logfile[64];            // log path     
    int         loglevel;               // log level
    int         logsize;                // log max size

}log_conf_t;

typedef struct hardware_conf_s {

    char            deviceid[16];       // device id
    int             ds18b20;            // ds18b20 = 1 means this hardware exist

}hardware_conf_t;

typedef struct broker_conf_s {
    
    char        	host[256];          // broker hostname
    int         	port;               // broker port
    char            clientid[128];      // client id
    char            username[128];      // user name
    char            password[128];      // pass word

}broker_conf_t;

typedef struct puber_conf_s {

    char            pubtopic[256];      // publish topic
    int             readtime;           // sample interval tim

}puber_conf_t;


/*	description:	get configurations
 *	 input args:	
 *					$confile  : configure file path
 * return value:    <0: failure   0: success
 */
extern int readConf(char *confile);


/*****************************************************
*lower API, will just been used by fuction readConf()*
******************************************************/


/*	description:	get log system configurations
 *	 input args:	
 *					$confile  : configure file path
 *                  $log_conf : log system configurations struct 
 * return value:    <0: failure   0: success
 */
int readLogConf(char *confile, log_conf_t *log_conf);


/*	description:	get hardware configurations
 *	 input args:	
 *					$confile  : configure file path
 *                  $log_conf : hardware configurations struct 
 * return value:    <0: failure   0: success
 */
int readHardWareConf(char *confile, hardware_conf_t *hard_conf);


/*	description:	get mosquitto mqtt broker configurations
 *	 input args:	
 *					$confile  : configure file path
 *                  $log_conf : mosquitto mqtt broker configurations struct 
 * return value:    <0: failure   0: success
 */
int readBrokerConf(char *confile, broker_conf_t *bro_conf);


/*	description:	get mosquitto mqtt publisher configurations
 *	 input args:	
 *					$confile  : configure file path
 *                  $log_conf : mosquitto mqtt publisher configurations struct 
 * return value:    <0: failure   0: success
 */
int readPubConf(char *confile, puber_conf_t *pub_conf);

#endif