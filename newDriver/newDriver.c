#include <ntifs.h>
#define _DRIVER
#ifndef _DRIVER
/*
针对Windbg的双机调试的保护
针对打开进程的NtOpenProcess的保护
针对读内存的NtReadVirtualMemory的保护
针对写内存的NtWriteVirtualMemory的保护
针对打开线程的NtOpenThread的保护
针对附加进程的KeAttchProcess的保护
针对附加进程的KeStackAttachProcess的保护
针对附加进程的KeForceAttachProcess的保护
针对恢复进程的NtResumeProcess的保护
针对恢复线程的NtResumeThread的保护
针对调试信息传递相关的DbgkpPostFakeThreadMessages的保护
针对唤醒调试目标的DbgkpWakeTarget的保护
针对调试权限标志的ValidAccessMask清零的保护
针对调试对象的DebugPort清零的保护
*/
#endif

//////////////////////////////////////////////////////////////////////////
// 此程序目前有两个问题; 
// 1.路径如果是中文会导致取值失败;
// 2.当程序第二次启动时会再次进行自身拷贝但路径错误;
//////////////////////////////////////////////////////////////////////////

// 卸载函数;
VOID DrvUnload(PDRIVER_OBJECT pDriver)
{
	KdPrint(("驱动卸载函数执行;\n"));
}

// 删除文件;
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
		KdPrint(("删除文件失败;\n"));
	}
	return STATUS_SUCCESS;
}

// 内核拷贝;
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
	// 打开要复制的文件;
	status = ZwOpenFile(&hfile, GENERIC_ALL, &objabs, &iostack, FILE_SHARE_READ | FILE_SHARE_WRITE, FILE_SYNCHRONOUS_IO_NONALERT);
	if(!NT_SUCCESS(status))
	{
		KdPrint(("打开源文件失败,错误码:%x;\n", status));
		ZwClose(hfile);
		return status;
	}
	// 得到文件大小;
	status = ZwQueryInformationFile(hfile, &iostack, &fsi, sizeof(fsi), FileStandardInformation);
	if(!NT_SUCCESS(status))
	{
		KdPrint(("查询文件大小失败,错误码:%x;\n", status));
		ZwClose(hfile);
		return status;
	}
	// 申请内存空间;
	fileBuffer = ExAllocatePool(NonPagedPool, (SIZE_T)fsi.EndOfFile.QuadPart);
	if(!fileBuffer)
	{
		KdPrint(("申请空间失败,错误码:%x;\n", status));
		ZwClose(hfile);
		return status;
	}
	// 初始化申请到的空间;
	RtlZeroMemory(fileBuffer, (SIZE_T)fsi.EndOfFile.QuadPart);
	readoffset.QuadPart = 0x00;		// 从文件的那里开始读; 
	// 读取目标文件到内存;
	status = ZwReadFile(hfile, NULL, NULL, NULL, &iostack, fileBuffer, (SIZE_T)fsi.EndOfFile.QuadPart, &readoffset, NULL);
	if(!NT_SUCCESS(status))
	{
		KdPrint(("文件读取失败,错误码:%x;\n", status));
		ZwClose(hfile);
		ExFreePool(fileBuffer);
		return status;
	}
	KdPrint(("实际读取数量:%d;\n", iostack.Information));
	ZwClose(hfile);
	// 创建新文件;
	RtlInitUnicodeString(&destfilepath, destfile_path);
	InitializeObjectAttributes(&objabs2, &destfilepath, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);
	status = ZwCreateFile(&hfile2, GENERIC_ALL, &objabs2, &iostack2, NULL, FILE_ATTRIBUTE_NORMAL, FILE_SHARE_WRITE, FILE_SUPERSEDE, FILE_SYNCHRONOUS_IO_NONALERT, NULL, 0);
	if(!NT_SUCCESS(status))
	{
		KdPrint(("文件创建失败,错误码:%x;\n", status));
		ExFreePool(fileBuffer);
		ZwClose(hfile2);
		return status;
	}
	writeoffset.QuadPart = 0;
	// 从内存将数据写入指定路径;
	status = ZwWriteFile(hfile2, NULL, NULL, NULL, &iostack2, fileBuffer, (SIZE_T)fsi.EndOfFile.QuadPart, &writeoffset, NULL);
	if(!NT_SUCCESS(status))
	{
		KdPrint(("文件写入失败,错误码:%x;\n", status));
		ExFreePool(fileBuffer);
		ZwClose(hfile2);
		return status;
	}
	KdPrint(("实际写入数量:%d;\n", iostack.Information));
	// 释放空间关闭句柄;
	ExFreePool(fileBuffer);
	// 如果最后不调用ZwClose来关闭句柄,则会导致目标文件被占用;
	ZwClose(hfile2);

	return STATUS_SUCCESS;
}

// 驱动程序入口,相当于main函数;
NTSTATUS DriverEntry(PDRIVER_OBJECT driver, PUNICODE_STRING reg_path)
{
	// 文件的基本操作;

	// 设置一个返回值;
	NTSTATUS status = STATUS_SUCCESS;

	// 删除文件;
	// KernelDeleteFile(L"\\??\\C:\\abC.txt");

	// 拷贝文件;
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

	// 设置一个驱动卸载函数;
	driver -> DriverUnload = DrvUnload;

	// 打开注册表;
	InitializeObjectAttributes(&obja, reg_path, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);
	// 打开注册表的函数 Openkey  ZwOpenKey 和 ZwCreateKey;
	status = ZwOpenKey(&keyhandle, KEY_ALL_ACCESS, &obja);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("ZwOpenKey执行失败,错误码 %d;\n", status));
		if (keyhandle) ZwClose(keyhandle);
		return 0;
	}
	keyinfo = ExAllocatePool(NonPagedPool, 0x1000);
	if (!keyinfo)
	{
		KdPrint(("ExAllocatePool执行失败,错误码 %d;\n", status));
		return 0;
	}
	RtlZeroMemory(keyinfo, 0x1000);
	RtlInitUnicodeString(&name, L"ImagePath");
	status = ZwQueryValueKey(keyhandle, &name, KeyValuePartialInformation, keyinfo, 0x1000, &keyop);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("ZwQueryValueKey执行失败,错误码 %d;\n", status));
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
		KdPrint(("KernelCopyFile执行失败,错误码 %d;\n", status));
		goto cleanup;
	}

	rootpath = L"\\SystemRoot\\System32\\drivers\\NewDriver.sys";

	status = ZwSetValueKey(keyhandle, &name, 0, REG_EXPAND_SZ, rootpath, wcslen(rootpath) * 2 + 2);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("设置字符串失败,错误码:%d\n", status));
		goto cleanup;
		return 0;
	}
	// 写入注册表值(使用运行时函数);
	tempstart = 0x1;
	RtlWriteRegistryValue(RTL_REGISTRY_ABSOLUTE, reg_path -> Buffer, L"Start", REG_DWORD, &tempstart, 0x4);
	// 运行时函数 ZwDeleteKey 此函数只能删除没有子项的注册表项;
	// RtlCheckRegistryKey 检测某一个注册表项是否存在;
	// RtlCreateRegistryKey 检测某一个注册表项是否存在,没有则创建;

	// TO DO..;

cleanup:
		ZwClose(keyhandle);
		ExFreePool(keyinfo);
		return status;
}