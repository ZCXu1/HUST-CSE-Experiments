#include <iostream>
#include<windows.h>
#include<stdio.h>
#include <stdlib.h>
#include <fstream>

#include "tchar.h"

#pragma comment(lib, "ws2_32.lib")  
#define nNumberOfBytes 1024
using namespace std;

void FileOperation();
void writeFile();
void readFile();
void copyFile();
void writeToExe();
void CopyMyselfTo(LPCTSTR targetPath);
int main()
{
	FileOperation();
	return 0;
}

void FileOperation() {
	cout << "请按Enter键开始读取文件" << endl;
	getchar();
	readFile();
	cout << endl;
	cout << "请按Enter键开始写入文件" << endl;
	getchar();
	writeFile();
	cout << "请问是否让文件复制到多个目录(1:yes,0:no)\n";
	int n;
	cin >> n;
	if (n == 1) {
		copyFile();
	}
	else if (n != 0) {
		cout << "输入错误\n" << endl;
	}
	cout << "请问是否让文件自我复制(1:yes,0:no)\n";
	int b;
	cin >> b;
	if (b == 1) {
		LPCTSTR targetPath = _T("D:\\hook\\newExe.exe");
		CopyMyselfTo(targetPath);
	}
	else if (b != 0) {
		cout << "输入错误\n" << endl;
	}
	cout << "请问是否修改exe(D:\\hook\\Metro.exe)(1:yes,0:no)\n";
	int a;
	cin >> a;
	if (a == 1) {
		writeToExe();
	}
	else if (a != 0) {
		cout << "输入错误\n" << endl;
	}

}


void readFile() {
	bool flag;
	HANDLE h = (HANDLE)CreateFile(L"D:\\hook\\InjectDll\\Debug\\in.txt", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_ALWAYS, NULL, NULL);
	if (h == INVALID_HANDLE_VALUE) {
		h = NULL;
		return;
	}
	
	char s[nNumberOfBytes]= "\0";
	flag = ReadFile(h, s, nNumberOfBytes, NULL, NULL);
	if (flag) {
		cout << "成功读入文件 内容如下" << endl;
		cout << s << endl;
	}
	FlushFileBuffers(h);
	

}
void writeFile() {
	bool flag;

	HANDLE h = (HANDLE)CreateFile(L"D:\\hook\\InjectDll\\Debug\\out.txt", GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, NULL, NULL);
	if (h == INVALID_HANDLE_VALUE) {
		h = NULL;
		return;
	}
	char s[1024] = "";
	cout << "请输入一个字符串：";
	cin >> s;
	cout << endl;
	flag = WriteFile(h, s, strlen(s), NULL, NULL);
	FlushFileBuffers(h);
	/*CloseHandle(h);*/
}
void copyFile() {
	CopyFileA("D:\\hook\\FileOperation\\in.txt", "D:\\hook\\Client\\newin.txt", false);
	CopyFileA("D:\\hook\\FileOperation\\in.txt", "D:\\hook\\HeapOperation\\newin.txt", false);
	CopyFileA("D:\\hook\\FileOperation\\in.txt", "D:\\hook\\HttpOperation\\newin.txt", false);
	CopyFileA("D:\\hook\\FileOperation\\in.txt", "D:\\hook\\RegOperation\\newin.txt", false);
	printf("成功复制\n");
}

void writeToExe() {
	fstream f;
	//追加写入,在原来基础上加了ios::app 
	f.open("D:\\hook\\Metro.exe", ios::out | ios::app);
	for (int i = 0; i < 10; i++)
	{
		f << "test" << endl;
	}
	f.close();
	
}


void CopyMyselfTo(LPCTSTR targetPath)
{
	TCHAR tcBuf[_MAX_PATH]; // 缓冲区
   // 取自己程序的程序名
	GetModuleFileName(NULL, tcBuf, sizeof(tcBuf));
	// 复制文件
	CopyFile(tcBuf, targetPath, FALSE);
}

