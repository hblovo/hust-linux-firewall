package model

import "encoding/xml"

//	type Problem struct {
//		Identity   string `gorm:"column:identity;type:varchar(36);" json:"identity"`        // 问题表的唯一标识
//		CategoryId string `gorm:"column:category_id;type:varchar(255);" json:"category_id"` // 分类ID，已逗号分隔
//		Title      string `gorm:"column:title;type:varchar(255);" json:"title"`             // 文章标题
//		Content    string `gorm:"column:content;type:text;" json:"content"`                 // 文章正文
//		MaxRuntime int    `gorm:"column:max_runtime;type:int(11);" json:"max_runtime"`      // 最大运行时长
//		MaxMem     int    `gorm:"column:max_mem;type:int(11);" json:"max_mem"`              // 最大运行内存
//	}
//
//	func (table *Problem) TableName() string {
//		return "problem"
//	}
type Problem struct {
	ID            uint   `gorm:"primaryKey"`
	Title         string `gorm:"column:title"`
	TimeLimit     string `gorm:"column:time_limit"`
	TimeUnit      string `gorm:"column:time_unit"`
	MemoryLimit   string `gorm:"column:memory_limit"`
	MemoryUnit    string `gorm:"column:memory_unit"`
	Description   string `gorm:"column:description"`
	InputFormat   string `gorm:"column:input_format"`
	OutputFormat  string `gorm:"column:output_format"`
	ExampleInput  string `gorm:"column:example_input"`
	ExampleOutput string `gorm:"column:example_output"`
	DataRange     string `gorm:"column:data_range"`
}
type ProblemSet struct {
	XMLName   xml.Name `xml:"fps"`
	Version   string   `xml:"version,attr"`
	Url       string   `xml:"url,attr"`
	Generator struct {
		Name string `xml:"name,attr"`
		Url  string `xml:"url,attr"`
	} `xml:"generator"`
	Items []Problem `xml:"item"` // 使用切片表示多个题目
}

// 指定表名
func (Problem) TableName() string {
	return "problems" // 这里指定表名为 problems
}
