/* SPEC: PJ2026-050109 network-ready v0.3; host IPv4 TCP port. */
#if defined(__linux__) && !defined(_POSIX_C_SOURCE)
#define _POSIX_C_SOURCE 200112L
#endif

#include "platform_socket.h"

#if defined(__linux__)

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <netdb.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

static int socket_is_open(const PikaPlatformSocket* socket) {
    return socket != NULL &&
           socket->handle != PIKA_SOCKET_INVALID_HANDLE;
}

static PikaSocketStatus socket_error_status(
    const PikaPlatformSocket* socket,
    int error) {
    if (error == EAGAIN || error == EWOULDBLOCK ||
        error == EINPROGRESS || error == EALREADY) {
        return socket != NULL &&
                       socket->timeout_milliseconds > 0
                   ? PIKA_SOCKET_TIMEOUT
                   : PIKA_SOCKET_WOULD_BLOCK;
    }
    if (error == ETIMEDOUT) {
        return PIKA_SOCKET_TIMEOUT;
    }
    if (error == ECONNREFUSED) {
        return PIKA_SOCKET_CONNECTION_REFUSED;
    }
    if (error == ECONNRESET || error == EPIPE ||
        error == ENOTCONN) {
        return PIKA_SOCKET_CONNECTION_RESET;
    }
    return PIKA_SOCKET_IO_ERROR;
}

static PikaSocketStatus resolve_address(
    const char* host,
    uint16_t port,
    int passive,
    struct sockaddr_in* address) {
    struct addrinfo hints;
    struct addrinfo* result = NULL;
    struct addrinfo* current;
    char service[6];
    int length;
    int status;
    size_t host_length;
    if (host == NULL || address == NULL) {
        return PIKA_SOCKET_INVALID_ARGUMENT;
    }
    host_length = strlen(host);
    if (host_length > PIKA_SOCKET_HOST_BYTE_LIMIT) {
        return PIKA_SOCKET_INVALID_ARGUMENT;
    }
    length = snprintf(service, sizeof(service), "%u",
                      (unsigned int)port);
    if (length <= 0 || (size_t)length >= sizeof(service)) {
        return PIKA_SOCKET_INVALID_ARGUMENT;
    }
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = passive ? AI_PASSIVE : 0;
    status = getaddrinfo(
        host_length == 0u ? NULL : host, service, &hints, &result);
    if (status != 0 || result == NULL) {
        if (result != NULL) {
            freeaddrinfo(result);
        }
        return PIKA_SOCKET_DNS_ERROR;
    }
    for (current = result; current != NULL;
         current = current->ai_next) {
        if (current->ai_family == AF_INET &&
            current->ai_addrlen >=
                (socklen_t)sizeof(struct sockaddr_in)) {
            memcpy(address, current->ai_addr, sizeof(*address));
            freeaddrinfo(result);
            return PIKA_SOCKET_OK;
        }
    }
    freeaddrinfo(result);
    return PIKA_SOCKET_DNS_ERROR;
}

void pika_platform_socket_initialize(PikaPlatformSocket* socket) {
    if (socket != NULL) {
        socket->handle = PIKA_SOCKET_INVALID_HANDLE;
        socket->timeout_milliseconds = -1;
    }
}

PikaSocketStatus pika_platform_socket_open(
    PikaPlatformSocket* endpoint,
    int family,
    int type,
    int protocol) {
    int handle;
    if (endpoint == NULL ||
        endpoint->handle != PIKA_SOCKET_INVALID_HANDLE ||
        family != PIKA_SOCKET_AF_INET ||
        type != PIKA_SOCKET_STREAM ||
        (protocol != 0 && protocol != IPPROTO_TCP)) {
        return PIKA_SOCKET_INVALID_ARGUMENT;
    }
    handle = socket(AF_INET, SOCK_STREAM, protocol);
    if (handle < 0) {
        return socket_error_status(endpoint, errno);
    }
    endpoint->handle = handle;
    endpoint->timeout_milliseconds = -1;
    return PIKA_SOCKET_OK;
}

PikaSocketStatus pika_platform_socket_close(
    PikaPlatformSocket* socket) {
    int status;
    int error;
    if (socket == NULL) {
        return PIKA_SOCKET_INVALID_ARGUMENT;
    }
    if (socket->handle == PIKA_SOCKET_INVALID_HANDLE) {
        return PIKA_SOCKET_OK;
    }
    status = close(socket->handle);
    error = errno;
    socket->handle = PIKA_SOCKET_INVALID_HANDLE;
    socket->timeout_milliseconds = -1;
    return status == 0 || error == EINTR
               ? PIKA_SOCKET_OK
               : PIKA_SOCKET_IO_ERROR;
}

