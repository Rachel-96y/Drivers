// Driver_R3.cpp : �������̨Ӧ�ó������ڵ�;
//

#include "stdafx.h"

#include <list>			// ����;
#include <vector>		// Vector;
#include <set>			// ����������;

#include <string>		// string��;
#include <iostream>		// ���������;


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
	printf("���;\n");
	return 0;
}
*/

int _tmain(int argc, _TCHAR* argv[])
{
	// Vector��ʹ�÷�ʽ;
	string a = "����\n";
	cout << a;
	cout << "��������" << endl;
	int arr[] = {1, 2, 3, 4, 5};
	vector<int> v1(3, 10);
	v1[0] = 1;

	vector<int>::iterator b = v1.begin();

	printf("%d\n", b[0]);

	char name[50];		// string name
	char password[50];	// string password
	cout << "�������������Ƽ����룺 \t"<< endl;
	cin >> name >> password;
	cout << "���������ǣ� \t" << name << "�������\t" << password << "\t����!" <<endl;

/*
	DWORD dwOldProtect = 0;
	const char* const a = "1234";
	printf("%s\n", a);
	// �����ڴ�����;
	VirtualProtect((LPVOID)a, 0x5, PAGE_READWRITE, &dwOldProtect);
	*(char*)(a + 1) = '5';
	// �ָ��ڴ�����;
	VirtualProtect((LPVOID)a, 0x5, dwOldProtect, 0);
	printf("%s\n", a);
	
*/

/*
	// �ص������Ĺ���;
	CExample aaaa;
	aaaa.function(123, a);
*/



/*
	// R3����������豸����;
	TCHAR szReadBudder[0x50] = { 0 };
	DWORD dwBread = NULL;
	HANDLE hDevice = CreateFile(L"\\\\.\\MyFirstDevice", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hDevice == INVALID_HANDLE_VALUE)
	{
		printf("���豸ʧ��;\n");
		system("pause");
		return 0;
	}
	printf("���豸�ɹ�;\n");
	ReadFile(hDevice, (LPVOID)szReadBudder, 0x50, &dwBread, NULL);
	printf("������ֵ:%s_�����������ݳ���:%d;\n", szReadBudder, dwBread);
	system("pause");
	printf("WriteFile����ִ��,���ں˻�����д������,����:%d;\n", strlen("������Ϣ����R3"));
	WriteFile(hDevice, "������Ϣ����R3", strlen("������Ϣ����R3"), &dwBread, NULL);
	system("pause");
	printf("�رվ��;\n");
	CloseHandle(hDevice);
*/

	system("pause");
	return 0;
}
