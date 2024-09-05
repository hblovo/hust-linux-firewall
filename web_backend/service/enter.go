package service

import "web_backend/service/rule"

type ServiceGroup struct {
	RuleServiceGroup rule.ServiceGroup
}

var ServiceGroupApp = new(ServiceGroup)
