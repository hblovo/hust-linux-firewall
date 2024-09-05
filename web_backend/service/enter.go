package service

import "web_backend/service/problem"

type ServiceGroup struct {
	ProblemServiceGroup problem.ServiceGroup
}

var ServiceGroupApp = new(ServiceGroup)
