#include "_socket_socket.h"
#include "platform_socket.h"

#if !PIKASCRIPT_VERSION_REQUIRE_MINIMUN(1, 10, 4)
#error "This library requires PikaScript version 1.10.4 or higher"
#endif

/*
    The functinos start with PIKA_WEAK are weak functions,
    you need to override them in your platform.
*/

PIKA_WEAK int __platform_socket(int __domain, int __type, int __protocol) {
#ifdef __linux__
    return socket(__domain, __type, __protocol);
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
#endif
}

PIKA_WEAK int __platform_bind(int __fd,
                              const struct sockaddr* __addr,
                              socklen_t __addr_len) {
#ifdef __linux__
    return bind(__fd, __addr, __addr_len);
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
#endif
}

PIKA_WEAK int __platform_listen(int __fd, int __n) {
#ifdef __linux__
    return listen(__fd, __n);
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
#endif
}

PIKA_WEAK int __platform_accept(int __fd,
                                struct sockaddr* __addr,
                                socklen_t* __addr_len) {
#ifdef __linux__
    return accept(__fd, __addr, __addr_len);
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
#endif
}

PIKA_WEAK int __platform_connect(int __fd,
                                 const struct sockaddr* __addr,
                                 socklen_t __addr_len) {
#ifdef __linux__
    return connect(__fd, __addr, __addr_len);
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
#endif
}

PIKA_WEAK int __platform_send(int __fd,
                              const void* __buf,
                              size_t __n,
                              int __flags) {
#ifdef __linux__
    return send(__fd, __buf, __n, __flags);
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
#endif
}

PIKA_WEAK int __platform_recv(int __fd, void* __buf, size_t __n, int __flags) {
#ifdef __linux__
    return recv(__fd, __buf, __n, __flags);
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
#endif
}

PIKA_WEAK int __platform_close(int __fd) {
#ifdef __linux__
    return close(__fd);
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
#endif
}

/* gethostname */
PIKA_WEAK int __platform_gethostname(char* __name, size_t __len) {
#ifdef __linux__
    return gethostname(__name, __len);
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
#endif
}

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
        obj_setErrorCode(self, PIKA_RES_ERR_RUNTIME_ERROR);
        __platform_printf("recv error\n");
        return NULL;
    }
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
