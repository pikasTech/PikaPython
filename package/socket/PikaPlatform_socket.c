#include "PikaPlatform_socket.h"
/*
    The functinos start with PIKA_WEAK are weak functions,
    you need to override them in your platform.
*/

PIKA_WEAK int pika_platform_socket(int __domain, int __type, int __protocol) {
#if defined(__linux__) || PIKA_LWIP_ENABLE
    return socket(__domain, __type, __protocol);
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
#endif
}

PIKA_WEAK int pika_platform_bind(int __fd,
                                 const struct sockaddr* __addr,
                                 socklen_t __addr_len) {
#if defined(__linux__) || PIKA_LWIP_ENABLE
    return bind(__fd, __addr, __addr_len);
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
#endif
}

PIKA_WEAK int pika_platform_listen(int __fd, int __n) {
#if defined(__linux__) || PIKA_LWIP_ENABLE
    return listen(__fd, __n);
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
#endif
}

PIKA_WEAK int pika_platform_accept(int __fd,
                                   struct sockaddr* __addr,
                                   socklen_t* __addr_len) {
#if defined(__linux__) || PIKA_LWIP_ENABLE
    return accept(__fd, __addr, __addr_len);
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
#endif
}

PIKA_WEAK int pika_platform_connect(int __fd,
                                    const struct sockaddr* __addr,
                                    socklen_t __addr_len) {
#if defined(__linux__) || PIKA_LWIP_ENABLE
    return connect(__fd, __addr, __addr_len);
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
#endif
}

PIKA_WEAK int pika_platform_htons(int __hostshort) {
#if defined(__linux__) || PIKA_LWIP_ENABLE
    return htons(__hostshort);
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
#endif
}

/* gethostbyname */
PIKA_WEAK struct hostent* pika_platform_gethostbyname(const char* __name) {
#if defined(__linux__) || PIKA_LWIP_ENABLE
    return gethostbyname(__name);
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
#endif
}

/* inet_ntoa */
PIKA_WEAK char* pika_platform_inet_ntoa(struct in_addr __in) {
#if defined(__linux__) || PIKA_LWIP_ENABLE
    return inet_ntoa(__in);
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
#endif
}

PIKA_WEAK int pika_platform_send(int __fd,
                                 const void* __buf,
                                 size_t __n,
                                 int __flags) {
#if defined(__linux__) || PIKA_LWIP_ENABLE
    return send(__fd, __buf, __n, __flags);
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
#endif
}

PIKA_WEAK int pika_platform_recv(int __fd,
                                 void* __buf,
                                 size_t __n,
                                 int __flags) {
#if defined(__linux__) || PIKA_LWIP_ENABLE
    return recv(__fd, __buf, __n, __flags);
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
    return -1;
#endif
}

/* gethostname */
PIKA_WEAK int pika_platform_gethostname(char* __name, size_t __len) {
#if defined(__linux__)
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
#if defined(__linux__) || PIKA_LWIP_ENABLE
    return getaddrinfo(__name, __service, __req, __pai);
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
#endif
}

PIKA_WEAK void pika_platform_freeaddrinfo(struct addrinfo* __ai) {
#if defined(__linux__) || PIKA_LWIP_ENABLE
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
#if defined(__linux__) || PIKA_LWIP_ENABLE
    return setsockopt(__fd, __level, __optname, __optval, __optlen);
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
#endif
}

PIKA_WEAK int pika_platform_fcntl(int fd, int cmd, long arg) {
#if defined(__linux__) || PIKA_LWIP_ENABLE
    return fcntl(fd, cmd, arg);
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
#endif
}

/* os file API */
PIKA_WEAK int pika_platform_close(int __fd) {
#if defined(__linux__) || PIKA_LWIP_ENABLE
    return close(__fd);
#elif PIKA_FREERTOS_ENABLE
    return closesocket(__fd);
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
#endif
}

PIKA_WEAK int pika_platform_write(int __fd, const void* __buf, size_t __nbyte) {
#if defined(__linux__) || PIKA_LWIP_ENABLE
    return write(__fd, __buf, __nbyte);
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
#endif
}
