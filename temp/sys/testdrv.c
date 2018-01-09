#include <ntddk.h>

#define BUFFER_SIZE 256
#define MEM_TAG 'test'

int t_strncpy(char* dst, char* src, int max_count)
{
	int i = 0;
	if (!dst || !src)
		return 0;

	for (i = 0; i < max_count; i++)
	{
		if (src[i] == 0)
			break;
		dst[i] = src[i];
	}
	dst[i] = 0;
	return i;
}

int t_strnlen(char* buffer, int max_count)
{
	int i = 0;
	if (!buffer)
		return 0;

	for (i = 0; i < max_count; i++)
	{
		if (buffer[i] == 0)
			break;
	}
	return i;
}


VOID Print(PUNICODE_STRING pustr)
{
	WCHAR pBuffer[BUFFER_SIZE] = { 0 };
	NTSTATUS status;
	STRING str = { 0 };
	str.Buffer = pBuffer;
	str.MaximumLength = BUFFER_SIZE;

	status = RtlUnicodeStringToAnsiString(&str, pustr, FALSE);
	if (status == STATUS_SUCCESS)
		DbgPrint("%Z", &str);
}

VOID TestUnicodeString()
{
	NTSTATUS status = STATUS_SUCCESS;
	WCHAR pBuffer[BUFFER_SIZE] = { 0 };
	UNICODE_STRING ustr_const = RTL_CONSTANT_STRING(L"这是常数字符串\n");
	UNICODE_STRING ustr_copy = RTL_CONSTANT_STRING(L"通过RtlCopyUnicodeString函数来复制unicode string\n");
	UNICODE_STRING ustr_append = RTL_CONSTANT_STRING(L"通过RtlAppendUnicodeStringToString追加的\n");
	UNICODE_STRING ustr = { 0 };	

	DbgPrint("====测试Unicode String=====\n");

	Print(&ustr_const);

	RtlInitUnicodeString(&ustr, L"通过RtlInitUnicodeString函数来初始化unicode string\n");
	Print(&ustr);	

	RtlInitEmptyUnicodeString(&ustr, pBuffer, BUFFER_SIZE * sizeof(WCHAR));
	RtlCopyUnicodeString(&ustr, &ustr_copy);
	Print(&ustr);

	status = RtlAppendUnicodeToString(&ustr, L"这是RtlAppendUnicodeToString追加的\n");
	if (status == STATUS_SUCCESS)
		Print(&ustr);
	else
		DbgPrint("RtlAppendUnicodeToString失败, 0x%x\n", status);

	status = RtlAppendUnicodeStringToString(&ustr, &ustr_append);
	if (status == STATUS_SUCCESS)
		Print(&ustr);
	else
		DbgPrint("RtlAppendUnicodeToString失败, 0x%x\n", status);
}

VOID TestExAllocatePoolWithTag()
{	
	INT i = 0;
	CHAR* pBuffer = (CHAR*)ExAllocatePoolWithTag(NonPagedPool, BUFFER_SIZE, MEM_TAG);

	DbgPrint("====测试Allocate Pool=====\n");
	if (pBuffer == 0)
		return;

	RtlZeroMemory(pBuffer, BUFFER_SIZE);

	for (i = 0; i < 20; i++)
		pBuffer[i] = 'a' + i;

	DbgPrint("%s\n",pBuffer);

	ExFreePool(pBuffer);
	pBuffer = 0;
}

