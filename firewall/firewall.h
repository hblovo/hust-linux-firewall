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
    unsigned char number;    // 规则的编号
    unsigned src_ip;         // 源IP地址
    unsigned dst_ip;         // 目的IP地址
    int src_port;            // 源端口
    int dst_port;            // 目的端口
    int protocol;            // 协议
    int action;              // 动作：拒绝/放行
} Rule;