PikaSocketStatus pika_platform_socket_bind(
    PikaPlatformSocket* socket,
    const char* host,
    uint16_t port) {
    struct sockaddr_in address;
    PikaSocketStatus status;
    int result;
    int enabled = 1;
    if (!socket_is_open(socket)) {
        return PIKA_SOCKET_CLOSED;
    }
    status = resolve_address(host, port, 1, &address);
    if (status != PIKA_SOCKET_OK) {
        return status;
    }
    (void)setsockopt(
        socket->handle, SOL_SOCKET, SO_REUSEADDR,
        &enabled, (socklen_t)sizeof(enabled));
    do {
        result = bind(
            socket->handle, (const struct sockaddr*)&address,
            (socklen_t)sizeof(address));
    } while (result != 0 && errno == EINTR);
    return result == 0
               ? PIKA_SOCKET_OK
               : socket_error_status(socket, errno);
}

PikaSocketStatus pika_platform_socket_listen(
    PikaPlatformSocket* socket,
    int backlog) {
    int result;
    if (!socket_is_open(socket)) {
        return PIKA_SOCKET_CLOSED;
    }
    if (backlog < 0) {
        return PIKA_SOCKET_INVALID_ARGUMENT;
    }
    do {
        result = listen(socket->handle, backlog);
    } while (result != 0 && errno == EINTR);
    return result == 0
               ? PIKA_SOCKET_OK
               : socket_error_status(socket, errno);
}

PikaSocketStatus pika_platform_socket_accept(
    PikaPlatformSocket* server,
    PikaPlatformSocket* accepted,
    char* peer_host,
    uint32_t peer_host_capacity,
    uint16_t* peer_port) {
    struct sockaddr_in address;
    socklen_t address_length = (socklen_t)sizeof(address);
    int handle;
    if (!socket_is_open(server)) {
        return PIKA_SOCKET_CLOSED;
    }
    if (accepted == NULL ||
        accepted->handle != PIKA_SOCKET_INVALID_HANDLE ||
        peer_host == NULL ||
        peer_host_capacity < PIKA_SOCKET_ADDRESS_BYTE_LIMIT ||
        peer_port == NULL) {
        return PIKA_SOCKET_INVALID_ARGUMENT;
    }
    peer_host[0] = '\0';
    *peer_port = 0u;
    do {
        handle = accept(
            server->handle, (struct sockaddr*)&address,
            &address_length);
    } while (handle < 0 && errno == EINTR);
    if (handle < 0) {
        return socket_error_status(server, errno);
    }
    if (inet_ntop(
            AF_INET, &address.sin_addr, peer_host,
            (socklen_t)peer_host_capacity) == NULL) {
        close(handle);
        peer_host[0] = '\0';
        return PIKA_SOCKET_IO_ERROR;
    }
    accepted->handle = handle;
    accepted->timeout_milliseconds = -1;
    *peer_port = ntohs(address.sin_port);
    return PIKA_SOCKET_OK;
}

PikaSocketStatus pika_platform_socket_connect(
    PikaPlatformSocket* socket,
    const char* host,
    uint16_t port) {
    struct sockaddr_in address;
    PikaSocketStatus status;
    int result;
    if (!socket_is_open(socket)) {
        return PIKA_SOCKET_CLOSED;
    }
    status = resolve_address(host, port, 0, &address);
    if (status != PIKA_SOCKET_OK) {
        return status;
    }
    do {
        result = connect(
            socket->handle, (const struct sockaddr*)&address,
            (socklen_t)sizeof(address));
    } while (result != 0 && errno == EINTR);
    return result == 0
               ? PIKA_SOCKET_OK
               : socket_error_status(socket, errno);
}

PikaSocketStatus pika_platform_socket_send(
    PikaPlatformSocket* socket,
    const uint8_t* data,
    uint32_t length,
    uint32_t* sent) {
    ssize_t result;
    int flags = 0;
    if (!socket_is_open(socket)) {
        return PIKA_SOCKET_CLOSED;
    }
    if ((length != 0u && data == NULL) || sent == NULL ||
        length > (uint32_t)INT_MAX) {
        return PIKA_SOCKET_INVALID_ARGUMENT;
    }
    *sent = 0u;
    if (length == 0u) {
        return PIKA_SOCKET_OK;
    }
#ifdef MSG_NOSIGNAL
    flags = MSG_NOSIGNAL;
#endif
    do {
        result = send(socket->handle, data, (size_t)length, flags);
    } while (result < 0 && errno == EINTR);
    if (result < 0) {
        return socket_error_status(socket, errno);
    }
    *sent = (uint32_t)result;
    return PIKA_SOCKET_OK;
}

