package problem

import (
	"github.com/gin-gonic/gin"
	"net/http"
	"strconv"
	"web_backend/api/common/response"
)

type ProblemApi struct{}

func (api *ProblemApi) GetProblemList(c *gin.Context) {
	pageStr := c.DefaultQuery("page", "1")
	sizeStr := c.DefaultQuery("size", "10")
	keyword := c.Query("keyword")
	page, _ := strconv.Atoi(pageStr)
	size, _ := strconv.Atoi(sizeStr)
	PageResult := ProblemService.GetProblemList(page, size, keyword)
	response.OkWithPageResult(*PageResult, c)
}
func (api *ProblemApi) AddProblem(c *gin.Context) {
	c.JSON(http.StatusOK, gin.H{
		"err": false,
	})
}
