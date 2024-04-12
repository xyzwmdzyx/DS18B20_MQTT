/*********************************************************************************
 *      Copyright:  (C) 2024 Company
 *                  All rights reserved.
 *
 *       Filename:  mqtt.c
 *    Description:  This file is a mqtt function file.
 *                 
 *        Version:  1.0.0(2024年04月02日)
 *         Author:  WangMingda <wmd.de.zhanghu@gmail.com>
 *      ChangeLog:  1, Release initial version on "2024年04月02日 22时16分57秒"
 *                 
 ********************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/un.h>
#include <poll.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/sockios.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/resource.h>
#include <mosquitto.h>
#include "mqtt.h"
#include "readconf.h"
#include "logger.h"


/*	description:	init mosquitto mqtt
 *	 input args:	
 *					$mosq  : mosquitto mqtt pointer
 * return value:    <0: failure   0: success
 */
int mqttInit(struct mosquitto *mosq) {
	
	// init mosquitto lib
    if( mosquitto_lib_init() != MOSQ_ERR_SUCCESS ) {
        return -1;
    }

    // set mosq = NULL
    mosq = NULL;

    return 0;
}


/*	description:	terminate mosquitto mqtt
 *	 input args:	
 *					$mosq  : mosquitto mqtt pointer
 * return value:    <0: failure   0: success
 */
int mqttTerm(struct mosquitto *mosq) {

    // clean mosquitto instance and set mosq = NULL
    if( mosq ) {
        mosquitto_destroy(mosq);
        mosq = NULL;
    }
    
    return 0;
}


/*	description:	mosquitto mqtt client connect to broker
 *	 input args:	
 *					$mosq  : mosquitto mqtt pointer
 *					$conf  : client configurations
 * return value:    <0: failure   0: success
 */
int mqttConnect(struct mosquitto *mosq, conf_t *conf) {

    int                 rv = 0;
    char                service[24] = {0};
    struct addrinfo     hints = {0}, *res = NULL, *rp = NULL;
    struct in_addr      inaddr = {0};
    struct mosquitto	*tmp_mosq = NULL;
    
    // check input args
    if( !mosq || !conf ) {
        return -1;
    }

    // make sure this mosquitto instance not already exist
    mqttTerm(mosq);

    // create a new mosquitoo mqtt instance
    tmp_mosq = mosquitto_new(conf->clientid, true, NULL);
    if( !tmp_mosq ) {
        logError("mosquitto_new() create failure\n");
        mosquitto_destroy(tmp_mosq);
        return -2;
    }
        
    // set username and password
    mosquitto_username_pw_set(tmp_mosq, conf->username, conf->password);

    // connect to broker
    rv = mosquitto_connect(tmp_mosq, conf->host, conf->port, conf->keepalive);
    if( rv != MOSQ_ERR_SUCCESS ) {
     	// connect get error
     	logError("mosquitto_connect() connect to broker faliure\n");
        mqttTerm(tmp_mosq);
        return -3;
    }
    mosq = tmp_mosq;
    logInfo("connect to broker success\n");
    
    return 0;
}


/*	description:	mosquitto mqtt client publish data to broker
 *	 input args:	
 *					$mosq  : mosquitto mqtt pointer
 *					$data  : packeted data(JSON)
 *					$bytes : data total bytes
 * return value:    <0: failure   0: success
 */
int mqttPublish(struct mosquitto *mosq, char *data, int bytes) {
	
	int			rv = 0;
	
	// check input args
	if( !mosq || !data || bytes <= 0 ) {
		return -1;
	}
	
	// publish data to broker
	rv = mosquitto_publish(mosq, NULL, conf->pubtopic, bytes + 1, data, conf->qos, false);
	if( rv != MOSQ_ERR_SUCCESS ) {
		logError("publish data to broker faliure\n");
		mqttTerm(mosq);
		return -2;
	}
	logInfo("publish data to broker success\n");
	
	return 0;
}
