package initialize

import (
	"github.com/gin-gonic/gin"
)

func Routers() *gin.Engine {
	Router := gin.New()
	Router.Use(gin.Recovery())
	if gin.Mode() == gin.DebugMode {
		Router.Use(gin.Logger())
	}
	//problemRouter := router.RouterGroupApp.Problem
	//// 跨域，如需跨域可以打开下面的注释
	//Router.Use(middleware.Cors()) // 直接放行全部跨域请求
	//// Router.Use(middleware.CorsByRules()) // 按照配置的规则放行跨域请求
	//PrivateGroup := Router.Group("")
	//PrivateGroup.Use()
	//{
	//	problemRouter.InitProblemRouter(PrivateGroup)
	//}
	return Router
}
