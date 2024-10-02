#include <ntddk.h>

#define DEVICE_NAME L"\\Device\\MyFirstDevice"
#define SYMBOLIC_NAME L"\\??\\MyFirstDevice"

// 卸载函数;
VOID DrvUnload(PDRIVER_OBJECT pDriver)
{
	UNICODE_STRING usSymbolicName = { 0 };
	KdPrint(("驱动卸载函数执行...\n"));
	// 删除设备及符号链接;
	if (pDriver -> DeviceObject)
	{
		IoDeleteDevice(pDriver -> DeviceObject);
		RtlInitUnicodeString(&usSymbolicName, SYMBOLIC_NAME);
		IoDeleteSymbolicLink(&usSymbolicName);
	}
}

// R3程序访问此设备对象时执行的函数;
NTSTATUS MyCreate(PDEVICE_OBJECT pDevice, PIRP pirp)
{
	 NTSTATUS status = STATUS_SUCCESS;
	 KdPrint(("MyDevice设备创建了...\n"));
	 pirp -> IoStatus.Status = status;
	 pirp -> IoStatus.Information = 0;
	 IoCompleteRequest(pirp, IO_NO_INCREMENT);
	 return STATUS_SUCCESS;
}

// R3程序调用CloseHandle后次执行的函数
NTSTATUS MyClose(PDEVICE_OBJECT pDevice, PIRP pirp)
{
	NTSTATUS status = STATUS_SUCCESS;
	KdPrint(("MyClose设备关闭了...\n"));
	pirp -> IoStatus.Status = status;
	pirp -> IoStatus.Information = 0;
	IoCompleteRequest(pirp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

// R3程序调用CloseHandle后先执行的函数
NTSTATUS MyCleanUp(PDEVICE_OBJECT pDevice, PIRP pirp)
{
	NTSTATUS status = STATUS_SUCCESS;
	KdPrint(("MyCleanUp函数运行了...\n"));
	pirp -> IoStatus.Status = status;
	pirp -> IoStatus.Information = 0;
	IoCompleteRequest(pirp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

// R3程序调用ReadFile后执行的函数
NTSTATUS MyRead(PDEVICE_OBJECT pDevice, PIRP pirp)
{
	NTSTATUS status = STATUS_SUCCESS;

	PIO_STACK_LOCATION pStack = NULL;
	ULONG uReadSize = (ULONG)NULL;
	PTCHAR szReadBuffer = NULL;
	pStack = IoGetCurrentIrpStackLocation(pirp);
	uReadSize = pStack -> Parameters.Read.Length;
	szReadBuffer = (PTCHAR)pirp -> AssociatedIrp.SystemBuffer;
	RtlCopyMemory(szReadBuffer, "这则消息来自内核", strlen("这则消息来自内核"));
	KdPrint(("MyRead函数运行了...\n"));
	pirp -> IoStatus.Status = status;
	pirp -> IoStatus.Information = strlen("这则消息来自内核");
	KdPrint(("R3实际读取的长度是:%d\n", strlen("这则消息来自内核")));
	IoCompleteRequest(pirp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

// R3程序调用WriteFile后执行的函数
NTSTATUS MyWrite(PDEVICE_OBJECT pDevice, PIRP pirp)
{
	NTSTATUS status = STATUS_SUCCESS;

	PIO_STACK_LOCATION pStack = NULL;
	ULONG uWriteSize = (ULONG)NULL;
	PTCHAR szWriteBuffer = NULL;
	pStack = IoGetCurrentIrpStackLocation(pirp);
	uWriteSize = pStack -> Parameters.Write.Length;
	szWriteBuffer = (PTCHAR)pirp -> AssociatedIrp.SystemBuffer;
	KdPrint(("MyWrite函数运行了...\n"));
	RtlZeroMemory(pDevice -> DeviceExtension, 200);
	RtlCopyMemory(pDevice -> DeviceExtension, szWriteBuffer, uWriteSize);
	KdPrint(("内核缓冲区地址:%p__值:%s\n", szWriteBuffer, (PTCHAR)pDevice -> DeviceExtension));
	pirp -> IoStatus.Status = status;
	pirp -> IoStatus.Information = strlen((PTCHAR)pDevice -> DeviceExtension);
	KdPrint(("R3写入的数据长度: %d\n", strlen((PTCHAR)pDevice -> DeviceExtension)));
	IoCompleteRequest(pirp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}


// 驱动程序入口,相当于main函数;
NTSTATUS DriverEntry(PDRIVER_OBJECT driver, PUNICODE_STRING reg_path)
{
	// 和R3程序进行通信;

	NTSTATUS status = STATUS_SUCCESS;		// 返回值;
	PDEVICE_OBJECT pDevice = NULL;			// 设备对象;
	UNICODE_STRING usDeviceName = { 0 };	// 设备名称;
	UNICODE_STRING usSymbolicName = { 0 };	// 符号链接名称;
	// 设置一个驱动卸载函数;
	driver -> DriverUnload = DrvUnload;
	// 用于转换Unicode字符串;
	RtlInitUnicodeString(&usDeviceName, DEVICE_NAME);
	// 创建设备对象; 200BYTE,设备扩展;
	status = IoCreateDevice(driver, 200, &usDeviceName, FILE_DEVICE_UNKNOWN, 0, TRUE, &pDevice);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("创建设备对象失败,错误码: %x\n", status));
		return status;
	}
	// 设置交互方式;
	pDevice -> Flags |= DO_BUFFERED_IO;
	// 创建符号链接;
	RtlInitUnicodeString(&usSymbolicName, SYMBOLIC_NAME);
	status = IoCreateSymbolicLink(&usSymbolicName, &usDeviceName);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("创建符号链接失败,错误码: %x\n", status));
		IoDeleteDevice(pDevice);
		return status;
	}
	// 设置响应函数(回调函数);
	driver -> MajorFunction[IRP_MJ_CREATE] = MyCreate;
	driver -> MajorFunction[IRP_MJ_CLEANUP] = MyCleanUp;
	driver -> MajorFunction[IRP_MJ_CLOSE] = MyClose;
	driver -> MajorFunction[IRP_MJ_READ] = MyRead;
	driver -> MajorFunction[IRP_MJ_WRITE] = MyWrite;
	
	return STATUS_SUCCESS;
}