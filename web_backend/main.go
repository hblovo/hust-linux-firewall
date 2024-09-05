package main

import (
	"web_backend/core"
	"web_backend/utils"
)

func main() {
	//global.DB = initialize.Gorm()
	//initialize.RegisterUserTables()
	//utils.MAIN()
	utils.Write_rules()
	utils.ReadRules()
	core.RunWindowsServer()

}
