package response

import (
	"github.com/gin-gonic/gin"
	"net/http"
)

type Response struct {
	Err  bool        `json:"err"`
	Data interface{} `json:"data"`
	Msg  interface{} `json:"msg"`
}

type Page struct {
	Total     int64 `json:"total"`
	PageNum   int   `json:"page_num"`
	PageSize  int   `json:"page_size"`
	PageCount int   `json:"page_count"`
}

type ResponseForPageResult struct {
	Flag  bool        `json:"err"`
	Data  interface{} `json:"data"`
	Msg   interface{} `json:"msg"`
	*Page `json:"page"`
}

// 是否失败
const (
	FAIL    = true
	SUCCESS = false
)

func Result(err bool, data interface{}, msg interface{}, c *gin.Context) {
	c.JSON(http.StatusOK, Response{
		err,
		data,
		msg,
	})
}

func ResultForPageResult(err bool, data PageResult, c *gin.Context) {
	page := new(Page)
	page.Total = data.Total
	page.PageNum = data.PageNum
	page.PageSize = data.PageSize
	page.PageCount = data.PageCount
	page.Total = data.Total

	c.JSON(http.StatusOK, ResponseForPageResult{
		err,
		data.List,
		nil,
		page,
	})
}

func OkWithData(data interface{}, c *gin.Context) {
	Result(SUCCESS, data, nil, c)
}

func OkWithPageResult(data PageResult, c *gin.Context) {
	ResultForPageResult(SUCCESS, data, c)
}

func FailWithMessage(message interface{}, c *gin.Context) {
	Result(FAIL, nil, message, c)
}

func NoAuth(message string, c *gin.Context) {
	c.JSON(http.StatusUnauthorized, Response{
		false,
		nil,
		message,
	})
}
