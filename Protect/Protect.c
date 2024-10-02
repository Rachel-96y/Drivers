#include <ntifs.h>
#include <wdm.h>
#include <windef.h>

typedef struct _LDR_DATA_TABLE_ENTRY {
	// Start from Windows7
	LIST_ENTRY64 InLoadOrderLinks;
	LIST_ENTRY64 InMemoryOrderLinks;
	LIST_ENTRY64 InInitializationOrderLinks;
	PVOID DllBase;
	PVOID EntryPoint;
	ULONG SizeOfImage;
	UNICODE_STRING FullDllName;
	UNICODE_STRING BaseDllName;
	ULONG Flags;
} LDR_DATA_TABLE_ENTRY, *PLDR_DATA_TABLE_ENTRY;

NTKERNELAPI UCHAR *PsGetProcessImageFileName(PEPROCESS EProcess);


LARGE_INTEGER cookie = { 0 };
PVOID _HANDLE = NULL;

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

// 卸载函数;
VOID DrvUnload(PDRIVER_OBJECT pDriver)
{
	CmUnRegisterCallback(cookie);
	ObUnRegisterCallbacks(_HANDLE);
	KdPrint(("驱动卸载函数执行;\n"));
}

// 注册表回调;
NTSTATUS RegistryCallBack(PVOID callbackcontext, PVOID arg1, PVOID arg2)
{
	// 参数1决定通知的类型,参数2决定操作数据;
	REG_NOTIFY_CLASS tempclass = (REG_NOTIFY_CLASS)arg1;
	PREG_CREATE_KEY_INFORMATION pkInfi = { 0 };
	UNICODE_STRING tempname = { 0 };
	NTSTATUS status = STATUS_SUCCESS;
	RtlInitUnicodeString(&tempname, L"*ASDIOPXXAADCGR");
	// KdPrint(("%p\n"));
	switch(tempclass)
	{
	case  RegNtPreOpenKey:
	case  RegNtPreOpenKeyEx:
	case  RegNtPreCreateKey:
	case  RegNtPreCreateKeyEx:
		{
			// KdPrint(("注册表被创建或者打开;\n"));
			pkInfi = (PREG_CREATE_KEY_INFORMATION)arg2;
			__try
			{
				if (FsRtlIsNameInExpression(&tempname, pkInfi->CompleteName, TRUE, NULL))
				{
					KdPrint(("%wZ", pkInfi->CompleteName));
					status = STATUS_UNSUCCESSFUL;
				}
				
			}
			__except (1)
			{
				KdPrint(("错误的内存\n"));
			}
			break;
		}
	default:
		break;
	}

	return status;
}


OB_PREOP_CALLBACK_STATUS ProtectProcess(PVOID RegistrationContext, POB_PRE_OPERATION_INFORMATION OperationInformation)
{
	PUCHAR imagefilename = PsGetProcessImageFileName(OperationInformation->Object);
	if (strstr(imagefilename, "calc"))
	{
		OperationInformation->Parameters->CreateHandleInformation.DesiredAccess = 0;
		OperationInformation->Parameters->DuplicateHandleInformation.DesiredAccess = 0;
	}
	return OB_PREOP_SUCCESS;
}

// 驱动程序入口,相当于main函数;
NTSTATUS DriverEntry(PDRIVER_OBJECT driver, PUNICODE_STRING reg_path)
{
	// 注册表和进程保护;

	// 初始化;
	NTSTATUS status = STATUS_SUCCESS;
	PLDR_DATA_TABLE_ENTRY ldr = { 0 };
	OB_CALLBACK_REGISTRATION ob = { 0 };
	OB_OPERATION_REGISTRATION oor = { 0 };
	UNICODE_STRING attde = { 0 };

	// 设置一个驱动卸载函数;
	driver -> DriverUnload = DrvUnload;

	// 监控注册表;
	status = CmRegisterCallback(RegistryCallBack, (PVOID)0x123456, &cookie);

	// 进程保护;
	ldr = (PLDR_DATA_TABLE_ENTRY)driver->DriverSection;
	ldr -> Flags |= 0x20;
	ob.Version = ObGetFilterVersion();
	ob.OperationRegistrationCount = 1;
	ob.OperationRegistration = &oor;
	RtlInitUnicodeString(&attde, L"32199");
	ob.RegistrationContext = NULL;
	oor.ObjectType = PsProcessType;
	oor.Operations = OB_OPERATION_HANDLE_CREATE | OB_OPERATION_HANDLE_DUPLICATE;
	oor.PreOperation = ProtectProcess;
	oor.PostOperation = NULL;
	status = ObRegisterCallbacks(&ob, &_HANDLE);

	return status;
}
