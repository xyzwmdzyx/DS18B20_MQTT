/*********************************************************************************
 *      Copyright:  (C) 2024 Company
 *                  All rights reserved.
 *
 *       Filename:  packet.c
 *    Description:  This file is a pack data function file.
 *                 
 *        Version:  1.0.0(2024年03月25日)
 *         Author:  WangMingda <wmd.de.zhanghu@gmail.com>
 *      ChangeLog:  1, Release initial version on "2024年03月25日 22时51分14秒"
 *                 
 ********************************************************************************/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "packet.h"
#include "logger.h"
#include "ds18b20.h"


/*	description:	get sample time
 *	 input args:	
 *					$sample_time : time information buffer
 *					$size        : buffer size
 * return value:    <0: failure   0: success
 */
int getTime(char *sample_time, int size) {
       
    time_t 			t = time(NULL);
    struct tm 		*tm = localtime(&t);
    
    // check input args
    if( !sample_time || size < 0) {
    	logError("function %s() gets invalid input arugments\n", __func__);
    	return -1;
    }
    
    memset(sample_time, 0, size);
    strftime(sample_time, size, "%Y-%m-%d %H:%M:%S", tm);
    
    return 0;
}


/*	description:	packet segment data into text, include device ID, sample time, sample temper
 *	 input args:	
 *					$pack_info : struct whitch store segment data
 *                  $pack_buf  : buffer whitch will store packeted data
 *                  $size      : buffer size 
 * return value:    <0: failure   >0: success
 */
int packetSegmentData(pack_info_t *pack_info, char *pack_buf, int size) {

    // check input args
    if( !pack_info || !pack_buf || size <= 0 ) {
        logError("function %s() gets invalid input arguments\n", __func__);
        return -1;
    }
    
    memset(pack_buf, 0, size);
    snprintf(pack_buf, size, "%s,%s,%.2f", pack_info->devid, pack_info->sample_time, pack_info->temper);

    return strlen(pack_buf);
}


/*	description:	packet segment data into json, include device ID, sample time, sample temper
 *	 input args:	
 *					$pack_info : struct whitch store segment data
 *                  $pack_buf  : buffer whitch will store packeted data
 *                  $size      : buffer size 
 * return value:    <0: failure   >0: success
 */
int packetJsonData(pack_info_t *pack_info, char *pack_buf, int size, int platform) {

    // check input args
    if( !pack_info || !pack_buf || size <= 0 ) {
        logError("function %s() gets invalid input arguments\n");
        return -1;
    }

    memset(pack_buf, 0, size);
    
    // packet data into JSON
    if( platform == 1 ) {
    	snprintf(pack_buf, size, "{\"services\": [{\"service_id\": \"1\",\"properties\": {\"temperature\": %.2f}}]}", pack_info->temper);
    }
    else if( platform == 2 ) {
    	snprintf(pack_buf, size, "{\"params\": {\"temperature\": %.2f}}", pack_info->temper);
    }
	else if( platform == 3 ) {
		snprintf(pack_buf, size, "{\"type\": \"update\",\"state\": {\"reported\": {\"temperature\": %.2f}},\"version\": 1,   \"clientToken\": \"clientToken\"", pack_info->temper);
	}
	
    return strlen(pack_buf);
}
