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

typedef struct conf_s {

	/*device and hardware configurations*/
	
    char            deviceid[16];       // device id
    int             ds18b20;            // ds18b20 = 1 means this hardware exist

	/*mosquitto mqtt broker configurations*/	
	    
    char        	host[256];          // broker hostname
    int         	port;               // broker port
    char            clientid[128];      // client id
    char            username[128];      // user name
    char            password[128];      // pass word

	/*mosquitto mqtt publisher configuations*/	
	
    char            pubtopic[256];      // publish topic
    int             readtime;           // sample interval tim

}conf_t;


/*	description:	get configurations
 *	 input args:	
 *					$confile  : configure file path
 *					$conf	  : configure struct
 * return value:    <0: failure   0: success
 */
extern int readConf(char *confile, conf_t *conf);

#endif
