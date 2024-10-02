#include <ntifs.h>
#define _DRIVER
#ifndef _DRIVER
/*
���Windbg��˫�����Եı���
��Դ򿪽��̵�NtOpenProcess�ı���
��Զ��ڴ��NtReadVirtualMemory�ı���
���д�ڴ��NtWriteVirtualMemory�ı���
��Դ��̵߳�NtOpenThread�ı���
��Ը��ӽ��̵�KeAttchProcess�ı���
��Ը��ӽ��̵�KeStackAttachProcess�ı���
��Ը��ӽ��̵�KeForceAttachProcess�ı���
��Իָ����̵�NtResumeProcess�ı���
��Իָ��̵߳�NtResumeThread�ı���
��Ե�����Ϣ������ص�DbgkpPostFakeThreadMessages�ı���
��Ի��ѵ���Ŀ���DbgkpWakeTarget�ı���
��Ե���Ȩ�ޱ�־��ValidAccessMask����ı���
��Ե��Զ����DebugPort����ı���
*/
#endif

//////////////////////////////////////////////////////////////////////////
// �˳���Ŀǰ����������; 
// 1.·����������Ļᵼ��ȡֵʧ��;
// 2.������ڶ�������ʱ���ٴν�����������·������;
//////////////////////////////////////////////////////////////////////////

// ж�غ���;
VOID DrvUnload(PDRIVER_OBJECT pDriver)
{
	KdPrint(("����ж�غ���ִ��;\n"));
}

// ɾ���ļ�;
NTSTATUS KernelDeleteFile(PWCHAR file_path)
{
	UNICODE_STRING filepath = { 0 };
	NTSTATUS status = STATUS_SUCCESS;
	OBJECT_ATTRIBUTES objabs = { 0 };
	RtlInitUnicodeString(&filepath, file_path);
	InitializeObjectAttributes(&objabs, &filepath, OBJ_CASE_INSENSITIVE, NULL, NULL);
	status = ZwDeleteFile(&objabs);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("ɾ���ļ�ʧ��;\n"));
	}
	return STATUS_SUCCESS;
}

// �ں˿���;
NTSTATUS KernelCopyFile(PWCHAR destfile_path, PWCHAR sourcefile_path)
{

	NTSTATUS status = STATUS_SUCCESS;
	HANDLE hfile = NULL;
	UNICODE_STRING sourcefilepath = { 0 };
	OBJECT_ATTRIBUTES objabs = { 0 };
	IO_STATUS_BLOCK iostack = { 0 };
	FILE_STANDARD_INFORMATION fsi = { 0 };
	LARGE_INTEGER readoffset = { 0 };
	PVOID fileBuffer = NULL;
	HANDLE hfile2 = NULL;
	UNICODE_STRING destfilepath = { 0 };
	OBJECT_ATTRIBUTES objabs2 = { 0 };
	IO_STATUS_BLOCK iostack2 = { 0 };
	LARGE_INTEGER writeoffset = { 0 };

	RtlInitUnicodeString(&sourcefilepath, sourcefile_path);
	InitializeObjectAttributes(&objabs, &sourcefilepath, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);
	// ��Ҫ���Ƶ��ļ�;
	status = ZwOpenFile(&hfile, GENERIC_ALL, &objabs, &iostack, FILE_SHARE_READ | FILE_SHARE_WRITE, FILE_SYNCHRONOUS_IO_NONALERT);
	if(!NT_SUCCESS(status))
	{
		KdPrint(("��Դ�ļ�ʧ��,������:%x;\n", status));
		ZwClose(hfile);
		return status;
	}
	// �õ��ļ���С;
	status = ZwQueryInformationFile(hfile, &iostack, &fsi, sizeof(fsi), FileStandardInformation);
	if(!NT_SUCCESS(status))
	{
		KdPrint(("��ѯ�ļ���Сʧ��,������:%x;\n", status));
		ZwClose(hfile);
		return status;
	}
	// �����ڴ�ռ�;
	fileBuffer = ExAllocatePool(NonPagedPool, (SIZE_T)fsi.EndOfFile.QuadPart);
	if(!fileBuffer)
	{
		KdPrint(("����ռ�ʧ��,������:%x;\n", status));
		ZwClose(hfile);
		return status;
	}
	// ��ʼ�����뵽�Ŀռ�;
	RtlZeroMemory(fileBuffer, (SIZE_T)fsi.EndOfFile.QuadPart);
	readoffset.QuadPart = 0x00;		// ���ļ������￪ʼ��; 
	// ��ȡĿ���ļ����ڴ�;
	status = ZwReadFile(hfile, NULL, NULL, NULL, &iostack, fileBuffer, (SIZE_T)fsi.EndOfFile.QuadPart, &readoffset, NULL);
	if(!NT_SUCCESS(status))
	{
		KdPrint(("�ļ���ȡʧ��,������:%x;\n", status));
		ZwClose(hfile);
		ExFreePool(fileBuffer);
		return status;
	}
	KdPrint(("ʵ�ʶ�ȡ����:%d;\n", iostack.Information));
	ZwClose(hfile);
	// �������ļ�;
	RtlInitUnicodeString(&destfilepath, destfile_path);
	InitializeObjectAttributes(&objabs2, &destfilepath, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);
	status = ZwCreateFile(&hfile2, GENERIC_ALL, &objabs2, &iostack2, NULL, FILE_ATTRIBUTE_NORMAL, FILE_SHARE_WRITE, FILE_SUPERSEDE, FILE_SYNCHRONOUS_IO_NONALERT, NULL, 0);
	if(!NT_SUCCESS(status))
	{
		KdPrint(("�ļ�����ʧ��,������:%x;\n", status));
		ExFreePool(fileBuffer);
		ZwClose(hfile2);
		return status;
	}
	writeoffset.QuadPart = 0;
	// ���ڴ潫����д��ָ��·��;
	status = ZwWriteFile(hfile2, NULL, NULL, NULL, &iostack2, fileBuffer, (SIZE_T)fsi.EndOfFile.QuadPart, &writeoffset, NULL);
	if(!NT_SUCCESS(status))
	{
		KdPrint(("�ļ�д��ʧ��,������:%x;\n", status));
		ExFreePool(fileBuffer);
		ZwClose(hfile2);
		return status;
	}
	KdPrint(("ʵ��д������:%d;\n", iostack.Information));
	// �ͷſռ�رվ��;
	ExFreePool(fileBuffer);
	// �����󲻵���ZwClose���رվ��,��ᵼ��Ŀ���ļ���ռ��;
	ZwClose(hfile2);

	return STATUS_SUCCESS;
}

