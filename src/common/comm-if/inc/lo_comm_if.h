#ifndef LO_COMM_IF_COMMON_H__
#define LO_COMM_IF_COMMON_H__

#include <stdint.h>
#include <netinet/in.h>

int Comm_Lo_CreateSocket(const char *ifname, int protocol,
                        struct sockaddr_ll *sk_addr);
#endif /* ETHER_COMM_IF_COMMON_H__ */
