//
// Created by hblovo on 2024/9/9.
//

#include <linux/netlink.h>
#include <linux/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "firewall.h"

int bind_socket(int socket_fd)
{
    struct sockaddr_nl nlsrc_addr = {0};
    /* 设置本地socket地址 */
    nlsrc_addr.nl_family = AF_NETLINK;
    nlsrc_addr.nl_pid = getpid();
    nlsrc_addr.nl_groups = 0;

    /* 绑定套接字 */
    return bind(socket_fd, (struct sockaddr*)&nlsrc_addr,sizeof(nlsrc_addr));

}