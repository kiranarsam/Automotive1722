/**
 * Copyright (c) 2025, Kiran Kumar Arsam
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *
 *   * Redistributions in binary form must reproduce the above copyright notice,
 *   * this list of conditions and the following disclaimer in the documentation
 *   * and/or other materials provided with the distribution.
 *
 *   * Neither the name of the Kiran Kumar Arsam nor the names of its
 *   * contributors may be used to endorse or promote products derived from
 *   * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <linux/if.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <stdio.h>
#include <unistd.h>

#include "lo_comm_if.h"

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