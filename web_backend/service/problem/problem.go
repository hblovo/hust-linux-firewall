package problem

import (
	"github.com/gin-gonic/gin"
	"math"
	"web_backend/api/common/response"
	"web_backend/global"
	"web_backend/model"
)

type ProblemService struct{}

func (apiService *ProblemService) GetProblemList(page int, size int, keyword string) *response.PageResult {
	var data []model.Problem
	var count int64
	db := global.DB
	if db != nil {
		if keyword != "" {
			db = db.Where("title like ? OR content like ?", "%"+keyword+"%", "%"+keyword+"%")
		}
		db.Table("problem").Count(&count).Offset((page - 1) * size).Order("id").Limit(size).Find(&data)
	}
	page_count := math.Ceil(float64(count) / float64(size))
	pageResult := response.PageResult{List: data, PageCount: int(page_count), PageNum: page, PageSize: size, Total: count}
	return &pageResult
}
func (apiService *ProblemService) AddProblem(c *gin.Context) {

}
