#include "PikaPlatform_socket.h"
#include "_socket_socket.h"

#if !PIKASCRIPT_VERSION_REQUIRE_MINIMUN(1, 12, 0)
#error "This library requires PikaScript version 1.12.0 or higher"
#endif

void _socket_socket__init(PikaObj* self) {
    int family = obj_getInt(self, "family");
    int type = obj_getInt(self, "type");
    int protocol = obj_getInt(self, "protocol");
    int sockfd = 0;
    sockfd = __platform_socket(family, type, protocol);
    if (sockfd < 0) {
        obj_setErrorCode(self, PIKA_RES_ERR_RUNTIME_ERROR);
        __platform_printf("socket error\n");
        return;
    }
    obj_setInt(self, "sockfd", sockfd);
    obj_setInt(self, "blocking", 1);
}

void _socket_socket__close(PikaObj* self) {
    int sockfd = obj_getInt(self, "sockfd");
    __platform_close(sockfd);
}

void _socket_socket__send(PikaObj* self, Arg* data) {
    uint8_t* data_send = NULL;
    int len = 0;
    if (arg_getType(data) == ARG_TYPE_STRING) {
        data_send = (uint8_t*)arg_getStr(data);
        len = strGetSize((char*)data_send);
    }

    if (arg_getType(data) == ARG_TYPE_BYTES) {
        data_send = (uint8_t*)arg_getBytes(data);
        len = arg_getBytesSize(data);
    }

    int sockfd = obj_getInt(self, "sockfd");
    int ret = 0;
    ret = __platform_send(sockfd, data_send, len, 0);
    if (ret < 0) {
        obj_setErrorCode(self, PIKA_RES_ERR_RUNTIME_ERROR);
        __platform_printf("send error\n");
        return;
    }
}

void _socket_socket__accept(PikaObj* self) {
    int sockfd = obj_getInt(self, "sockfd");
    int client_sockfd = 0;
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    client_sockfd = __platform_accept(sockfd, (struct sockaddr*)&client_addr,
                                      &client_addr_len);
    if (client_sockfd < 0) {
        obj_setErrorCode(self, PIKA_RES_ERR_RUNTIME_ERROR);
        __platform_printf("accept error\n");
        return;
    }
    obj_setInt(self, "client_sockfd", client_sockfd);
    obj_setStr(self, "client_addr", inet_ntoa(client_addr.sin_addr));
}

Arg* _socket_socket__recv(PikaObj* self, int num) {
    int sockfd = obj_getInt(self, "sockfd");
    int ret = 0;
    uint8_t* data_recv = NULL;
    Arg* res = arg_newBytes(NULL, num);
    data_recv = arg_getBytes(res);
    ret = __platform_recv(sockfd, data_recv, num, 0);
    if (ret < 0) {
        if (obj_getInt(self, "blocking")) {
            obj_setErrorCode(self, PIKA_RES_ERR_RUNTIME_ERROR);
            __platform_printf("recv error\n");
            return NULL;
        }else{
            Arg* res_r = arg_newBytes(NULL, 0);
            arg_deinit(res);
            return res_r;
        }
    } else {
        if (ret < num) {
            uint8_t* res_buff = NULL;
            Arg* res_r = arg_newBytes(NULL, ret);
            res_buff = arg_getBytes(res_r);
            pika_platform_memcpy(res_buff, data_recv, ret);
            arg_deinit(res);
            __platform_printf("recv return res_r\n");
            return res_r;
        }
    }
    __platform_printf("recv return res\n");
    return res;
}

void _socket_socket__listen(PikaObj* self, int num) {
    int sockfd = obj_getInt(self, "sockfd");
    __platform_listen(sockfd, num);
}

void _socket_socket__connect(PikaObj* self, char* host, int port) {
    int sockfd = obj_getInt(self, "sockfd");
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(host);
    __platform_connect(sockfd, (struct sockaddr*)&server_addr,
                       sizeof(server_addr));
    if (obj_getInt(self, "blocking") == 0) {
        int flags = fcntl(sockfd, F_GETFL);
        if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1) {
            obj_setErrorCode(self, PIKA_RES_ERR_RUNTIME_ERROR);
            __platform_printf("Unable to set socket non blocking\n");
            return;
        }
    }
}

void _socket_socket__bind(PikaObj* self, char* host, int port) {
    int sockfd = obj_getInt(self, "sockfd");
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(host);
    int res = __platform_bind(sockfd, (struct sockaddr*)&server_addr,
                              sizeof(server_addr));
    if (res < 0) {
        obj_setErrorCode(self, PIKA_RES_ERR_RUNTIME_ERROR);
        __platform_printf("bind error\n");
        return;
    }
}

char* _socket__gethostname(PikaObj* self) {
    char hostname_buff[128] = {0};
    char* hostname = (char*)hostname_buff;
    __platform_gethostname(hostname_buff, 128);
    return obj_cacheStr(self, hostname);
}

void _socket_socket__setblocking(PikaObj* self, int sta) {
    obj_setInt(self, "blocking", sta);
}