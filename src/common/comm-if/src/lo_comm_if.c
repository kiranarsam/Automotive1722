
#include <linux/if.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <stdio.h>
#include <unistd.h>

#include "lo_comm_if.h"

int Comm_Lo_CreateListenerSocket(const char *ifname, int protocol, struct sockaddr_ll *sk_addr)
{
    int fd, res;
    //struct sockaddr_ll sk_addr;
    struct ifreq req;

    fd = socket(AF_PACKET, SOCK_DGRAM, htons(protocol));
    if (fd < 0) {
        perror("Failed to open socket");
        return -1;
    }

    snprintf(req.ifr_name, sizeof(req.ifr_name), "%s", ifname);
    res = ioctl(fd, SIOCGIFINDEX, &req);
    if (res < 0) {
        perror("Failed to get interface index");
        return -1;
    }

    sk_addr->sll_family = AF_PACKET;
    sk_addr->sll_protocol = htons(protocol);
    sk_addr->sll_halen = ETH_ALEN;
    sk_addr->sll_ifindex = req.ifr_ifindex;

    res = bind(fd, (struct sockaddr *) &sk_addr, sizeof(sk_addr));
    if (res < 0) {
        perror("Couldn't bind() to interface");
        goto err;
    }

    return fd;

err:
    close(fd);
    return -1;
}

int Comm_Lo_CreateSocket(const char *ifname, int protocol,
                               struct sockaddr_ll *sk_addr)
{
    int fd, res;
    struct ifreq req;

    fd = socket(AF_PACKET, SOCK_DGRAM, htons(protocol));
    if (fd < 0) {
        perror("Failed to open socket");
        return -1;
    }

    snprintf(req.ifr_name, sizeof(req.ifr_name), "%s", ifname);
    res = ioctl(fd, SIOCGIFINDEX, &req);
    if (res < 0) {
        perror("Failed to get interface index");
        goto err;
    }

    sk_addr->sll_family = AF_PACKET;
    sk_addr->sll_protocol = htons(protocol);
    sk_addr->sll_halen = ETH_ALEN;
    sk_addr->sll_ifindex = req.ifr_ifindex;

    res = bind(fd, (struct sockaddr *) sk_addr, sizeof(struct sockaddr_ll));
    if (res < 0) {
        perror("Couldn't bind() to interface");
        goto err;
    }

    return fd;
err:
    close(fd);
    return -1;
}