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

static int myfw_init(void)
{
    printk("my firewall module loaded.\n");

    filter.hook = telnet_filter;
    filter.pf = PF_INET;
    nfho.hooknum = NF_INET_FORWARD;//进行包过滤
    nfho.priority = NF_IP_PRI_FIRST; //new version, maybe changed to NF_INET_PRI_FIRST
    nf_register_hook(&filter);
    return 0;
}

static void myfw_exit(void)
{
    printk("my firewall module exit ...\n");
    nf_unregister_hook(&nfho);
}

module_init(myfw_init);
module_exit(myfw_exit);
