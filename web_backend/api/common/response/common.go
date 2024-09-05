package response

type PageResult struct {
	List      interface{} `json:"list"`
	Total     int64       `json:"total"`
	PageNum   int         `json:"page_num"`
	PageSize  int         `json:"page_size"`
	PageCount int         `json:"page_count"`
}
