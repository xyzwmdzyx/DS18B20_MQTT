/********************************************************************************
 *      Copyright:  (C) 2024 Company
 *                  All rights reserved.
 *
 *       Filename:  ds18b20.h
 *    Description:  This file is a get ds18b20 temper function declare file.
 *
 *        Version:  1.0.0(2024年03月26日)
 *         Author:  WangMingda <wmd.de.zhanghu@gmail.com>
 *      ChangeLog:  1, Release initial version on "2024年03月26日 20时58分59秒"
 *
 * Pin connection:
 *
 *               DS18B20 Module          	Raspberry Pi Board
 *               	VCC      	 <----->       #Pin1(3.3V)
 *               	DQ       	 <----->       #Pin7(BCM GPIO4)
 *               	GND      	 <----->       GND
 *
 * /boot/config.txt:
 *
 *          dtoverlay=w1-gpio-pullup,gpiopin=4
 *                 
 ********************************************************************************/

#ifndef  _DS18B20_H_
#define  _DS18B20_H_

extern int ds18b20GetTemperature(float *temp);

#endif
