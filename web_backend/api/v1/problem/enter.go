package problem

import "web_backend/service"

type ApiGroup struct {
	ProblemApi
}

var (
	ProblemService = service.ServiceGroupApp.ProblemServiceGroup.ProblemService
)
