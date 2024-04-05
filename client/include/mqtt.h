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
#include "mqtt.h"
#include "readconf.h"

typedef struct mqtt_ctx_s {

    struct conf_t       conf;   // client configure
    struct mosquitto 	*mosq;  // mosquitto mqtt instance
} mqtt_ctx_t;



#endif
