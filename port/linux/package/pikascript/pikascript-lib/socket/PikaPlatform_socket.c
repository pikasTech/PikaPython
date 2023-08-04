#include "PikaPlatform_socket.h"
/*
    The functinos start with PIKA_WEAK are weak functions,
    you need to override them in your platform.
*/

#ifdef _WIN32
#pragma comment(lib, "ws2_32.lib")

static int pika_platform_winsock_initialized = 0;

int pika_platform_init_winsock() {
    if (0 == pika_platform_winsock_initialized) {
        WSADATA wsaData;
        int res = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (res != 0) {
            __platform_printf("WSAStartup failed with error: %d\n", res);
            return 1;
        }
        pika_platform_winsock_initialized = 1;
    } else if (0 < pika_platform_winsock_initialized) {
        pika_platform_winsock_initialized++;
    }
    return 0;
}

int pika_platform_cleanup_winsock() {
    if (1 == pika_platform_winsock_initialized) {
        WSACleanup();
        pika_platform_winsock_initialized = 0;
    } else if (1 < pika_platform_winsock_initialized) {
        pika_platform_winsock_initialized--;
    }
    return 0;
}
#endif

PIKA_WEAK int pika_platform_socket(int __domain, int __type, int __protocol) {
#if defined(__linux__) || defined(_WIN32) || PIKA_LWIP_ENABLE
    return socket(__domain, __type, __protocol);
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
#endif
}

PIKA_WEAK int pika_platform_bind(int __fd,
                                 const struct sockaddr* __addr,
                                 socklen_t __addr_len) {
#if defined(__linux__) || defined(_WIN32) || PIKA_LWIP_ENABLE
    return bind(__fd, __addr, __addr_len);
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
#endif
}

PIKA_WEAK int pika_platform_listen(int __fd, int __n) {
#if defined(__linux__) || defined(_WIN32) || PIKA_LWIP_ENABLE
    return listen(__fd, __n);
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
#endif
}

PIKA_WEAK int pika_platform_accept(int __fd,
                                   struct sockaddr* __addr,
                                   socklen_t* __addr_len) {
#if defined(__linux__) || defined(_WIN32) || PIKA_LWIP_ENABLE
    return accept(__fd, __addr, __addr_len);
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
#endif
}

PIKA_WEAK int pika_platform_connect(int __fd,
                                    const struct sockaddr* __addr,
                                    socklen_t __addr_len) {
#if defined(__linux__) || defined(_WIN32) || PIKA_LWIP_ENABLE
    return connect(__fd, __addr, __addr_len);
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
#endif
}

PIKA_WEAK int pika_platform_htons(int __hostshort) {
#if defined(__linux__) || defined(_WIN32) || PIKA_LWIP_ENABLE
    return htons(__hostshort);
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
#endif
}

/* gethostbyname */
PIKA_WEAK struct hostent* pika_platform_gethostbyname(const char* __name) {
#if defined(__linux__) || defined(_WIN32) || PIKA_LWIP_ENABLE
    return gethostbyname(__name);
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
#endif
}

/* inet_ntoa */
PIKA_WEAK char* pika_platform_inet_ntoa(struct in_addr __addr) {
#if defined(__linux__) || defined(_WIN32) || PIKA_LWIP_ENABLE
    return inet_ntoa(__addr);
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
#endif
}

PIKA_WEAK int pika_platform_send(int __fd,
                                 const void* __buf,
                                 size_t __n,
                                 int __flags) {
#if defined(__linux__) || defined(_WIN32) || PIKA_LWIP_ENABLE
    return send(__fd, __buf, __n, __flags);
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
#endif
}

PIKA_WEAK int pika_platform_recv(int __fd,
                                 void* __buf,
                                 size_t __n,
                                 int __flags) {
#if defined(__linux__) || defined(_WIN32) || PIKA_LWIP_ENABLE
    return recv(__fd, __buf, __n, __flags);
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
    return -1;
#endif
}

/* gethostname */
PIKA_WEAK int pika_platform_gethostname(char* __name, size_t __len) {
#if defined(__linux__) || defined(_WIN32)
    return gethostname(__name, __len);
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
    return -1;
#endif
}

PIKA_WEAK int pika_platform_getaddrinfo(const char* __name,
                                        const char* __service,
                                        const struct addrinfo* __req,
                                        struct addrinfo** __pai) {
#if defined(__linux__) || defined(_WIN32) || PIKA_LWIP_ENABLE
    return getaddrinfo(__name, __service, __req, __pai);
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
#endif
}

PIKA_WEAK void pika_platform_freeaddrinfo(struct addrinfo* __ai) {
#if defined(__linux__) || defined(_WIN32) || PIKA_LWIP_ENABLE
    freeaddrinfo(__ai);
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
#endif
}

PIKA_WEAK int pika_platform_setsockopt(int __fd,
                                       int __level,
                                       int __optname,
                                       const void* __optval,
                                       socklen_t __optlen) {
#if defined(__linux__) || defined(_WIN32) || PIKA_LWIP_ENABLE
    return setsockopt(__fd, __level, __optname, __optval, __optlen);
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
#endif
}

PIKA_WEAK int pika_platform_fcntl(int fd, int cmd, long arg) {
#if defined(__linux__) || PIKA_LWIP_ENABLE
    return fcntl(fd, cmd, arg);
#elif defined(_WIN32)
    if (cmd == F_GETFL) {
        u_long mode = 0;
        ioctlsocket(fd, FIONBIO, &mode);
        return (mode ? O_NONBLOCK : 0);
    } else if (cmd == F_SETFL) {
        u_long mode = (arg & O_NONBLOCK) ? 1 : 0;
        return ioctlsocket(fd, FIONBIO, &mode);
    }
    return -1;
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
#endif
}

/* os file API */
PIKA_WEAK int pika_platform_close(int __fd) {
#if defined(__linux__) || PIKA_LWIP_ENABLE
    return close(__fd);
#elif defined(_WIN32)
    return closesocket(__fd);
#elif PIKA_FREERTOS_ENABLE
    return closesocket(__fd);
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
#endif
}

PIKA_WEAK int pika_platform_write(int __fd, const void* __buf, size_t __nbyte) {
#if defined(__linux__) || PIKA_LWIP_ENABLE
    return write(__fd, __buf, __nbyte);
#elif defined(_WIN32)
    return send(__fd, __buf, __nbyte, 0);
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
#endif
}