VOID TestListEntry()
{
	typedef struct {
		INT id;
		LIST_ENTRY list_entry;
		UNICODE_STRING name;
	}STUDENT, *PSTUDENT;

	INT i = 0;
	LIST_ENTRY list_head;
	PLIST_ENTRY plist = 0;
	PSTUDENT pStudent = 0;
	LPCWSTR names[5] = { L"乔丹", L"林旋风", L"黑曼巴", L"土豆", L"超人" };

	DbgPrint("====测试List Entry=====\n");

	InitializeListHead(&list_head);

	for (i = 0; i < 5; i++)
	{
		pStudent = (PSTUDENT)ExAllocatePoolWithTag(NonPagedPool, sizeof(STUDENT), MEM_TAG);
		if (!pStudent)
			return;

		pStudent->id = i + 1;
		RtlInitUnicodeString(&pStudent->name, names[i]);
		InsertTailList(&list_head, &pStudent->list_entry);//InsertHeadList
	}
	
	for (plist = list_head.Flink; plist != &list_head; plist = plist->Flink)
	{
		pStudent = CONTAINING_RECORD(plist, STUDENT, list_entry);
		DbgPrint("id=%d,name=", pStudent->id);
		Print(&pStudent->name);
		DbgPrint("\n");
	}
	
	while (!IsListEmpty(&list_head))
	{
		plist = RemoveHeadList(&list_head);
		pStudent = CONTAINING_RECORD(plist, STUDENT, list_entry);
		ExFreePool(pStudent);
	}

}

VOID TestLargeInteger()
{
	LARGE_INTEGER a, b, c;
	DbgPrint("====测试Large Integer=====\n");
	a.QuadPart = 0x12345678;
	b.QuadPart = 0xffffffff;
	c.QuadPart = a.QuadPart + b.QuadPart;
	DbgPrint("%u,%u + %u,%u = %u,%u\n", a.HighPart, a.LowPart, b.HighPart, b.LowPart, c.HighPart, c.LowPart);
	DbgPrint("%x,%x + %x,%x = %x,%x\n", a.HighPart, a.LowPart, b.HighPart, b.LowPart, c.HighPart, c.LowPart);
}


KSPIN_LOCK my_spin_lock; //global variable!
INT shared_v = 0;

VOID TestSpinLock()
{
	KIRQL irql;
	DbgPrint("====测试Spin Lock(useless here)=====\n");
	KeInitializeSpinLock(&my_spin_lock);
	KeAcquireSpinLock(&my_spin_lock, &irql);
	shared_v++;
	KeReleaseSpinLock(&my_spin_lock, irql);
	//ExInterlockedInsertHeadList();
	//ExInterlockedRemoveHeadList();
	//KeAcquireInStackQueuedSpinLock();
	//KeReleaseInStackQueuedSpinLock();
}

VOID TestFileOperation()
{
	NTSTATUS status;
	HANDLE file_handle=NULL;
	IO_STATUS_BLOCK io_status;
	OBJECT_ATTRIBUTES oa;
	UNICODE_STRING ufile_name = RTL_CONSTANT_STRING(L"\\??\\C:\\a.txt");
	CHAR buffer[BUFFER_SIZE] = "hello world\r\n";
	ULONG length = 13;
	LARGE_INTEGER offset = { 0 };
	DbgPrint("====测试文件操作 =====\n");	

	do
	{
		InitializeObjectAttributes(&oa,
			&ufile_name,
			OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
			NULL, NULL);

		status = ZwCreateFile(
			&file_handle,
			GENERIC_WRITE | GENERIC_READ,
			&oa,
			&io_status,
			NULL,
			FILE_ATTRIBUTE_NORMAL,
			FILE_SHARE_READ,
			FILE_OPEN_IF,
			FILE_NON_DIRECTORY_FILE |
			FILE_RANDOM_ACCESS |
			FILE_SYNCHRONOUS_IO_NONALERT,
			NULL,
			0);
		if (!NT_SUCCESS(status))
		{
			DbgPrint("create fail 0x%x\n", status);
			break;
		}			

		status = ZwWriteFile(file_handle, NULL, NULL, NULL, &io_status, buffer, length, &offset, NULL);
		if (!NT_SUCCESS(status))
		{
			DbgPrint("write fail 0x%x\n", status);
			break;
		}
		DbgPrint("write status: %x\n", status);

		RtlZeroMemory(buffer, BUFFER_SIZE);
		status = ZwReadFile(file_handle, NULL, NULL, NULL, &io_status, buffer, length, &offset, NULL);
		if (!NT_SUCCESS(status))
		{
			DbgPrint("read fail 0x%x\n", status);
			break;
		}

		DbgPrint("read ok, %s", buffer);

	} while (0);

	status = ZwClose(file_handle);
	if (!NT_SUCCESS(status))
	{
		DbgPrint("close fail 0x%x\n", status);
	}

}

