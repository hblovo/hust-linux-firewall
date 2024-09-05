package rule

import (
	"fmt"
	"github.com/google/nftables"
	"github.com/google/nftables/expr"
	"net"
	"strconv"
	"web_backend/model"
)

type RuleService struct{}

func (apiService *RuleService) GetRuleList() []model.FirewallRule {
	// 初始化 nftables 连接
	conn := &nftables.Conn{}

	// 获取所有的防火墙规则表
	tables, err := conn.ListTables()
	if err != nil {
		fmt.Println("Error fetching tables:", err)
		return nil
	}

	var firewallRules []model.FirewallRule

	// 遍历表，获取规则链和规则
	for _, table := range tables {
		chains, err := conn.ListChains()
		if err != nil {
			fmt.Println("Error fetching chains:", err)
			continue
		}

		for _, chain := range chains {
			rules, err := conn.GetRules(table, chain)
			if err != nil {
				fmt.Println("Error fetching rules:", err)
				continue
			}

			for _, rule := range rules {
				// 解析规则表达式
				firewallRule := parseRule(rule)
				if firewallRule != nil {
					firewallRules = append(firewallRules, *firewallRule)
				}
			}
		}
	}

	return firewallRules
}

// 解析单条规则的表达式，转换为 FirewallRule
func parseRule(rule *nftables.Rule) *model.FirewallRule {
	var srcIP, dstIP, srcPort, dstPort, protocol, action string

	for _, e := range rule.Exprs {
		switch expr := e.(type) {
		case *expr.Meta:
			if expr.Key == expr.MetaKeyL4PROTO {
				protocol = strconv.Itoa(int(expr.Register))
			}
		case *expr.Payload:
			if expr.Base == expr.PayloadBaseNetworkHeader {
				if expr.Offset == 12 { // Source IP
					srcIP = net.IP(expr.Data).String()
				} else if expr.Offset == 16 { // Destination IP
					dstIP = net.IP(expr.Data).String()
				}
			} else if expr.Base == expr.PayloadBaseTransportHeader {
				if expr.Offset == 0 { // Source Port
					srcPort = strconv.Itoa(int(expr.Data[0]))
				} else if expr.Offset == 2 { // Destination Port
					dstPort = strconv.Itoa(int(expr.Data[0]))
				}
			}
		case *expr.Verdict:
			if expr.Kind == expr.VerdictAccept {
				action = "accept"
			} else if expr.Kind == expr.VerdictDrop {
				action = "drop"
			}
		}
	}

	if srcIP == "" && dstIP == "" {
		return nil
	}

	return &model.FirewallRule{
		SourceIP:        srcIP,
		SourcePort:      srcPort,
		DestinationIP:   dstIP,
		DestinationPort: dstPort,
		Protocol:        protocol,
		Action:          action,
	}
}
