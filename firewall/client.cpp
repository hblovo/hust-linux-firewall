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
void init_map(){
    protocol_map["TCP"] = TCP;
    protocol_map["UDP"] = UDP;
    protocol_map["ICMP"] = ICMP;
}
int main()
{
    //安装模块，并使用脚本生成日志文件
    system("sudo insmod firewall.ko");
    system("sudo sysctl -w net.ipv4.ip_forward=1");
    system("python log.py");

    //创建套接字并绑定
    int socket_fd = socket(AF_NETLINK, SOCK_RAW, NETLINK);
    bind_socket(socket_fd);

    Rule rule;Rule rules[RULE_SIZE];
    int rule_cnt = 0;//规则计数
    init_map();

    bool flag = true;
    while(flag)
    {
        cout << "——————————————————————————————————————————————————————————————————————\n"
             << "请按照规则输入指令进行相应的防火墙控制，使用方式如下：\n"
             << "退出程序            q\n"
             << "——————————————————————————————————————————————————————————————————————\n"
             << "请输入指令：\n";

        string cmd;
        getline(cin,cmd);
        stringstream ss(cmd);
        ss >> cmd;
        cout<<cmd<<endl;

        if(cmd == "q"){
            flag = false;
        }
    }

    close(socket_fd);
    system("sudo rmmod firewall");
}
