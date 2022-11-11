/*
 * @Author       : jiejie
 * @GitHub       : https://github.com/jiejieTop
 * @Date         : 2021-02-26 12:00:24
 * @LastEditors  : jiejie
 * @LastEditTime : 2022-06-15 19:48:43
 * @FilePath     : /mqttclient/network/nettype_tls.h
 * Copyright (c) 2022 jiejie, All Rights Reserved. Please keep the author information and source code according to the license.
 */
/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2020-01-11 19:45:44
 * @LastEditTime: 2020-10-17 14:14:11
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */

#ifndef _NETTYPE_TLS_H_
#define _NETTYPE_TLS_H_

#include "mqtt_defconfig.h"
#include "network.h"
#include "mqtt_error.h"
#include "mqtt_log.h"

#ifndef MQTT_NETWORK_TYPE_NO_TLS

#include "mbedtls/config.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"
#include "mbedtls/debug.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct nettype_tls_params {
    mbedtls_net_context         socket_fd;        /**< mbed TLS network context. */
    mbedtls_entropy_context     entropy;          /**< mbed TLS entropy. */
    mbedtls_ctr_drbg_context    ctr_drbg;         /**< mbed TLS ctr_drbg. */
    mbedtls_ssl_context         ssl;              /**< mbed TLS control context. */
    mbedtls_ssl_config          ssl_conf;         /**< mbed TLS configuration context. */
#if defined(MBEDTLS_X509_CRT_PARSE_C)
    mbedtls_x509_crt            ca_cert;          /**< mbed TLS CA certification. */
    mbedtls_x509_crt            client_cert;      /**< mbed TLS Client certification. */
#endif
    mbedtls_pk_context          private_key;      /**< mbed TLS Client key. */
} nettype_tls_params_t;

int nettype_tls_read(network_t *n, unsigned char *buf, int len, int timeout);
int nettype_tls_write(network_t *n, unsigned char *buf, int len, int timeout);
int nettype_tls_connect(network_t* n);
void nettype_tls_disconnect(network_t* n);

#endif /* MQTT_NETWORK_TYPE_NO_TLS */

#ifdef __cplusplus
}
#endif

#endif
