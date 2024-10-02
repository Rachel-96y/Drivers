// Driver_R3.cpp : 定义控制台应用程序的入口点;
//

#include "stdafx.h"

#include <list>			// 链表;
#include <vector>		// Vector;
#include <set>			// 搜索二叉树;

#include <string>		// string类;
#include <iostream>		// 输入输出流;


using namespace std;

/*
#define  SUCCESS TRUE

struct qqq
{
	int a;
	int b;
	void funca()
	{
		this -> a = 0x4554;
		this -> b = 0x4234;
	}
};

struct ccc
{
	int a;
	int b;
	void funca()
	{
		this -> a = 2;
		this -> b = 3;
	}
};

class CExample
{
public:
	BOOL function(DWORD dwNumber, LPVOID LPCALLBACK)
	{
		int nRet = SUCCESS;
		BOOL (WINAPI* pCallBack)(qqq*, ccc*) = (BOOL (WINAPI* )(qqq*, ccc*))LPCALLBACK;
		qqq q;
		ccc c;
		pCallBack(&q, &c);
		return nRet;
	}
};

BOOL WINAPI a(qqq* vvv, ccc* yyy)
{
	vvv -> a = 16541;
	vvv -> b = 41213;
	yyy -> a = 4241;
	yyy -> b = 12;
	printf("完成;\n");
	return 0;
}
*/

int _tmain(int argc, _TCHAR* argv[])
{
	// Vector的使用方式;
	string a = "可怕\n";
	cout << a;
	cout << "哈哈哈哈" << endl;
	int arr[] = {1, 2, 3, 4, 5};
	vector<int> v1(3, 10);
	v1[0] = 1;

	vector<int>::iterator b = v1.begin();

	printf("%d\n", b[0]);

	char name[50];		// string name
	char password[50];	// string password
	cout << "请输入您的名称及密码： \t"<< endl;
	cin >> name >> password;
	cout << "您的名称是： \t" << name << "连续输出\t" << password << "\t结束!" <<endl;

/*
	DWORD dwOldProtect = 0;
	const char* const a = "1234";
	printf("%s\n", a);
	// 更改内存属性;
	VirtualProtect((LPVOID)a, 0x5, PAGE_READWRITE, &dwOldProtect);
	*(char*)(a + 1) = '5';
	// 恢复内存属性;
	VirtualProtect((LPVOID)a, 0x5, dwOldProtect, 0);
	printf("%s\n", a);
	
*/

/*
	// 回调函数的构造;
	CExample aaaa;
	aaaa.function(123, a);
*/



/*
	// R3程序打开驱动设备对象;
	TCHAR szReadBudder[0x50] = { 0 };
	DWORD dwBread = NULL;
	HANDLE hDevice = CreateFile(L"\\\\.\\MyFirstDevice", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hDevice == INVALID_HANDLE_VALUE)
	{
		printf("打开设备失败;\n");
		system("pause");
		return 0;
	}
	printf("打开设备成功;\n");
	ReadFile(hDevice, (LPVOID)szReadBudder, 0x50, &dwBread, NULL);
	printf("缓冲区值:%s_缓冲区的数据长度:%d;\n", szReadBudder, dwBread);
	system("pause");
	printf("WriteFile函数执行,向内核缓冲区写入数据,长度:%d;\n", strlen("这则消息来自R3"));
	WriteFile(hDevice, "这则消息来自R3", strlen("这则消息来自R3"), &dwBread, NULL);
	system("pause");
	printf("关闭句柄;\n");
	CloseHandle(hDevice);
*/

	system("pause");
	return 0;
}
