#include "_socket_socket.h"
#ifdef __linux__
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

void _socket_socket__init(PikaObj* self) {
    int family = obj_getInt(self, "family");
    int type = obj_getInt(self, "type");
    int protocol = obj_getInt(self, "protocol");
    int sockfd = 0;
#ifdef __linux__
    sockfd = socket(family, type, protocol);
#endif
    if (sockfd < 0) {
        obj_setErrorCode(self, PIKA_RES_ERR_RUNTIME_ERROR);
        __platform_printf("socket error\n");
        return;
    }
    obj_setInt(self, "sockfd", sockfd);
}

void _socket_socket__close(PikaObj* self) {
    int sockfd = obj_getInt(self, "sockfd");
#ifdef __linux__
    close(sockfd);
#endif
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
#ifdef __linux__
    ret = send(sockfd, data_send, len, 0);
#endif
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
#ifdef __linux__
    client_sockfd =
        accept(sockfd, (struct sockaddr*)&client_addr, &client_addr_len);
#endif
    if (client_sockfd < 0) {
        obj_setErrorCode(self, PIKA_RES_ERR_RUNTIME_ERROR);
        __platform_printf("accept error\n");
        return;
    }
    obj_setInt(self, "client_sockfd", client_sockfd);
    obj_setStr(self, "client_addr", inet_ntoa(client_addr.sin_addr));
}

char* _socket_socket__recv(PikaObj* self, int num) {
    int sockfd = obj_getInt(self, "sockfd");
    int ret = 0;
    char* data = NULL;
    uint8_t* data_recv = NULL;
#ifdef __linux__
    obj_setBytes(self, "_recv_data", NULL, num);
    data_recv = obj_getBytes(self, "_recv_data");
    ret = recv(sockfd, data_recv, num, 0);
#endif
    if (ret < 0) {
        obj_setErrorCode(self, PIKA_RES_ERR_RUNTIME_ERROR);
        __platform_printf("recv error\n");
        return NULL;
    }
    data = (char*)data_recv;
    return data;
}

void _socket_socket__listen(PikaObj* self, int num) {
    int sockfd = obj_getInt(self, "sockfd");
#ifdef __linux__
    listen(sockfd, num);
#endif
}

void _socket_socket__connect(PikaObj* self, char* host, int port) {
    int sockfd = obj_getInt(self, "sockfd");
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(host);
#ifdef __linux__
    connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
#endif
}

void _socket_socket__bind(PikaObj* self, char* host, int port) {
    int sockfd = obj_getInt(self, "sockfd");
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(host);
#ifdef __linux__
    bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
#endif
}

char* _socket_socket__gethostname(PikaObj* self) {
    char hostname_buff[128] = {0};
    char* hostname = (char*)hostname_buff;
#ifdef __linux__
    gethostname(hostname_buff, 128);
#endif
    return hostname;
}
