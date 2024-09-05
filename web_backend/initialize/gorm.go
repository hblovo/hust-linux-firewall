package initialize

import (
	"gorm.io/gorm"
	"web_backend/global"
	"web_backend/model"
)

func Gorm() *gorm.DB {
	return GormMysql()
}
func RegisterUserTables() {
	db := global.DB
	db.AutoMigrate(
		model.Problem{},
	)

}
