#include "PikaObj.h"
#ifdef __linux__
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
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
int __platform_close(int __fd);
int __platform_gethostname(char* __name, size_t __len);
