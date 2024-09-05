package problem

import (
	"github.com/gin-gonic/gin"
	v1 "web_backend/api/v1"
)

type ProblemRouter struct{}

func (r *ProblemRouter) InitProblemRouter(router *gin.RouterGroup) {
	ProblemApiRouter := router.Group("problem")
	ProblemApi := v1.ApiGroupApp.ProblemApiGroup.ProblemApi
	{
		ProblemApiRouter.GET("list", ProblemApi.GetProblemList)
		ProblemApiRouter.POST("add", ProblemApi.AddProblem)
	}
}
