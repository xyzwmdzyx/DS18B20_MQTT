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
#include "readconf.h"

typedef struct mqtt_ctx_s {

    conf_t       		conf;   // client configure
    struct mosquitto 	*mosq;  // mosquitto mqtt instance
} mqtt_ctx_t;


/*	description:	init mosquitto mqtt
 *	 input args:	
 *					$mq  : mosquitto mqtt context
 * return value:    <0: failure   0: success
 */
extern int mqttInit(mqtt_ctx_t *mq);


/*	description:	terminate mosquitto mqtt
 *	 input args:	
 *					$mq  : mosquitto mqtt context
 * return value:    <0: failure   0: success
 */
extern int mqttTerm(mqtt_ctx_t *mq);


/*	description:	mosquitto mqtt client connect to broker
 *	 input args:	
 *					$mq  : mosquitto mqtt context
 * return value:    <0: failure   0: success
 */
extern int mqttConnect(mqtt_ctx_t *mq);


/*	description:	check if mosquitto mqtt client connect to broker or not
 *	 input args:	
 *					$mq  : mosquitto mqtt context
 * return value:    <0: failure   0: success
 */
extern int mqttCheckConnect(mqtt_ctx_t *mq);


/*	description:	mosquitto mqtt client publish data to broker
 *	 input args:	
 *					$mq  : mosquitto mqtt context
 * return value:    <0: failure   0: success
 */
extern int mqttPublish(mqtt_ctx_t *mq, char *data, int bytes);

#endif
