package utils

import (
	"encoding/xml"
	"fmt"
	"gorm.io/gorm"
	"io/ioutil"
	"os"
	"web_backend/global"
)

type Problem struct {
	gorm.Model
	Title        string `gorm:"type:varchar(255);not null"`
	TimeLimit    int    `gorm:"not null"`
	MemoryLimit  int    `gorm:"not null"`
	Description  string `gorm:"type:text;not null"`
	InputFormat  string `gorm:"type:text"`
	OutputFormat string `gorm:"type:text"`
	SampleInput  string `gorm:"type:text"`
	SampleOutput string `gorm:"type:text"`
	TestInput    string `gorm:"type:text"`
	TestOutput   string `gorm:"type:text"`
	Hint         string `gorm:"type:text"`
	Source       string `gorm:"type:text"`
}

func setupDatabase(db *gorm.DB) {
	db.AutoMigrate(&Problem{})
}

func parseXML(data []byte) ([]Problem, error) {
	var root struct {
		Items []struct {
			Title        string `xml:"title"`
			TimeLimit    int    `xml:"time_limit"`
			MemoryLimit  int    `xml:"memory_limit"`
			Description  string `xml:"description"`
			InputFormat  string `xml:"input"`
			OutputFormat string `xml:"output"`
			SampleInput  string `xml:"sample_input"`
			SampleOutput string `xml:"sample_output"`
			TestInput    string `xml:"test_input"`
			TestOutput   string `xml:"test_output"`
			Hint         string `xml:"hint"`
			Source       string `xml:"source"`
		} `xml:"item"`
	}

	if err := xml.Unmarshal(data, &root); err != nil {
		return nil, err
	}

	problems := make([]Problem, len(root.Items))
	for i, item := range root.Items {
		problems[i] = Problem{
			Title:        item.Title,
			TimeLimit:    item.TimeLimit,
			MemoryLimit:  item.MemoryLimit,
			Description:  item.Description,
			InputFormat:  item.InputFormat,
			OutputFormat: item.OutputFormat,
			SampleInput:  item.SampleInput,
			SampleOutput: item.SampleOutput,
			TestInput:    item.TestInput,
			TestOutput:   item.TestOutput,
			Hint:         item.Hint,
			Source:       item.Source,
		}
	}
	return problems, nil
}

func MAIN() {
	db := global.DB
	// 设置数据库表
	setupDatabase(db)

	// 读取 XML 文件
	filePath := `C:\Users\hbl\Downloads\fps-loj-small-pics\fps-loj-small-pics.xml`
	xmlData, err := ioutil.ReadFile(filePath)
	if err != nil {
		fmt.Println("Error reading XML file:", err)
		os.Exit(1)
	}

	// 解析 XML 数据
	problems, err := parseXML(xmlData)
	fmt.Println(problems)
	if err != nil {
		fmt.Println("Error parsing XML:", err)
		return
	}

	// 将解析的数据插入到数据库中
	for _, problem := range problems {
		if err := db.Create(&problem).Error; err != nil {
			fmt.Println("Error inserting data:", err)
		}
	}

	fmt.Println("Data successfully inserted into the database.")
}
