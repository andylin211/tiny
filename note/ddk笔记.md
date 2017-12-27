## 下载ddk并安装

开发xp上运行的驱动还是用ddk吧：

https://www.microsoft.com/en-us/download/details.aspx?id=11800

下载wdk的话：

https://developer.microsoft.com/zh-cn/windows/hardware/windows-driver-kit

## 目录下makefile和sources

这个不改：
```
!INCLUDE $(NTMAKEENV)\makefile.def
```

示例：
```
TARGETNAME=hellosys
TARGETTYPE=DRIVER
SOURCES=hellosys.c
TARGETPATH=obj
```

## hello world测试下

```
#include <ntddk.h>

UNICODE_STRING g_device_name = RTL_CONSTANT_STRING(L"\\Device\\hellosys");
UNICODE_STRING g_syblnk_name = RTL_CONSTANT_STRING(L"\\??\\hellosys");
PDEVICE_OBJECT g_cdo = 0;

VOID unload_func(PDRIVER_OBJECT driver)
{
	NTSTATUS status = STATUS_SUCCESS;
	HANDLE pid = PsGetCurrentProcessId();
	DbgPrint("unloading...pid=%d\n", (USHORT)pid);
	
	status = IoDeleteSymbolicLink(&g_syblnk_name);
	DbgPrint("IoDeleteSymbolicLink %d", status);
	ASSERT(g_cdo != 0);
	IoDeleteDevice(g_cdo);
}

NTSTATUS dispatch_func(IN PDEVICE_OBJECT dev, IN PIRP irp)
{
	NTSTATUS status = STATUS_SUCCESS;

	PIO_STACK_LOCATION irpsp = IoGetCurrentIrpStackLocation(irp);
	do
	{
		if (dev != g_cdo)
			break;

		if (irpsp->MajorFunction == IRP_MJ_CREATE || irpsp->MajorFunction == IRP_MJ_CLOSE)
			break;

		if (irpsp->MajorFunction == IRP_MJ_DEVICE_CONTROL)
		{
			PVOID buffer = irp->AssociatedIrp.SystemBuffer;
			ULONG in_len = irpsp->Parameters.DeviceIoControl.InputBufferLength;
			ULONG out_len = irpsp->Parameters.DeviceIoControl.OutputBufferLength;
			ULONG len = 0;

			switch (irpsp->Parameters.DeviceIoControl.IoControlCode)
			{
			default:
				status = STATUS_INVALID_PARAMETER;
				break;
			}
		}
		break;
	} while (0);

	irp->IoStatus.Information = ret_len;
	irp->IoStatus.Status = status;
	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return status;
}


NTSTATUS DriverEntry(PDRIVER_OBJECT driver, PUNICODE_STRING reg_path)
{
	NTSTATUS status = STATUS_SUCCESS;
	INT i = 0;
	
	HANDLE pid = PsGetCurrentProcessId();
	DbgPrint("starting...pid=%d\n", (USHORT)pid);
	
	status = IoCreateDevice(
		driver,
		0, 
		&g_device_name,
		FILE_DEVICE_UNKNOWN,
		FILE_DEVICE_SECURE_OPEN,
		FALSE, 
		&g_cdo);
	if (!NT_SUCCESS(status))
		return status;

	status = IoDeleteSymbolicLink(&g_syblnk_name);
	DbgPrint("IoDeleteSymbolicLink %d", status);
	status = IoCreateSymbolicLink(&g_syblnk_name, &g_device_name);
	if (!NT_SUCCESS(status))
	{
		IoDeleteDevice(g_cdo);
		return status;
	}

	for (i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; i++)
		driver->MajorFunction[i] = dispatch_func;
	
	driver->DriverUnload = unload_func;
	g_cdo->Flags &= ~DO_DEVICE_INITIALIZING;
	return STATUS_SUCCESS;
}
```

vs2015 创建新项目，
添加makefile，写入内容
添加sources，写入内容
添加hellosys.c，写入内容
添加include目录

打开build命令行环境xp check，进入当前代码目录，build

调试模式进入xp，拷贝hellosys.sys
cmd
copy hellosys.sys c:\
sc create hellosys binPath= "c:\hellosys.sys" start= demand type= kernel
sc start hellosys