PikaSocketStatus pika_platform_socket_receive(
    PikaPlatformSocket* socket,
    uint8_t* data,
    uint32_t capacity,
    uint32_t* received) {
    ssize_t result;
    if (!socket_is_open(socket)) {
        return PIKA_SOCKET_CLOSED;
    }
    if ((capacity != 0u && data == NULL) || received == NULL ||
        capacity > (uint32_t)INT_MAX) {
        return PIKA_SOCKET_INVALID_ARGUMENT;
    }
    *received = 0u;
    if (capacity == 0u) {
        return PIKA_SOCKET_OK;
    }
    do {
        result = recv(socket->handle, data, (size_t)capacity, 0);
    } while (result < 0 && errno == EINTR);
    if (result < 0) {
        return socket_error_status(socket, errno);
    }
    *received = (uint32_t)result;
    return PIKA_SOCKET_OK;
}

PikaSocketStatus pika_platform_socket_set_timeout(
    PikaPlatformSocket* socket,
    int32_t timeout_milliseconds) {
    struct timeval timeout;
    int flags;
    if (!socket_is_open(socket)) {
        return PIKA_SOCKET_CLOSED;
    }
    if (timeout_milliseconds < -1) {
        return PIKA_SOCKET_INVALID_ARGUMENT;
    }
    flags = fcntl(socket->handle, F_GETFL, 0);
    if (flags < 0) {
        return PIKA_SOCKET_IO_ERROR;
    }
    if (timeout_milliseconds == 0) {
        flags |= O_NONBLOCK;
    } else {
        flags &= ~O_NONBLOCK;
    }
    if (fcntl(socket->handle, F_SETFL, flags) != 0) {
        return PIKA_SOCKET_IO_ERROR;
    }
    timeout.tv_sec =
        timeout_milliseconds > 0
            ? timeout_milliseconds / 1000
            : 0;
    timeout.tv_usec =
        timeout_milliseconds > 0
            ? (timeout_milliseconds % 1000) * 1000
            : 0;
    if (setsockopt(
            socket->handle, SOL_SOCKET, SO_RCVTIMEO,
            &timeout, (socklen_t)sizeof(timeout)) != 0 ||
        setsockopt(
            socket->handle, SOL_SOCKET, SO_SNDTIMEO,
            &timeout, (socklen_t)sizeof(timeout)) != 0) {
        return PIKA_SOCKET_IO_ERROR;
    }
    socket->timeout_milliseconds = timeout_milliseconds;
    return PIKA_SOCKET_OK;
}

PikaSocketStatus pika_platform_socket_resolve_ipv4(
    const char* host,
    char* address,
    uint32_t address_capacity) {
    struct sockaddr_in resolved;
    PikaSocketStatus status;
    if (address == NULL ||
        address_capacity < PIKA_SOCKET_ADDRESS_BYTE_LIMIT) {
        return PIKA_SOCKET_INVALID_ARGUMENT;
    }
    address[0] = '\0';
    status = resolve_address(host, 0u, 0, &resolved);
    if (status != PIKA_SOCKET_OK) {
        return status;
    }
    return inet_ntop(
               AF_INET, &resolved.sin_addr, address,
               (socklen_t)address_capacity) != NULL
               ? PIKA_SOCKET_OK
               : PIKA_SOCKET_IO_ERROR;
}

PikaSocketStatus pika_platform_socket_local_port(
    PikaPlatformSocket* socket,
    uint16_t* port) {
    struct sockaddr_in address;
    socklen_t length = (socklen_t)sizeof(address);
    if (!socket_is_open(socket)) {
        return PIKA_SOCKET_CLOSED;
    }
    if (port == NULL) {
        return PIKA_SOCKET_INVALID_ARGUMENT;
    }
    if (getsockname(
            socket->handle, (struct sockaddr*)&address,
            &length) != 0 ||
        length < (socklen_t)sizeof(address) ||
        address.sin_family != AF_INET) {
        return PIKA_SOCKET_IO_ERROR;
    }
    *port = ntohs(address.sin_port);
    return PIKA_SOCKET_OK;
}

