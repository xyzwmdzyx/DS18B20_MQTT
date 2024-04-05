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

#define HOSTNAME_LEN	256
#define CLIENTID_LEN    128
#define USERNAME_LEN    128
#define PASSWORD_LEN    128

typedef struct conf_s {
    
    char        	host[HOSTNAME_LEN];         // broker hostname
    int         	port;                       // broker port
    char            clientid[CLIENTID_LEN];     // client id
    char            username[USERNAME_LEN];     // user name
    char            password[PASSWORD_LEN];     // pass word

}conf_t;

int readConf(char *confile, conf_t *conf);

#endif