#include <ntddk.h>

#define DEVICE_NAME L"\\Device\\MyFirstDevice"
#define SYMBOLIC_NAME L"\\??\\MyFirstDevice"

// ж�غ���;
VOID DrvUnload(PDRIVER_OBJECT pDriver)
{
	UNICODE_STRING usSymbolicName = { 0 };
	KdPrint(("����ж�غ���ִ��...\n"));
	// ɾ���豸����������;
	if (pDriver -> DeviceObject)
	{
		IoDeleteDevice(pDriver -> DeviceObject);
		RtlInitUnicodeString(&usSymbolicName, SYMBOLIC_NAME);
		IoDeleteSymbolicLink(&usSymbolicName);
	}
}

// R3������ʴ��豸����ʱִ�еĺ���;
NTSTATUS MyCreate(PDEVICE_OBJECT pDevice, PIRP pirp)
{
	 NTSTATUS status = STATUS_SUCCESS;
	 KdPrint(("MyDevice�豸������...\n"));
	 pirp -> IoStatus.Status = status;
	 pirp -> IoStatus.Information = 0;
	 IoCompleteRequest(pirp, IO_NO_INCREMENT);
	 return STATUS_SUCCESS;
}

// R3�������CloseHandle���ִ�еĺ���
NTSTATUS MyClose(PDEVICE_OBJECT pDevice, PIRP pirp)
{
	NTSTATUS status = STATUS_SUCCESS;
	KdPrint(("MyClose�豸�ر���...\n"));
	pirp -> IoStatus.Status = status;
	pirp -> IoStatus.Information = 0;
	IoCompleteRequest(pirp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

// R3�������CloseHandle����ִ�еĺ���
NTSTATUS MyCleanUp(PDEVICE_OBJECT pDevice, PIRP pirp)
{
	NTSTATUS status = STATUS_SUCCESS;
	KdPrint(("MyCleanUp����������...\n"));
	pirp -> IoStatus.Status = status;
	pirp -> IoStatus.Information = 0;
	IoCompleteRequest(pirp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

// R3�������ReadFile��ִ�еĺ���
NTSTATUS MyRead(PDEVICE_OBJECT pDevice, PIRP pirp)
{
	NTSTATUS status = STATUS_SUCCESS;

	PIO_STACK_LOCATION pStack = NULL;
	ULONG uReadSize = (ULONG)NULL;
	PTCHAR szReadBuffer = NULL;
	pStack = IoGetCurrentIrpStackLocation(pirp);
	uReadSize = pStack -> Parameters.Read.Length;
	szReadBuffer = (PTCHAR)pirp -> AssociatedIrp.SystemBuffer;
	RtlCopyMemory(szReadBuffer, "������Ϣ�����ں�", strlen("������Ϣ�����ں�"));
	KdPrint(("MyRead����������...\n"));
	pirp -> IoStatus.Status = status;
	pirp -> IoStatus.Information = strlen("������Ϣ�����ں�");
	KdPrint(("R3ʵ�ʶ�ȡ�ĳ�����:%d\n", strlen("������Ϣ�����ں�")));
	IoCompleteRequest(pirp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

// R3�������WriteFile��ִ�еĺ���
NTSTATUS MyWrite(PDEVICE_OBJECT pDevice, PIRP pirp)
{
	NTSTATUS status = STATUS_SUCCESS;

	PIO_STACK_LOCATION pStack = NULL;
	ULONG uWriteSize = (ULONG)NULL;
	PTCHAR szWriteBuffer = NULL;
	pStack = IoGetCurrentIrpStackLocation(pirp);
	uWriteSize = pStack -> Parameters.Write.Length;
	szWriteBuffer = (PTCHAR)pirp -> AssociatedIrp.SystemBuffer;
	KdPrint(("MyWrite����������...\n"));
	RtlZeroMemory(pDevice -> DeviceExtension, 200);
	RtlCopyMemory(pDevice -> DeviceExtension, szWriteBuffer, uWriteSize);
	KdPrint(("�ں˻�������ַ:%p__ֵ:%s\n", szWriteBuffer, (PTCHAR)pDevice -> DeviceExtension));
	pirp -> IoStatus.Status = status;
	pirp -> IoStatus.Information = strlen((PTCHAR)pDevice -> DeviceExtension);
	KdPrint(("R3д������ݳ���: %d\n", strlen((PTCHAR)pDevice -> DeviceExtension)));
	IoCompleteRequest(pirp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}


// �����������,�൱��main����;
NTSTATUS DriverEntry(PDRIVER_OBJECT driver, PUNICODE_STRING reg_path)
{
	// ��R3�������ͨ��;

	NTSTATUS status = STATUS_SUCCESS;		// ����ֵ;
	PDEVICE_OBJECT pDevice = NULL;			// �豸����;
	UNICODE_STRING usDeviceName = { 0 };	// �豸����;
	UNICODE_STRING usSymbolicName = { 0 };	// ������������;
	// ����һ������ж�غ���;
	driver -> DriverUnload = DrvUnload;
	// ����ת��Unicode�ַ���;
	RtlInitUnicodeString(&usDeviceName, DEVICE_NAME);
	// �����豸����; 200BYTE,�豸��չ;
	status = IoCreateDevice(driver, 200, &usDeviceName, FILE_DEVICE_UNKNOWN, 0, TRUE, &pDevice);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("�����豸����ʧ��,������: %x\n", status));
		return status;
	}
	// ���ý�����ʽ;
	pDevice -> Flags |= DO_BUFFERED_IO;
	// ������������;
	RtlInitUnicodeString(&usSymbolicName, SYMBOLIC_NAME);
	status = IoCreateSymbolicLink(&usSymbolicName, &usDeviceName);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("������������ʧ��,������: %x\n", status));
		IoDeleteDevice(pDevice);
		return status;
	}
	// ������Ӧ����(�ص�����);
	driver -> MajorFunction[IRP_MJ_CREATE] = MyCreate;
	driver -> MajorFunction[IRP_MJ_CLEANUP] = MyCleanUp;
	driver -> MajorFunction[IRP_MJ_CLOSE] = MyClose;
	driver -> MajorFunction[IRP_MJ_READ] = MyRead;
	driver -> MajorFunction[IRP_MJ_WRITE] = MyWrite;
	
	return STATUS_SUCCESS;
}