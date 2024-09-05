package main

import (
	"web_backend/core"
	"web_backend/global"
	"web_backend/initialize"
	"web_backend/utils"
)

func main() {
	global.DB = initialize.Gorm()
	initialize.RegisterUserTables()
	utils.MAIN()
	core.RunWindowsServer()
}