#elif defined(_WIN32)

#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

static int winsock_initialized;

static PikaSocketStatus ensure_winsock(void) {
    WSADATA data;
    int status;
    if (winsock_initialized) {
        return PIKA_SOCKET_OK;
    }
    status = WSAStartup(MAKEWORD(2, 2), &data);
    if (status != 0) {
        return PIKA_SOCKET_IO_ERROR;
    }
    winsock_initialized = 1;
    return PIKA_SOCKET_OK;
}

static int socket_is_open(const PikaPlatformSocket* socket) {
    return socket != NULL &&
           socket->handle != PIKA_SOCKET_INVALID_HANDLE;
}

static PikaSocketStatus socket_error_status(
    const PikaPlatformSocket* socket,
    int error) {
    if (error == WSAEWOULDBLOCK || error == WSAEINPROGRESS ||
        error == WSAEALREADY) {
        return socket != NULL && socket->timeout_milliseconds > 0
                   ? PIKA_SOCKET_TIMEOUT
                   : PIKA_SOCKET_WOULD_BLOCK;
    }
    if (error == WSAETIMEDOUT) {
        return PIKA_SOCKET_TIMEOUT;
    }
    if (error == WSAECONNREFUSED) {
        return PIKA_SOCKET_CONNECTION_REFUSED;
    }
    if (error == WSAECONNRESET || error == WSAECONNABORTED ||
        error == WSAENOTCONN || error == WSAESHUTDOWN) {
        return PIKA_SOCKET_CONNECTION_RESET;
    }
    return PIKA_SOCKET_IO_ERROR;
}

static PikaSocketStatus resolve_address(
    const char* host,
    uint16_t port,
    int passive,
    struct sockaddr_in* address) {
    struct addrinfo hints;
    struct addrinfo* result = NULL;
    struct addrinfo* current;
    char service[6];
    int length;
    int status;
    size_t host_length;
    PikaSocketStatus startup = ensure_winsock();
    if (startup != PIKA_SOCKET_OK) {
        return startup;
    }
    if (host == NULL || address == NULL) {
        return PIKA_SOCKET_INVALID_ARGUMENT;
    }
    host_length = strlen(host);
    if (host_length > PIKA_SOCKET_HOST_BYTE_LIMIT) {
        return PIKA_SOCKET_INVALID_ARGUMENT;
    }
    length = snprintf(service, sizeof(service), "%u", (unsigned int)port);
    if (length <= 0 || (size_t)length >= sizeof(service)) {
        return PIKA_SOCKET_INVALID_ARGUMENT;
    }
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = passive ? AI_PASSIVE : 0;
    status = getaddrinfo(
        host_length == 0u ? NULL : host, service, &hints, &result);
    if (status != 0 || result == NULL) {
        if (result != NULL) {
            freeaddrinfo(result);
        }
        return PIKA_SOCKET_DNS_ERROR;
    }
    for (current = result; current != NULL; current = current->ai_next) {
        if (current->ai_family == AF_INET &&
            current->ai_addrlen >= (int)sizeof(struct sockaddr_in)) {
            memcpy(address, current->ai_addr, sizeof(*address));
            freeaddrinfo(result);
            return PIKA_SOCKET_OK;
        }
    }
    freeaddrinfo(result);
    return PIKA_SOCKET_DNS_ERROR;
}

void pika_platform_socket_initialize(PikaPlatformSocket* socket) {
    if (socket != NULL) {
        socket->handle = PIKA_SOCKET_INVALID_HANDLE;
        socket->timeout_milliseconds = -1;
    }
}

PikaSocketStatus pika_platform_socket_open(
    PikaPlatformSocket* endpoint,
    int family,
    int type,
    int protocol) {
    SOCKET handle;
    PikaSocketStatus startup = ensure_winsock();
    if (startup != PIKA_SOCKET_OK) {
        return startup;
    }
    if (endpoint == NULL ||
        endpoint->handle != PIKA_SOCKET_INVALID_HANDLE ||
        family != PIKA_SOCKET_AF_INET || type != PIKA_SOCKET_STREAM ||
        (protocol != 0 && protocol != IPPROTO_TCP)) {
        return PIKA_SOCKET_INVALID_ARGUMENT;
    }
    handle = socket(AF_INET, SOCK_STREAM, protocol);
    if (handle == INVALID_SOCKET) {
        return socket_error_status(endpoint, WSAGetLastError());
    }
    endpoint->handle = (intptr_t)handle;
    endpoint->timeout_milliseconds = -1;
    return PIKA_SOCKET_OK;
}

