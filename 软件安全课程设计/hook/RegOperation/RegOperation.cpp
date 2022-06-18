
#include<windows.h>
#include<stdio.h>
#include <stdlib.h>

#pragma comment(lib, "ws2_32.lib")
#include <iostream>
using namespace std;

void regCreateAndSetValue();
void regOpenAndDelValue();
void createRun();


int main()
{
	printf("请按Enter键开始创建并设置值\n");
	getchar();
	regCreateAndSetValue();
	printf("请按Enter键开始打开并删除值\n");
	getchar();
	regOpenAndDelValue();
	printf("请按Enter键开始创建自启动执行文件项\n");
	getchar();
	createRun();
	getchar();
	return 0;
}

void regCreateAndSetValue() {
	// 创建注册表并设置键值
	HKEY hKey = NULL;
	TCHAR Data[1024];
	memset(Data, 0, sizeof(Data));
	wcsncpy_s(Data, TEXT("xuzichuan"), 1024);

	size_t lRet = RegCreateKeyEx(HKEY_CURRENT_USER, (LPWSTR)L"OldKey", 0, NULL, REG_OPTION_NON_VOLATILE,
		KEY_ALL_ACCESS, NULL, &hKey, NULL);
	if (lRet == ERROR_SUCCESS) {
		printf("成功创建!\n");
	}
	else {
		printf("创建失败!\n");
	}
	// 修改注册表键值，没有则创建它
	size_t iLen = wcslen(Data);
	// 设置键值
	lRet = RegSetValueEx(hKey, L"NewKey", 0, REG_SZ, (CONST BYTE*)Data, sizeof(TCHAR) * iLen);
	if (lRet == ERROR_SUCCESS)
	{
		printf("成功设值!\n");
		return;
	}
	else {
		printf("设值失败!\n");
	}
	RegCloseKey(hKey);
}
void regOpenAndDelValue() {
	HKEY hKey = NULL;
	size_t lRet = RegOpenKeyEx(HKEY_CURRENT_USER, (LPWSTR)L"OldKey", 0, KEY_ALL_ACCESS, &hKey);
	if (lRet == ERROR_SUCCESS) {
		printf("打开成功!\n");
	}
	else {
		printf("打开失败!\n");
	}
	lRet = RegDeleteValue(hKey, L"NewKey");
	if (lRet == ERROR_SUCCESS) {
		printf("删除成功!\n");
	}
	else {
		printf("删除失败!\n");
	}
	RegCloseKey(hKey);
}

void createRun() {
	
	HKEY hKey = NULL;
	TCHAR Data[1024];
	memset(Data, 0, sizeof(Data));
	wcsncpy_s(Data, TEXT("xuzichuan"), 1024);

	size_t lRet = RegCreateKeyEx(HKEY_CURRENT_USER, (LPWSTR)L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", 0, NULL, REG_OPTION_NON_VOLATILE,
		KEY_ALL_ACCESS, NULL, &hKey, NULL);
	if (lRet == ERROR_SUCCESS) {
		printf("成功创建自启动执行文件项!\n");
	}
	else {
		printf("创建失败!\n");
	}
	// 修改注册表键值，没有则创建它
	size_t iLen = wcslen(Data);
	// 设置键值
	lRet = RegSetValueEx(hKey, L"Self-Starting", 0, REG_SZ, (CONST BYTE*)Data, sizeof(TCHAR) * iLen);
	if (lRet == ERROR_SUCCESS)
	{
		printf("成功设值!\n");
		return;
	}
	else {
		printf("设值失败!\n");
	}
	RegCloseKey(hKey);
}