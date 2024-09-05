package rule

import (
	"github.com/gin-gonic/gin"
	"net/http"
	"strconv"
	"web_backend/api/common/response"
)

package rule

import (
	"github.com/gin-gonic/gin"
	"net/http"
	"strconv"
	"web_backend/api/common/response"
)

type RuleApi struct{}

func (api *RuleApi) GetProblemList(c *gin.Context) {
	PageResult := RuleService.GetProblemList()
	response.OkWithPageResult(*PageResult, c)
}
func (api *ProblemApi) AddProblem(c *gin.Context) {
	c.JSON(http.StatusOK, gin.H{
		"err": false,
	})
}
