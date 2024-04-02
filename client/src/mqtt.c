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


#include <mosquitto.h>

int mosqInit(mqtt_ctx_t *mq, char *host, int port) {

    // check input args
    if( !host || port <= 0 ) {
        return -1;
    }

    if( mosquitto_lib_init() != MOSQ_ERR_SUCCESS ) {
        return -2;
    }

    // assignment host, port
    strncpy(mq->host, host, HOSTNAME_LEN); 
	mq->port = port;
    mq->msoq = NULL;

    return 0;
}

int mosqTerm(mqtt_ctx_t *mq) {

    // check input args
    if( !mq ) {
        return -1;
    }

    // clean resource    
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();

    return 0;
}
