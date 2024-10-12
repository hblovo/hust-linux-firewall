#include "firewall.h"
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/skbuff.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>
#include <linux/tcp.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("hblovo");
unsigned filter_hook_func(void *priv, struct sk_buff *skb, const struct nf_hook_state *state);
void process_message(unsigned char *message);
void show_connection(void);
long get_time(void);
void netlink_recv(struct sk_buff *skb);
//struct nf_hook_ops
//{
//    struct list_head list; //由结构nf_hook_ops构成一个链表，
//    list是此链表的表头，把各个hook函数组成一个表。初始值为{NULL, NULL}
//    nf_hookfn *hook;// 用户自定义的hook处理函数。
//    struct module *owner;//模块所有者
//    int pf;//协议族
//    int hooknum;//hook点，取值为5个HOOK点之一。
//    一个HOOK点可以挂多个HOOK函数，谁先被调用要看优先级。
//    int priority;//优先级。取值越小，优先级越高。
//};
static struct nf_hook_ops filter;
static struct sock *socket = NULL;
unsigned char action = NF_DROP;//grep -r "#define NF_DROP" /usr/include/linux/
int pid;//用户进程ID
int i;//for loop
Rule rule_set[50];
unsigned char cnt = 0;//rule_cnt
TCP_Connection TCP_connection[256];
UDP_Connection UDP_connection[256];
ICMP_Connection ICMP_connection[256];
//struct netlink_kernel_cfg {
//    unsigned int groups;
//    unsigned int flags;
//    void (*input)(struct sk_buff *skb);
//    struct mutex *cb_mutex;
//    int (*bind)(struct net *net, int group);
//    void (*unbind)(struct net *net, int group);
//    bool (*compare)(struct net *net, struct sock *sk);
//};
struct netlink_kernel_cfg netlink_cfg = {
        .groups = 0,
        .flags = 0,
        .input = netlink_recv,//用于指定一个回调函数，当有消息通过 Netlink 套接字传递给内核时，这个回调函数就会被调用。它负责处理从用户空间发送过来的数据。
        .cb_mutex = NULL,
        .bind = NULL,
        .unbind = NULL,
        .compare = NULL,
};
unsigned int telnet_filter(void *priv,
                           struct sk_buff *skb,
                           const struct nf_hook_state *state)
{
    struct iphdr *iph;
    struct tcphdr *tcph;

    iph = ip_hdr(skb);
    tcph = (void *)iph + iph->ihl*4;

    if(iph->protocol == IPPROTO_TCP && tcph->dest == htons(23))
    {
        printk(KERN_INFO "Dropping telnet packet to %d.%d.%d.%d\n",
                ((unsigned char *)&iph->daddr)[0],
                ((unsigned char *)&iph->daddr)[1],
                ((unsigned char *)&iph->daddr)[2],
                ((unsigned char *)&iph->daddr)[3]);
        return NF_DROP;
    }
    else{
        return NF_ACCEPT;
    }
}

static int firewall_init(void)
{
    FIREWALL_LOG("Module loaded.\n");
    filter.hook = (nf_hookfn *)filter_hook_func;
    filter.pf = PF_INET;
    filter.hooknum = NF_INET_FORWARD;//进行包过滤
    filter.priority = NF_IP_PRI_FIRST; //new version, maybe changed to NF_INET_PRI_FIRST
    nf_register_hook(&filter);
    socket = netlink_kernel_create(&init_net,NETLINK,&netlink_cfg);
    //套接字创建失败时的清理操作
    if(!socket){
        sock_release(socket->sk_socket);
        return -1;
    }
    return 0;
}

static void firewall_exit(void)
{
    nf_unregister_hook(&filter);
    FIREWALL_LOG("Netfilter hook unregistered.\n");
    if (socket) {
        sock_release(socket->sk_socket);
        FIREWALL_LOG("Socket released.\n");
    }
    FIREWALL_LOG("Firewall module exit completed.\n");
}

module_init(firewall_init);
module_exit(firewall_exit);

