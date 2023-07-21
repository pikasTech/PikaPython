#include "PikaObj.h"
#ifdef __linux__
#include <arpa/inet.h>
#include <errno.h>
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
    You need to create the pika_platform_socket.h for your platform.
    For example:
    You can #include "lwip/socket.h" in the pika_platform_socket.h
*/
#include "pika_platform_socket.h"
#endif
int pika_platform_socket(int __domain, int __type, int __protocol);
int pika_platform_bind(int __fd,
                       const struct sockaddr* __addr,
                       socklen_t __addr_len);

int pika_platform_listen(int __fd, int __n);
int pika_platform_accept(int __fd,
                         struct sockaddr* __addr,
                         socklen_t* __addr_len);
int pika_platform_connect(int __fd,
                          const struct sockaddr* __addr,
                          socklen_t __addr_len);
int pika_platform_send(int __fd, const void* __buf, size_t __n, int __flags);
int pika_platform_recv(int __fd, void* __buf, size_t __n, int __flags);
int pika_platform_gethostname(char* __name, size_t __len);
int pika_platform_getaddrinfo(const char* __name,
                              const char* __service,
                              const struct addrinfo* __req,
                              struct addrinfo** __pai);
void pika_platform_freeaddrinfo(struct addrinfo* __ai);
int pika_platform_setsockopt(int __fd,
                             int __level,
                             int __optname,
                             const void* __optval,
                             socklen_t __optlen);
int pika_platform_htons(int __hostshort);
char* pika_platform_inet_ntoa(struct in_addr __in);
struct hostent* pika_platform_gethostbyname(const char* __name);

/* os file API */
int pika_platform_close(int fd);
int pika_platform_write(int fd, const void* buf, size_t count);
int pika_platform_fcntl(int fd, int cmd, long arg);
