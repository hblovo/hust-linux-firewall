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

typedef struct
{
    struct nlmsghdr hdr;//消息头
    char msg[MAX_MSG_LEN];//消息内容
} Message;

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

int netlink_send(int socket_fd, unsigned char *message, unsigned messageLen)
{
    // 创建消息头
    struct nlmsghdr *nlh;
    nlh = (struct nlmsghdr *)malloc(sizeof(Message));
    if (nlh == NULL)
    {
        // malloc失败
        return -1;
    }

    memset(nlh, 0, sizeof(struct nlmsghdr)); // 填充消息
    nlh->nlmsg_len = NLMSG_SPACE(messageLen + 1);
    nlh->nlmsg_flags = 0;
    nlh->nlmsg_type = 0;
    nlh->nlmsg_seq = 0;
    nlh->nlmsg_pid = getpid();
    memcpy(NLMSG_DATA(nlh), message, messageLen);

    // 设置内核地址
    struct sockaddr_nl kernel;
    memset(&kernel, 0, sizeof(kernel));
    kernel.nl_family = AF_NETLINK;
    kernel.nl_pid = 0; // 发往内核
    kernel.nl_groups = 0;

    int ret;
    ret = sendto(socket_fd, nlh, nlh->nlmsg_len, 0, (struct sockaddr *)&kernel, sizeof(kernel));
    if (!ret) // send失败
    {
        return -2;
    }
    free(nlh);
    return 0;
}