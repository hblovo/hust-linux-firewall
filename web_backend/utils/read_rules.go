package utils

import (
	"fmt"
	"github.com/google/nftables"
	"github.com/google/nftables/expr"
	"net"
)

func ReadRules() {
	conn := &nftables.Conn{}

	// 列出所有表
	tables, err := conn.ListTables()
	if err != nil {
		fmt.Println("Error listing tables:", err)
		return
	}

	for _, table := range tables {
		fmt.Printf("Table: %s (Family: %d)\n", table.Name, table.Family)

		// 列出表中的所有链
		chains, err := conn.ListChains()
		if err != nil {
			fmt.Println("Error listing chains:", err)
			return
		}

		for _, chain := range chains {
			fmt.Printf("  Chain: %s\n", chain.Name)

			// 列出链中的所有规则
			rules, err := conn.GetRules(table, chain)
			if err != nil {
				fmt.Println("Error getting rule:", err)
				return
			}

			for _, rule := range rules {
				fmt.Printf("    Rule: %v\n", rule)
				for _, e := range rule.Exprs {
					switch ex := e.(type) {
					case *expr.Payload:
						fmt.Printf("      Payload expression: %+v\n", ex)
					case *expr.Cmp:
						fmt.Printf("      Compare expression: %+v\n", ex)
					case *expr.Verdict:
						fmt.Printf("      Verdict: %+v\n", ex)
					default:
						fmt.Printf("      Unknown expression: %+v\n", ex)
					}
				}
			}
		}
	}
	fmt.Printf("******************************************************\n")
}
func Write_rules() {
	// 创建与nftables的连接
	fmt.Printf("******************************************************\n")
	conn := &nftables.Conn{}

	// 创建一个新的防火墙表（如果需要）
	table := &nftables.Table{
		Family: nftables.TableFamilyIPv4, // 如果想使用 IPv4
		Name:   "my_filter",
	}
	conn.AddTable(table)

	// 创建一个链
	chain := &nftables.Chain{
		Name:     "my_chain",
		Table:    table,
		Type:     nftables.ChainTypeFilter,
		Hooknum:  nftables.ChainHookForward, // 选择不同的挂钩点 (e.g., input, output, forward)
		Priority: nftables.ChainPriorityFilter,
	}
	conn.AddChain(chain)

	// 定义一个防火墙规则
	rule := &nftables.Rule{
		Table: table,
		Chain: chain,
		Exprs: []expr.Any{
			// 匹配源IP地址为192.168.1.1的规则
			&expr.Payload{
				DestRegister: 1,
				Base:         expr.PayloadBaseNetworkHeader,
				Offset:       12, // IPv4源地址的偏移
				Len:          4,
			},
			&expr.Cmp{
				Op:       expr.CmpOpEq,
				Register: 1,
				Data:     []byte(net.ParseIP("192.168.1.1").To4()),
			},
			// 匹配动作: 丢弃数据包
			&expr.Verdict{
				Kind: expr.VerdictDrop,
			},
		},
	}
	conn.AddRule(rule)

	// 提交更改
	if err := conn.Flush(); err != nil {
		panic(fmt.Sprintf("写入规则失败: %v", err))
	}

	fmt.Println("规则已成功写入到内核")
	fmt.Printf("******************************************************\n")
}
