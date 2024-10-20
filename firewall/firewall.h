//
// Created by hblovo on 2024/9/9.
//
#define ANY 0
#define ICMP 1
#define TCP 6
#define UDP 17
#define NETLINK 30
#define MAX_MSG_LEN 30
#define RULE_SIZE 50
#define FIREWALL_LOG(fmt, ...) printk(KERN_INFO "[firewall] " fmt, ##__VA_ARGS__)
typedef struct {
    unsigned src_ip;    // 源IP地址
    unsigned dst_ip;    // 目的IP地址
    int src_port;       // 源端口
    int dst_port;       // 目的端口
    int protocol;       // 协议
    int action;         // 动作：拒绝/放行
} Log;

typedef struct {
    int number;             // 规则的编号
    char action;             // 动作：拒绝/放行
    unsigned src_ip;         // 源IP地址
    unsigned dst_ip;         // 目的IP地址
    int src_port;            // 源端口
    int dst_port;            // 目的端口
    int protocol;            // 协议
} Rule;
typedef struct
{
    unsigned char flags;
    unsigned short src_port;
    unsigned short dst_port;
    unsigned src_IP;
    unsigned dst_IP;
    unsigned first_FIN;
} TCP_Connection;

typedef struct
{
    unsigned short src_port;
    unsigned short dst_port;
    unsigned src_IP;
    unsigned dst_IP;
    unsigned long time;
} UDP_Connection;

typedef struct
{
    unsigned src_IP;
    unsigned dst_IP;
    unsigned long time;
} ICMP_Connection;
unsigned char my_hash_32(unsigned key)
{
    int i, j;
    unsigned mask;
    unsigned char part, value = 0;
    for (i = 0; i < 4; ++i)
    {
        part = 0;
        for (mask = 1 << i, j = 0; j < 8; mask <<= 4, ++j)
        {
            part = (part << 1) ^ ((key & mask) >> ((j << 2) + i));
        }
        value ^= part;
    }
    return value;
}

unsigned char my_hash_16(unsigned short key)
{
    int i, j;
    unsigned short mask;
    unsigned char part, value = 0;
    for (i = 0; i < 2; ++i)
    {
        part = 0;
        for (mask = 1 << i, j = 0; j < 8; mask <<= 2, ++j)
        {
            part = (part << 1) ^ ((key & mask) >> ((j << 1) + i));
        }
        value ^= part;
    }
    return value;
}