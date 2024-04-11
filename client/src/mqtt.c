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

int mqttInit(mqtt_ctx_t *mq) {

    // check input args
    if( !mq ) {
        return -1;
    }

	// init mosquitto lib
    if( mosquitto_lib_init() != MOSQ_ERR_SUCCESS ) {
        return -2;
    }

    // set mosq = NULL
    mq->mosq = NULL;

    return 0;
}

int mqttTerm(mqtt_ctx_t *mq) {

    // check input args
    if( !mq ) {
        return -1;
    }

    // clean mosquitto instance
    if( mq->mosq ) {
        mosquitto_destroy(mq->mosq);
        mq->mosq = NULL;
    }

    // clean mosquitto lib   
    /*if( mosquitto_lib_cleanup() != MOSQ_ERR_SUCCESS ) {
        return -2;
    }*/

    return 0;
}


int mqttConnect(mqtt_ctx_t *mq) {

    int                 rv = 0;
    char                service[24] = {0};
    struct addrinfo     hints = {0}, *res = NULL, *rp = NULL;
    struct in_addr      inaddr = {0};
    struct mosquitto	*tmp_mosq = NULL;
    
    // check input args
    if( !mq ) {
        return -1;
    }

    // make sure this mosquitto instance not already exist
    mqttTerm(mq);

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // if $host is an IP address, then don't need to translate domain name
    if( inet_aton(mq->conf.host, &inaddr) ) {
        hints.ai_flags |= AI_NUMERICHOST;
    }

    snprintf(service, sizeof(service), "%d", mq->conf.port);

    // translate domain name into IPv4 address
    if( (rv = getaddrinfo(mq->conf.host, service, &hints, &res)) ) {
        logError("translate domain name into IPv4 address failure\n");
        return -3;
    }

    // try every IPv4 address results to connect borker
    for(rp = res; rp != NULL; rp = rp->ai_next) {

        // create a new mosquitoo mqtt instance
        tmp_mosq = mosquitto_new(mq->conf.clientid, true, NULL);
        if( !tmp_mosq ) {
            logError("mosquitto_new() create failure\n");
            rv = -3;
            continue;
        }
        
        // set username and password
        mosquitto_username_pw_set(tmp_mosq, mq->conf.username, mq->conf.password);

        // connect to broker
        rv = mosquitto_connect(tmp_mosq, rp->ai_addr->sa_data, mq->conf.port, mq->conf.keepalive);
        if( rv == MOSQ_ERR_SUCCESS ) {
            mq->mosq = tmp_mosq;
            logInfo("connect to server[%s:%d] successfully!\n", mq->conf.host, mq->conf.port);
            break;
        }
        else {
            // socket connect get error, try another IP address
            mosquitto_destroy(tmp_mosq);
            continue;
        }
    }
    
    freeaddrinfo(res);
    return rv;
}

int mqttCheckConnect(mqtt_ctx_t *mq) {

	// check input args
	if( !mq ) {
		return -1;
	}
	
	// check if mosquitto mqtt connect to broker
	if( mosquitto_is_connected(mq->mosq) ) {
		return 0;
	}
	else {
		return -2;
	}
}

int mqttPublish(mqtt_ctx_t *mq, char *data, int bytes) {
	
	int			rv = 0;
	
	// check input args
	if( !mq || !data || bytes <= 0 ) {
		return -1;
	}
	
	// publish data to broker
	rv = mosquitto_publish(mq->mosq, NULL, mq->conf.pubtopic, bytes + 1, data, mq->conf.qos, false);
	if( rv != MOSQ_ERR_SUCCESS ) {
		logError("publish data to broker faliure\n");
		mqttTerm(mq);
		return -2;
	}
	
	return 0;
}
