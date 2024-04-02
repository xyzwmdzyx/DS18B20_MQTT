/********************************************************************************
 *      Copyright:  (C) 2024 Company
 *                  All rights reserved.
 *
 *       Filename:  mqtt.h
 *    Description:  This file is a mqtt fuction declare file.
 *
 *        Version:  1.0.0(2024年04月02日)
 *         Author:  WangMingda <wmd.de.zhanghu@gmail.com>
 *      ChangeLog:  1, Release initial version on "2024年04月02日 22时33分47秒"
 *                 
 ********************************************************************************/

#ifndef _MQTT_H_
#define _MQTT_H_

#include <mosquitto.h>

#define HOSTNAME_LEN	256

typedef struct mqtt_ctx_s
{
    char        		host[HOSTNAME_LEN]; // broker hostname
    int         		port;               // broker port
    struct mosquitto 	*mosq;              // mosquitto mqtt descriptor
} mqtt_ctx_t;



#endif
