package model

// RuleAction 定义规则的动作
type RuleAction int

const (
	Accept RuleAction = iota
	Drop
	Reject
)

// ProtocolType 定义协议类型
type ProtocolType int

const (
	TCP ProtocolType = iota
	UDP
	ICMP
)

// FirewallRule 定义防火墙规则的数据结构
type FirewallRule struct {
	ID              int    `json:"id"`
	SourceIP        string `json:"source_ip"`
	SourcePort      string `json:"source_port"`
	DestinationIP   string `json:"destination_ip"`
	DestinationPort string `json:"destination_port"`
	Protocol        string `json:"protocol"`
	Action          string `json:"action"`
}

// FirewallRuleset 维护防火墙规则的链表
type FirewallRuleset struct {
	Rules []FirewallRule // 规则列表
}

// AddRule 添加新规则到规则链表
func (r *FirewallRuleset) AddRule(rule FirewallRule) {
	r.Rules = append(r.Rules, rule)
}

// GetRules 返回当前的所有规则
func (r *FirewallRuleset) GetRules() []FirewallRule {
	return r.Rules
}

// DeleteRule 删除指定索引的规则
func (r *FirewallRuleset) DeleteRule(index int) {
	if index >= 0 && index < len(r.Rules) {
		r.Rules = append(r.Rules[:index], r.Rules[index+1:]...)
	}
}