//hook函数的实现
unsigned filter_hook_func(void *priv, struct sk_buff *skb, const struct nf_hook_state *state){
    struct iphdr *iph;
    struct tcphdr *tcph;
    struct udphdr *udph;
    Log log;
    // 检查包是否为空
    if (!skb)
        return action;

    // 获取 IP 头
    iph = ip_hdr(skb);
    if (!iph)
        return action;

    // 检查协议类型
    switch (iph->protocol) {
        case TCP:{// TCP 协议

        }
        case UDP: {// UDP 协议

        }

        case ICMP:{// ICMP协议

        }
    }

    // 如果没有命中任何过滤规则，默认拒绝
    printk("[DEFAULT]%s\n", action == NF_DROP ? "DROP" : "PASS");
    return action;
}
void netlink_recv(struct sk_buff *skb)
{
    struct nlmsghdr *nlh;//netlink消息头结构指针
    nlh = nlmsg_hdr(skb);//提取netlink消息
    //消息长度
    if (skb->len < nlh->nlmsg_len){
        return;
    }
    //用户进程id
    pid = nlh->nlmsg_pid;
    //提取消息数据部分
    unsigned char message[MAX_MSG_LEN];
    memcpy(message,(char *) NLMSG_DATA(nlh),MAX_MSG_LEN);
    process_message(message);
}
static void netlink_send(unsigned char *message, int len)
{
    struct sk_buff *skb;
    struct nlmsghdr *nlh;

    skb = alloc_skb(NLMSG_SPACE(len), GFP_KERNEL);
    nlh = nlmsg_put(skb, 0, 0, 0, len, 0);
    NETLINK_CB(skb).portid = 0;
    NETLINK_CB(skb).dst_group = 0;
    memcpy(NLMSG_DATA(nlh), message, len);
    netlink_unicast(socket, skb, pid, MSG_DONTWAIT);
}
void process_message(unsigned char *message){
//    FIREWALL_LOG("Received message (hex):");
//    int i;
//    // 打印收到的消息内容，逐字节打印十六进制值
//    for (i = 0; i < sizeof(Rule) + 1; ++i) {  // sizeof(Rule) + 1 包含首字节的操作类型
//        FIREWALL_LOG("%02X ", message[i]);  // 按十六进制输出每个字节
//    }
    Rule  rule;
    unsigned char rm_num;
    unsigned char op = *message;
    switch (op) {
        case 0:{//添加规则
            FIREWALL_LOG("add-rule");
            memcpy(&rule,&message[1],sizeof(Rule));
            rule_set[cnt++] = rule;
            break;
        }
        case 1:{//修改规则
            FIREWALL_LOG("modify-rule");
            memcpy(&rule,&message[1],sizeof(Rule));
            rule_set[rule.number-1] = rule;
            break;
        }
        case 2:{//删除规则
            FIREWALL_LOG("delete-rule");
            rm_num = message[1];
            //删除下标为 rm_num-1 的规则
            memcpy(rule_set + rm_num - 1,rule_set + rm_num,(cnt-- - rm_num) * sizeof(Rule));
            for(i = rm_num-1;i < cnt;i++){
                rule_set[i].number = i + 1;
            }
            break;
        }
        case 3:{
            show_connection();
            break;
        }
        case 4:{

        }
        case '5':{

        }
        case 6:{
            action = message[1];
            char act[50];  // 用于存储格式化后的字符串
            snprintf(act, sizeof(act), "Set action: %s", action == 1 ? "Accept" : "Drop");
            FIREWALL_LOG("%s\n", act);
        }
        case '7':{

        }
        default:{
            FIREWALL_LOG("hello world");
        }
    }
}
void show_connection(void)
{
    Rule connection;
    for(i = 0;i<256;i++)
    {
        if(TCP_connection[i].flags)
        {
            connection.protocol = TCP;
            connection.src_ip = TCP_connection[i].src_IP;
            connection.dst_ip = TCP_connection[i].dst_IP;
            connection.src_port = TCP_connection[i].src_port;
            connection.dst_port = TCP_connection[i].dst_port;
            netlink_send((unsigned char*)&connection,sizeof(Rule));
        }
        if(jiffies < UDP_connection[i].time)
        {
            connection.protocol = UDP;
            connection.src_ip = UDP_connection[i].src_IP;
            connection.dst_ip = UDP_connection[i].dst_IP;
            connection.src_port = UDP_connection[i].src_port;
            connection.dst_port = UDP_connection[i].dst_port;
            netlink_send((unsigned char*)&connection,sizeof(Rule));
        }
        if(jiffies < ICMP_connection[i].time)
        {
            connection.protocol = ICMP;
            connection.src_ip = ICMP_connection[i].src_IP;
            connection.dst_ip = ICMP_connection[i].dst_IP;
            connection.src_port = 0;
            connection.dst_port = 0;
            netlink_send((unsigned char*)&connection,sizeof(Rule));
        }
    }
    //查询完成
    connection.action = 'f';
    netlink_send((unsigned char*)&connection,sizeof(Rule));
}