PikaSocketStatus pika_platform_socket_close(PikaPlatformSocket* socket) {
    int status;
    int error;
    if (socket == NULL) {
        return PIKA_SOCKET_INVALID_ARGUMENT;
    }
    if (socket->handle == PIKA_SOCKET_INVALID_HANDLE) {
        return PIKA_SOCKET_OK;
    }
    status = closesocket((SOCKET)socket->handle);
    error = WSAGetLastError();
    socket->handle = PIKA_SOCKET_INVALID_HANDLE;
    socket->timeout_milliseconds = -1;
    return status == 0 || error == WSAENOTSOCK
               ? PIKA_SOCKET_OK
               : PIKA_SOCKET_IO_ERROR;
}

PikaSocketStatus pika_platform_socket_bind(
    PikaPlatformSocket* socket,
    const char* host,
    uint16_t port) {
    struct sockaddr_in address;
    PikaSocketStatus status;
    int enabled = 1;
    if (!socket_is_open(socket)) {
        return PIKA_SOCKET_CLOSED;
    }
    status = resolve_address(host, port, 1, &address);
    if (status != PIKA_SOCKET_OK) {
        return status;
    }
    (void)setsockopt(
        (SOCKET)socket->handle, SOL_SOCKET, SO_REUSEADDR,
        (const char*)&enabled, (int)sizeof(enabled));
    return bind(
               (SOCKET)socket->handle,
               (const struct sockaddr*)&address,
               (int)sizeof(address)) == 0
               ? PIKA_SOCKET_OK
               : socket_error_status(socket, WSAGetLastError());
}

PikaSocketStatus pika_platform_socket_listen(
    PikaPlatformSocket* socket,
    int backlog) {
    if (!socket_is_open(socket)) {
        return PIKA_SOCKET_CLOSED;
    }
    if (backlog < 0) {
        return PIKA_SOCKET_INVALID_ARGUMENT;
    }
    return listen((SOCKET)socket->handle, backlog) == 0
               ? PIKA_SOCKET_OK
               : socket_error_status(socket, WSAGetLastError());
}

PikaSocketStatus pika_platform_socket_accept(
    PikaPlatformSocket* server,
    PikaPlatformSocket* accepted,
    char* peer_host,
    uint32_t peer_host_capacity,
    uint16_t* peer_port) {
    struct sockaddr_in address;
    int address_length = (int)sizeof(address);
    SOCKET handle;
    if (!socket_is_open(server)) {
        return PIKA_SOCKET_CLOSED;
    }
    if (accepted == NULL ||
        accepted->handle != PIKA_SOCKET_INVALID_HANDLE ||
        peer_host == NULL ||
        peer_host_capacity < PIKA_SOCKET_ADDRESS_BYTE_LIMIT ||
        peer_port == NULL) {
        return PIKA_SOCKET_INVALID_ARGUMENT;
    }
    peer_host[0] = '\0';
    *peer_port = 0u;
    handle = accept(
        (SOCKET)server->handle, (struct sockaddr*)&address,
        &address_length);
    if (handle == INVALID_SOCKET) {
        return socket_error_status(server, WSAGetLastError());
    }
    if (InetNtopA(
            AF_INET, &address.sin_addr, peer_host,
            peer_host_capacity) == NULL) {
        closesocket(handle);
        return PIKA_SOCKET_IO_ERROR;
    }
    accepted->handle = (intptr_t)handle;
    accepted->timeout_milliseconds = -1;
    *peer_port = ntohs(address.sin_port);
    return PIKA_SOCKET_OK;
}

PikaSocketStatus pika_platform_socket_connect(
    PikaPlatformSocket* socket,
    const char* host,
    uint16_t port) {
    struct sockaddr_in address;
    PikaSocketStatus status;
    if (!socket_is_open(socket)) {
        return PIKA_SOCKET_CLOSED;
    }
    status = resolve_address(host, port, 0, &address);
    if (status != PIKA_SOCKET_OK) {
        return status;
    }
    return connect(
               (SOCKET)socket->handle,
               (const struct sockaddr*)&address,
               (int)sizeof(address)) == 0
               ? PIKA_SOCKET_OK
               : socket_error_status(socket, WSAGetLastError());
}

