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

int mqttInit(mqtt_ctx_t *mq, char *host, int port, char) {

    // check input args
    if( !host || port <= 0 ) {
        return -1;
    }

	// init mosquitto lib
    if( mosquitto_lib_init() != MOSQ_ERR_SUCCESS ) {
        return -2;
    }

    // assignment host, port
    strncpy(mq->host, host, HOSTNAME_LEN); 
	mq->port = port;
    mq->msoq = NULL;

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
    if( mosquitto_lib_cleanup() != MOSQ_ERR_SUCCESS ) {
        return -2;
    }

    return 0;
}


int mqttConnect(mqtt_ctx_t *mq) {

    int                 rv = 0;
    char                service[24] = {0};
    struct addrinfo     hints = {0}, *res = NULL, *rp = NULL;
    struct in_addr      inaddr = {0};
    
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
    if( inet_aton(mq->host, &inaddr) ) {
        hints.ai_flags |= AI_NUMERICHOST;
    }

    snprintf(service, sizeof(service), "%d", mq->port);

    // translate domain name into IPv4 address
    if( (rv = getaddrinfo(mq->host, service, &hints, &res)) ) {
        logError("translate domain name into IPv4 address failure\n");
        return -3;
    }

    // try every IPv4 address results to connect borker
    for(rp = res; rp != NULL; rp = rp->ai_next) {

        // create a new mosquitoo mqtt instance
        mq->mosq = mosquitto_new();
        if( sockfd < 0) {
            log_error("socket() create failed: %s\n", strerror(errno));
            rv = -3;
            continue;
        }

        /* connect to server */
        rv = connect(sockfd, rp->ai_addr, len);
        if( 0 == rv )
        {
            sock->fd = sockfd;
            log_info("Connect to server[%s:%d] on fd[%d] successfully!\n", sock->host, sock->port, sockfd);
            break;
        }
        else
        {
            /* socket connect get error, try another IP address */
            close(sockfd);
            continue;
        }
    }
}