VOID TestRegistry()
{
	HANDLE my_key = NULL;
	NTSTATUS status;
	UNICODE_STRING my_key_path = RTL_CONSTANT_STRING(L"\\Registry\\Machine\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion");
	OBJECT_ATTRIBUTES my_obj_attr = { 0 };
	UNICODE_STRING my_key_name = RTL_CONSTANT_STRING(L"SystemRoot");
	KEY_VALUE_PARTIAL_INFORMATION key_infor;//用来试探大小的key_infor
	PKEY_VALUE_PARTIAL_INFORMATION ac_key_infor = NULL;
	ULONG ac_length;
	UNICODE_STRING system_root = { 0 };
	DbgPrint("====测试注册表操作 =====\n");
	do
	{
		InitializeObjectAttributes(&my_obj_attr, &my_key_path, OBJ_CASE_INSENSITIVE, NULL, NULL);
		status = ZwOpenKey(&my_key, KEY_READ, &my_obj_attr);
		if (!NT_SUCCESS(status))
		{
			DbgPrint("open fail. %x\n", status);
			break;
		}

		status = ZwQueryValueKey(my_key, &my_key_name, KeyValuePartialInformation, &key_infor, sizeof(KEY_VALUE_PARTIAL_INFORMATION), &ac_length);
		if (!NT_SUCCESS(status) && status != STATUS_BUFFER_OVERFLOW && status != STATUS_BUFFER_TOO_SMALL) 
		{ 
			DbgPrint("query fail. %x\n", status);
			break;
		}
		ac_key_infor = (PKEY_VALUE_PARTIAL_INFORMATION)ExAllocatePoolWithTag(NonPagedPool, ac_length, MEM_TAG);
		if (ac_key_infor == NULL) 
		{
			status = STATUS_INSUFFICIENT_RESOURCES;
			DbgPrint("allocate fail.\n");
		}
		status = ZwQueryValueKey(my_key, &my_key_name, KeyValuePartialInformation, ac_key_infor, ac_length, &ac_length);
		if (!NT_SUCCESS(status) || !ac_key_infor->Data)
		{
			DbgPrint("query fail. %x\n", status);
			break;
		}
		//数据在ac_key_infor->Data里
		RtlInitUnicodeString(&system_root, ac_key_infor->Data);
		Print(&system_root);
		DbgPrint("\n");

	} while (0);
	
	status = ZwClose(my_key);
	if (!NT_SUCCESS(status))
	{
		DbgPrint("close fail 0x%x", status);
	}
	if (ac_key_infor)
	{
		ExFreePool(ac_key_infor);
		ac_key_infor = NULL;
	}
}

// global variables

PDEVICE_OBJECT g_cdo = 0;
UNICODE_STRING g_device_name = RTL_CONSTANT_STRING(L"\\Device\\testdev");
UNICODE_STRING g_syblnk_name = RTL_CONSTANT_STRING(L"\\??\\testdev");

