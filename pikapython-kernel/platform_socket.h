/* SPEC: PJ2026-050109 network-ready v0.1; portable socket boundary. */
#ifndef PIKA_PLATFORM_SOCKET_H
#define PIKA_PLATFORM_SOCKET_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PIKA_SOCKET_AF_INET 2
#define PIKA_SOCKET_STREAM 1
#define PIKA_SOCKET_INVALID_HANDLE ((intptr_t)-1)
#define PIKA_SOCKET_ADDRESS_BYTE_LIMIT 16u

#ifndef PIKA_SOCKET_HOST_BYTE_LIMIT
#define PIKA_SOCKET_HOST_BYTE_LIMIT 255u
#endif
#ifndef PIKA_SOCKET_OBJECT_LIMIT
#define PIKA_SOCKET_OBJECT_LIMIT 8u
#endif
#ifndef PIKA_SOCKET_RECEIVE_BYTE_LIMIT
#define PIKA_SOCKET_RECEIVE_BYTE_LIMIT 256u
#endif

#if PIKA_SOCKET_OBJECT_LIMIT < 1u
#error "PIKA_SOCKET_OBJECT_LIMIT must be at least 1"
#endif
#if PIKA_SOCKET_RECEIVE_BYTE_LIMIT < 1u
#error "PIKA_SOCKET_RECEIVE_BYTE_LIMIT must be at least 1"
#endif

typedef enum {
    PIKA_SOCKET_OK = 0,
    PIKA_SOCKET_INVALID_ARGUMENT = 1,
    PIKA_SOCKET_CLOSED = 2,
    PIKA_SOCKET_WOULD_BLOCK = 3,
    PIKA_SOCKET_TIMEOUT = 4,
    PIKA_SOCKET_DNS_ERROR = 5,
    PIKA_SOCKET_CONNECTION_REFUSED = 6,
    PIKA_SOCKET_CONNECTION_RESET = 7,
    PIKA_SOCKET_IO_ERROR = 8,
} PikaSocketStatus;

typedef struct {
    intptr_t handle;
    int32_t timeout_milliseconds;
} PikaPlatformSocket;

void pika_platform_socket_initialize(PikaPlatformSocket* socket);
PikaSocketStatus pika_platform_socket_open(
    PikaPlatformSocket* socket,
    int family,
    int type,
    int protocol);
PikaSocketStatus pika_platform_socket_close(
    PikaPlatformSocket* socket);
PikaSocketStatus pika_platform_socket_bind(
    PikaPlatformSocket* socket,
    const char* host,
    uint16_t port);
PikaSocketStatus pika_platform_socket_listen(
    PikaPlatformSocket* socket,
    int backlog);
PikaSocketStatus pika_platform_socket_accept(
    PikaPlatformSocket* server,
    PikaPlatformSocket* accepted,
    char* peer_host,
    uint32_t peer_host_capacity,
    uint16_t* peer_port);
PikaSocketStatus pika_platform_socket_connect(
    PikaPlatformSocket* socket,
    const char* host,
    uint16_t port);
PikaSocketStatus pika_platform_socket_send(
    PikaPlatformSocket* socket,
    const uint8_t* data,
    uint32_t length,
    uint32_t* sent);
PikaSocketStatus pika_platform_socket_receive(
    PikaPlatformSocket* socket,
    uint8_t* data,
    uint32_t capacity,
    uint32_t* received);
PikaSocketStatus pika_platform_socket_set_timeout(
    PikaPlatformSocket* socket,
    int32_t timeout_milliseconds);
PikaSocketStatus pika_platform_socket_resolve_ipv4(
    const char* host,
    char* address,
    uint32_t address_capacity);
PikaSocketStatus pika_platform_socket_local_port(
    PikaPlatformSocket* socket,
    uint16_t* port);
PikaSocketStatus pika_platform_socket_local_host(
    PikaPlatformSocket* socket,
    char* host,
    uint32_t host_capacity);

#ifdef __cplusplus
}
#endif

#endif
