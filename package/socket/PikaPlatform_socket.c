#include "PikaPlatform_socket.h"
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

/* gethostname */
PIKA_WEAK int __platform_gethostname(char* __name, size_t __len) {
#ifdef __linux__
    return gethostname(__name, __len);
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
#endif
}

PIKA_WEAK int __platform_getaddrinfo(const char* __name,
                                     const char* __service,
                                     const struct addrinfo* __req,
                                     struct addrinfo** __pai) {
#ifdef __linux__
    return getaddrinfo(__name, __service, __req, __pai);
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
#endif
}

PIKA_WEAK void __platform_freeaddrinfo(struct addrinfo* __ai) {
#ifdef __linux__
    freeaddrinfo(__ai);
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
#endif
}

PIKA_WEAK int __platform_setsockopt(int __fd,
                                    int __level,
                                    int __optname,
                                    const void* __optval,
                                    socklen_t __optlen) {
#ifdef __linux__
    return setsockopt(__fd, __level, __optname, __optval, __optlen);
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
#endif
}

PIKA_WEAK int __platform_fcntl(int fd, int cmd, long arg) {
#ifdef __linux__
    return fcntl(fd, cmd, arg);
#else
    WEAK_FUNCTION_NEED_OVERRIDE_ERROR();
#endif
}
