//
// Created by hblovo on 2024/9/9.
//
#include "netlink.h"
#include <cstdio>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <linux/ip.h>
#include <list>
#include <map>
#include <unordered_map>
#include <string>
#include <utility>

using namespace std;
unordered_map<string,int> protocol_map;
Rule rules[RULE_SIZE];
int rule_cnt;//规则计数
void list_rules();
void init_map();
string get_IP(unsigned IP);
int input_rule(stringstream &ss, Rule *rule);
int main()
{
    //安装模块，并使用脚本生成日志文件
    system("sudo insmod firewall.ko");
    system("sudo sysctl -w net.ipv4.ip_forward=1");
    system("python log.py");

    //创建套接字并绑定
    int socket_fd = socket(AF_NETLINK, SOCK_RAW, NETLINK);
    int ret = bind_socket(socket_fd);

    init_map();

    bool flag = true;
    while(flag)
    {
        cout << "——————————————————————————————————————————————————————————————————————\n"
             << "请按照规则输入指令进行相应的防火墙控制，使用方式如下：\n"
             << "1. 添加防火墙规则   ||add-rule ||protocol ||src_IP   ||src_port ||dst_IP   ||dst_port ||action\n"
             << "2. 修改防火墙规则   ||modify-rule ||number   ||protocol ||src_IP   ||src_port ||dst_IP   ||dst_port ||action\n"
             << "3. 删除防火墙规则   ||delete-rule ||number\n"
             << "4. 展示防火墙规则   ||ls-rule\n"
             << "5. 展示已有连接     ||ls-connection\n"
             << "展示日志            ||show-log\n"
             << "退出程序            q\n"
             << "——————————————————————————————————————————————————————————————————————\n"
             << "请输入指令：\n";

        string cmd;
        getline(cin,cmd);
        stringstream ss(cmd);
        ss >> cmd;

        if (cmd == "add-rule")//0--添加规则
        {
            // 输入需要添加的规则
            Rule rule;
            input_rule(ss,&rule);
            rule.number = rule_cnt + 1;
            rules[rule_cnt++] = rule;

            // 组装消息并发送
            unsigned char message[MAX_MSG_LEN];
            memset(message, 0, sizeof(message));  // 清空 message 缓冲区

            message[0] = 0; // 设置第一个字节为 0，表示 add-rule 操作
            memcpy(&message[1], (unsigned char *)&rule, sizeof(Rule)); // 从 message[1] 开始存储 rule
            // 发送消息
            netlink_send(socket_fd, message, sizeof(Rule) + 1);
        }
        //修改防火墙规则
        if(cmd == "modify-rule")//1--修改规则
        {
            string index_str;
            ss >> index_str;
            int index = stoi(index_str);
            if(index > rule_cnt){
                cout << "[ERROR] 规则不存在\n";
                continue;
            }
            Rule rule;
            input_rule(ss,&rule);
            rule.number = index;
            rules[rule.number-1] = rule;

            // 组装消息并发送
            unsigned char message[MAX_MSG_LEN];
            memset(message, 0, sizeof(message));  // 清空 message 缓冲区

            message[0] = 1; // 设置第一个字节为 1，表示 modify-rule 操作
            memcpy(&message[1], (unsigned char *)&rule, sizeof(Rule)); // 从 message[1] 开始存储 rule
            // 发送消息
            netlink_send(socket_fd, message, sizeof(Rule) + 1);

        }
        if(cmd == "delete-rule")
        {
            string index_str;
            ss >> index_str;
            int index = stoi(index_str);
            if(index > rule_cnt){
                cout << "[ERROR] 规则不存在\n";
                continue;
            }
            // 前移其他规则
            memcpy(rules + index - 1, rules + index, (rule_cnt-- - index) * sizeof(Rule));
            for (int i = rm_num - 1; i < rule_cnt; ++i)
            {
                rules[i].number = i + 1;
            }

            unsigned char message[3];
            memset(message, 0, sizeof(message));  // 清空 message 缓冲区

            message[0] = 2; // 设置第一个字节为 2，表示 delete-rule 操作
            message[1] = index;
            // 发送消息
            netlink_send(socket_fd, message, 2);
        }
        //查询现有规则
        if(cmd == "ls-rule"){
            list_rules();
        }
        
        if(cmd == "show-log"){
            system("dmesg | grep \"firewall\"");
        }
        if(cmd == "q"){
            flag = false;
        }
    }
    close(socket_fd);
    system("sudo dmesg -C");
    system("sudo rmmod firewall");
}
void init_map(){
    protocol_map["TCP"] = TCP;
    protocol_map["UDP"] = UDP;
    protocol_map["ICMP"] = ICMP;
}
void list_rules() {
    if (rule_cnt == 0) {
        cout << "[无规则]\n";
        return;
    }

    cout << "————————————————————————————————————————————————————————————————————————\n";
    cout << "|编号| 协议  |       源IP       | 源端口 |      目的IP      | 目的端口 | 策略 |\n";
    cout << "————————————————————————————————————————————————————————————————————————\n";

    for (int i = 0; i < rule_cnt; i++) {
        string protocol_str;
        switch (rules[i].protocol) {
            case 6: protocol_str = "TCP"; break;
            case 17: protocol_str = "UDP"; break;
            case 1: protocol_str = "ICMP"; break;
            default: protocol_str = "OTHER"; break;
        }

        string action_str = (rules[i].action == 'Y') ? "放行" : "拒绝";

        // 转换 IP 地址
        string src_ip = get_IP(rules[i].src_ip);
        string dst_ip = get_IP(rules[i].dst_ip);

        // 输出每条规则
        cout << "|" << setw(3) << (int)rules[i].number << " | "
             << setw(5) << protocol_str << " | "
             << setw(15) << src_ip << " | "
             << setw(7) << (rules[i].src_port == 0 ? "ANY" : to_string(ntohs(rules[i].src_port))) << " | "
             << setw(15) << dst_ip << " | "
             << setw(7) << (rules[i].dst_port == 0 ? "ANY" : to_string(ntohs(rules[i].dst_port))) << " | "
             << setw(4) << action_str << " |\n";
    }

    cout << "————————————————————————————————————————————————————————————————————————\n";
}
int input_rule(stringstream &ss, Rule *rule) {

    string protocol;
    char action;
    ss >> protocol;
    rule->protocol = protocol_map.at(protocol); // 使用 at() 确保访问有效

    if (protocol == "ICMP") {
        std::string src_ip, dst_ip;
        ss >> src_ip >> dst_ip;
        rule->src_ip = (src_ip == "ANY") ? ANY : inet_addr(src_ip.c_str());
        rule->dst_ip = (dst_ip == "ANY") ? ANY : inet_addr(dst_ip.c_str());
        rule->src_port = 0;
        rule->dst_port = 0;
    } else if (protocol == "TCP" || protocol == "UDP") {
        std::string src_ip, dst_ip, src_port, dst_port;
        ss >> src_ip >> src_port >> dst_ip >> dst_port;
        rule->src_ip = (src_ip == "ANY") ? ANY : inet_addr(src_ip.c_str());
        rule->dst_ip = (dst_ip == "ANY") ? ANY : inet_addr(dst_ip.c_str());
        rule->src_port = (src_port == "ANY") ? ANY : htons(stoi(src_port));
        rule->dst_port = (dst_port == "ANY") ? ANY : htons(stoi(dst_port));
    }

    ss >> action;
    rule->action = action;
    return 0;
}
string get_IP(unsigned IP)
{
    if (IP == 0) return "ANY";
    else
    {
        struct in_addr addr;
        memcpy(&addr, &IP, 4);
        return inet_ntoa(addr);
    }
}