PikaSocketStatus pika_platform_socket_send(
    PikaPlatformSocket* socket,
    const uint8_t* data,
    uint32_t length,
    uint32_t* sent) {
    int result;
    if (!socket_is_open(socket)) {
        return PIKA_SOCKET_CLOSED;
    }
    if ((length != 0u && data == NULL) || sent == NULL ||
        length > (uint32_t)INT_MAX) {
        return PIKA_SOCKET_INVALID_ARGUMENT;
    }
    *sent = 0u;
    if (length == 0u) {
        return PIKA_SOCKET_OK;
    }
    result = send(
        (SOCKET)socket->handle, (const char*)data, (int)length, 0);
    if (result == SOCKET_ERROR) {
        return socket_error_status(socket, WSAGetLastError());
    }
    *sent = (uint32_t)result;
    return PIKA_SOCKET_OK;
}

PikaSocketStatus pika_platform_socket_receive(
    PikaPlatformSocket* socket,
    uint8_t* data,
    uint32_t capacity,
    uint32_t* received) {
    int result;
    if (!socket_is_open(socket)) {
        return PIKA_SOCKET_CLOSED;
    }
    if ((capacity != 0u && data == NULL) || received == NULL ||
        capacity > (uint32_t)INT_MAX) {
        return PIKA_SOCKET_INVALID_ARGUMENT;
    }
    *received = 0u;
    if (capacity == 0u) {
        return PIKA_SOCKET_OK;
    }
    result = recv(
        (SOCKET)socket->handle, (char*)data, (int)capacity, 0);
    if (result == SOCKET_ERROR) {
        return socket_error_status(socket, WSAGetLastError());
    }
    *received = (uint32_t)result;
    return PIKA_SOCKET_OK;
}

PikaSocketStatus pika_platform_socket_set_timeout(
    PikaPlatformSocket* socket,
    int32_t timeout_milliseconds) {
    u_long mode;
    DWORD timeout;
    if (!socket_is_open(socket)) {
        return PIKA_SOCKET_CLOSED;
    }
    if (timeout_milliseconds < -1) {
        return PIKA_SOCKET_INVALID_ARGUMENT;
    }
    mode = timeout_milliseconds == 0 ? 1ul : 0ul;
    if (ioctlsocket((SOCKET)socket->handle, FIONBIO, &mode) != 0) {
        return PIKA_SOCKET_IO_ERROR;
    }
    timeout = timeout_milliseconds > 0
                  ? (DWORD)timeout_milliseconds
                  : 0u;
    if (setsockopt(
            (SOCKET)socket->handle, SOL_SOCKET, SO_RCVTIMEO,
            (const char*)&timeout, (int)sizeof(timeout)) != 0 ||
        setsockopt(
            (SOCKET)socket->handle, SOL_SOCKET, SO_SNDTIMEO,
            (const char*)&timeout, (int)sizeof(timeout)) != 0) {
        return PIKA_SOCKET_IO_ERROR;
    }
    socket->timeout_milliseconds = timeout_milliseconds;
    return PIKA_SOCKET_OK;
}

PikaSocketStatus pika_platform_socket_resolve_ipv4(
    const char* host,
    char* address,
    uint32_t address_capacity) {
    struct sockaddr_in resolved;
    PikaSocketStatus status;
    if (address == NULL ||
        address_capacity < PIKA_SOCKET_ADDRESS_BYTE_LIMIT) {
        return PIKA_SOCKET_INVALID_ARGUMENT;
    }
    address[0] = '\0';
    status = resolve_address(host, 0u, 0, &resolved);
    if (status != PIKA_SOCKET_OK) {
        return status;
    }
    return InetNtopA(
               AF_INET, &resolved.sin_addr, address,
               address_capacity) != NULL
               ? PIKA_SOCKET_OK
               : PIKA_SOCKET_IO_ERROR;
}

PikaSocketStatus pika_platform_socket_local_port(
    PikaPlatformSocket* socket,
    uint16_t* port) {
    struct sockaddr_in address;
    int length = (int)sizeof(address);
    if (!socket_is_open(socket)) {
        return PIKA_SOCKET_CLOSED;
    }
    if (port == NULL) {
        return PIKA_SOCKET_INVALID_ARGUMENT;
    }
    if (getsockname(
            (SOCKET)socket->handle,
            (struct sockaddr*)&address, &length) != 0 ||
        length < (int)sizeof(address) ||
        address.sin_family != AF_INET) {
        return PIKA_SOCKET_IO_ERROR;
    }
    *port = ntohs(address.sin_port);
    return PIKA_SOCKET_OK;
}

#else

#error "platform_socket.c requires a platform socket implementation"

#endif
