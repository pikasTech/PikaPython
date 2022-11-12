#include "PikaObj.h"
#ifdef __linux__
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#elif PIKA_LWIP_ENABLE
#include <lwip/sockets.h>
#include "lwip/api.h"
#include "lwip/netdb.h"
#include "lwip/opt.h"
#include "lwip/sys.h"
#else
/*
    You need to create the __platform_socket.h for your platform.
    For example:
    You can #include "lwip/socket.h" in the __platform_socket.h
*/
#include "__platform_socket.h"
#endif
int __platform_socket(int __domain, int __type, int __protocol);
int __platform_bind(int __fd,
                    const struct sockaddr* __addr,
                    socklen_t __addr_len);

int __platform_listen(int __fd, int __n);
int __platform_accept(int __fd, struct sockaddr* __addr, socklen_t* __addr_len);
int __platform_connect(int __fd,
                       const struct sockaddr* __addr,
                       socklen_t __addr_len);
int __platform_send(int __fd, const void* __buf, size_t __n, int __flags);
int __platform_recv(int __fd, void* __buf, size_t __n, int __flags);
int __platform_gethostname(char* __name, size_t __len);
int __platform_getaddrinfo(const char* __name,
                           const char* __service,
                           const struct addrinfo* __req,
                           struct addrinfo** __pai);
void __platform_freeaddrinfo(struct addrinfo* __ai);
int __platform_setsockopt(int __fd,
                          int __level,
                          int __optname,
                          const void* __optval,
                          socklen_t __optlen);

/* os file API */
int __platform_open(const char* __file, int __oflag, ...);
int __platform_close(int fd);
int __platform_read(int fd, void* buf, size_t count);
int __platform_write(int fd, const void* buf, size_t count);
int __platform_fcntl(int fd, int cmd, long arg);