// 222000
#define ctl_800 \
	(ULONG)CTL_CODE(FILE_DEVICE_UNKNOWN, 0X800, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define ctl_800a \
	(ULONG)CTL_CODE(FILE_DEVICE_UNKNOWN, 0X800, METHOD_NEITHER, FILE_ANY_ACCESS)

// 0022E004
#define ctl_801 \
	(ULONG)CTL_CODE(FILE_DEVICE_UNKNOWN, 0X801, METHOD_BUFFERED, FILE_WRITE_ACCESS | FILE_READ_ACCESS)

// 0022A008
#define ctl_802 \
	(ULONG)CTL_CODE(FILE_DEVICE_UNKNOWN, 0X802, METHOD_BUFFERED, FILE_WRITE_ACCESS)

#define ctl_803 \
	(ULONG)CTL_CODE(FILE_DEVICE_UNKNOWN, 0X803, METHOD_BUFFERED, FILE_READ_ACCESS)

#define ctl_804 \
	(ULONG)CTL_CODE(FILE_DEVICE_UNKNOWN, 0X804, METHOD_BUFFERED, FILE_WRITE_ACCESS | FILE_READ_ACCESS)

#define ctl_805 \
	(ULONG)CTL_CODE(FILE_DEVICE_UNKNOWN, 0X805, METHOD_BUFFERED, FILE_WRITE_ACCESS | FILE_READ_ACCESS)

#define ctl_806 \
	(ULONG)CTL_CODE(FILE_DEVICE_UNKNOWN, 0X806, METHOD_BUFFERED, FILE_WRITE_ACCESS | FILE_READ_ACCESS)


NTSTATUS dispatch_func(IN PDEVICE_OBJECT dev, IN PIRP irp)
{
	NTSTATUS status = STATUS_INVALID_PARAMETER;
	PIO_STACK_LOCATION irpsp = IoGetCurrentIrpStackLocation(irp);
	ULONG ret_len = 0;
	
	do
	{
		if (dev != g_cdo)
			break;

		if (irpsp->MajorFunction == IRP_MJ_CREATE || irpsp->MajorFunction == IRP_MJ_CLOSE)
		{
			status = STATUS_SUCCESS;
			break;
		}
		
		if (irpsp->MajorFunction == IRP_MJ_DEVICE_CONTROL)
		{
			ULONG code = irpsp->Parameters.DeviceIoControl.IoControlCode;
			PVOID buffer = irp->AssociatedIrp.SystemBuffer;
			ULONG in_len = irpsp->Parameters.DeviceIoControl.InputBufferLength;
			ULONG out_len = irpsp->Parameters.DeviceIoControl.OutputBufferLength;
			ULONG len = 0;

			DbgPrint("code=%X, inlen=%d, oulen=%d\n", code, in_len, out_len);

			switch (code)
			{
			case ctl_800:
			case ctl_800a:
				status = STATUS_SUCCESS;
				break;
			case ctl_801:
				if (out_len == 4)
					status = STATUS_SUCCESS;
				break;
			case ctl_802:
				if (in_len == 512 && out_len == 4)
				{
					DbgPrint("ctl 802\n");
					status = STATUS_SUCCESS;
				}
				break;
			case ctl_803:
				if (in_len == 4 && out_len == 4)
					status = STATUS_SUCCESS;
				break;
			case ctl_804:
				if (in_len == 512)
					status = STATUS_SUCCESS;
				break;
			case ctl_805:
				if (out_len == 4)
					status = STATUS_SUCCESS;
				break;
			case ctl_806:
				if (out_len == 4)
					status = STATUS_SUCCESS;
				break;
			default:
				break;
			}
		}

	} while (0);
	irp->IoStatus.Information = ret_len;
	irp->IoStatus.Status = status;
	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return status;
}

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

NTSTATUS DriverEntry(PDRIVER_OBJECT driver, PUNICODE_STRING reg_path)
{
	NTSTATUS status = STATUS_SUCCESS;
	ULONG i = 0;
	UCHAR mem[256] = { 0 };

	HANDLE pid = PsGetCurrentProcessId();
	DbgPrint("starting...pid=%d\n", (USHORT)pid);
#if DBG
	_asm nop
#endif

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

	/*TestExAllocatePoolWithTag();
	TestUnicodeString();
	TestListEntry();
	TestLargeInteger();
	TestSpinLock();
	TestFileOperation();
	TestRegistry();
*/
	return STATUS_SUCCESS;
}