// �����������,�൱��main����;
NTSTATUS DriverEntry(PDRIVER_OBJECT driver, PUNICODE_STRING reg_path)
{
	// �ļ��Ļ�������;

	// ����һ������ֵ;
	NTSTATUS status = STATUS_SUCCESS;

	// ɾ���ļ�;
	// KernelDeleteFile(L"\\??\\C:\\abC.txt");

	// �����ļ�;
	// KernelCopyFile(L"\\??\\C:\\abc.exe", L"\\??\\C:\\123.exe");

	
	HANDLE keyhandle = NULL;
	OBJECT_ATTRIBUTES obja = { 0 };
	UNICODE_STRING regPath = { 0 };
	ULONG keyop = 0;
	PVOID keyinfo = NULL;
	UNICODE_STRING name = { 0 };
	PKEY_VALUE_PARTIAL_INFORMATION tempInfo = { 0 };
	PWCHAR imagePath = NULL;
	PWCHAR rootpath = NULL;
	ULONG tempstart = (ULONG)NULL;

	// ����һ������ж�غ���;
	driver -> DriverUnload = DrvUnload;

	// ��ע���;
	InitializeObjectAttributes(&obja, reg_path, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);
	// ��ע���ĺ��� Openkey  ZwOpenKey �� ZwCreateKey;
	status = ZwOpenKey(&keyhandle, KEY_ALL_ACCESS, &obja);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("ZwOpenKeyִ��ʧ��,������ %d;\n", status));
		if (keyhandle) ZwClose(keyhandle);
		return 0;
	}
	keyinfo = ExAllocatePool(NonPagedPool, 0x1000);
	if (!keyinfo)
	{
		KdPrint(("ExAllocatePoolִ��ʧ��,������ %d;\n", status));
		return 0;
	}
	RtlZeroMemory(keyinfo, 0x1000);
	RtlInitUnicodeString(&name, L"ImagePath");
	status = ZwQueryValueKey(keyhandle, &name, KeyValuePartialInformation, keyinfo, 0x1000, &keyop);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("ZwQueryValueKeyִ��ʧ��,������ %d;\n", status));
		goto cleanup;
		return 0;
	}
	tempInfo = (PKEY_VALUE_PARTIAL_INFORMATION)keyinfo;
	imagePath = (PWCHAR)(tempInfo -> Data);
	KdPrint(("ImagePath:%ws\n", imagePath));

	// C:\WINDOWS\system32\drivers          \SystemRoot\System32\drivers\NewDriver.sys

	// \\??\\C:\NewDriver.sys
	status = KernelCopyFile(L"\\??\\C:\\WINDOWS\\system32\\drivers\\NewDriver.sys", imagePath);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("KernelCopyFileִ��ʧ��,������ %d;\n", status));
		goto cleanup;
	}

	rootpath = L"\\SystemRoot\\System32\\drivers\\NewDriver.sys";

	status = ZwSetValueKey(keyhandle, &name, 0, REG_EXPAND_SZ, rootpath, wcslen(rootpath) * 2 + 2);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("�����ַ���ʧ��,������:%d\n", status));
		goto cleanup;
		return 0;
	}
	// д��ע���ֵ(ʹ������ʱ����);
	tempstart = 0x1;
	RtlWriteRegistryValue(RTL_REGISTRY_ABSOLUTE, reg_path -> Buffer, L"Start", REG_DWORD, &tempstart, 0x4);
	// ����ʱ���� ZwDeleteKey �˺���ֻ��ɾ��û�������ע�����;
	// RtlCheckRegistryKey ���ĳһ��ע������Ƿ����;
	// RtlCreateRegistryKey ���ĳһ��ע������Ƿ����,û���򴴽�;

	// TO DO..;

cleanup:
		ZwClose(keyhandle);
		ExFreePool(keyinfo);
		return status;
}