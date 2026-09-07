#include "platform_socket.h"

#include <stddef.h>

void pika_platform_socket_initialize(PikaPlatformSocket* socket) {
    if (socket != NULL) {
        socket->handle = PIKA_SOCKET_INVALID_HANDLE;
        socket->timeout_milliseconds = -1;
    }
}

PikaSocketStatus pika_platform_socket_open(
    PikaPlatformSocket* socket,
    int family,
    int type,
    int protocol) {
    (void)family;
    (void)type;
    (void)protocol;
    if (socket == NULL) {
        return PIKA_SOCKET_INVALID_ARGUMENT;
    }
    socket->handle = PIKA_SOCKET_INVALID_HANDLE;
    return PIKA_SOCKET_IO_ERROR;
}

PikaSocketStatus pika_platform_socket_close(PikaPlatformSocket* socket) {
    if (socket == NULL) {
        return PIKA_SOCKET_INVALID_ARGUMENT;
    }
    pika_platform_socket_initialize(socket);
    return PIKA_SOCKET_OK;
}

PikaSocketStatus pika_platform_socket_bind(
    PikaPlatformSocket* socket,
    const char* host,
    uint16_t port) {
    (void)host;
    (void)port;
    return socket == NULL ? PIKA_SOCKET_INVALID_ARGUMENT : PIKA_SOCKET_CLOSED;
}

PikaSocketStatus pika_platform_socket_listen(
    PikaPlatformSocket* socket,
    int backlog) {
    (void)backlog;
    return socket == NULL ? PIKA_SOCKET_INVALID_ARGUMENT : PIKA_SOCKET_CLOSED;
}

PikaSocketStatus pika_platform_socket_accept(
    PikaPlatformSocket* server,
    PikaPlatformSocket* accepted,
    char* peer_host,
    uint32_t peer_host_capacity,
    uint16_t* peer_port) {
    (void)accepted;
    (void)peer_host;
    (void)peer_host_capacity;
    (void)peer_port;
    return server == NULL ? PIKA_SOCKET_INVALID_ARGUMENT : PIKA_SOCKET_CLOSED;
}

PikaSocketStatus pika_platform_socket_connect(
    PikaPlatformSocket* socket,
    const char* host,
    uint16_t port) {
    (void)host;
    (void)port;
    return socket == NULL ? PIKA_SOCKET_INVALID_ARGUMENT : PIKA_SOCKET_IO_ERROR;
}

PikaSocketStatus pika_platform_socket_send(
    PikaPlatformSocket* socket,
    const uint8_t* data,
    uint32_t length,
    uint32_t* sent) {
    (void)data;
    (void)length;
    if (sent != NULL) {
        *sent = 0u;
    }
    return socket == NULL || (length != 0u && data == NULL)
               ? PIKA_SOCKET_INVALID_ARGUMENT
               : PIKA_SOCKET_IO_ERROR;
}

PikaSocketStatus pika_platform_socket_receive(
    PikaPlatformSocket* socket,
    uint8_t* data,
    uint32_t capacity,
    uint32_t* received) {
    (void)data;
    (void)capacity;
    if (received != NULL) {
        *received = 0u;
    }
    return socket == NULL || (capacity != 0u && data == NULL)
               ? PIKA_SOCKET_INVALID_ARGUMENT
               : PIKA_SOCKET_IO_ERROR;
}

PikaSocketStatus pika_platform_socket_set_timeout(
    PikaPlatformSocket* socket,
    int32_t timeout_milliseconds) {
    if (socket == NULL || timeout_milliseconds < -1) {
        return PIKA_SOCKET_INVALID_ARGUMENT;
    }
    socket->timeout_milliseconds = timeout_milliseconds;
    return PIKA_SOCKET_OK;
}

PikaSocketStatus pika_platform_socket_resolve_ipv4(
    const char* host,
    char* address,
    uint32_t address_capacity) {
    (void)host;
    if (address == NULL || address_capacity < PIKA_SOCKET_ADDRESS_BYTE_LIMIT) {
        return PIKA_SOCKET_INVALID_ARGUMENT;
    }
    address[0] = '\0';
    return PIKA_SOCKET_DNS_ERROR;
}

PikaSocketStatus pika_platform_socket_local_port(
    PikaPlatformSocket* socket,
    uint16_t* port) {
    if (socket == NULL || port == NULL) {
        return PIKA_SOCKET_INVALID_ARGUMENT;
    }
    *port = 0u;
    return PIKA_SOCKET_CLOSED;
}

PikaSocketStatus pika_platform_socket_local_host(
    PikaPlatformSocket* socket,
    char* host,
    uint32_t host_capacity) {
    if (socket == NULL || host == NULL ||
        host_capacity < PIKA_SOCKET_ADDRESS_BYTE_LIMIT) {
        return PIKA_SOCKET_INVALID_ARGUMENT;
    }
    host[0] = '\0';
    return PIKA_SOCKET_CLOSED;
}
