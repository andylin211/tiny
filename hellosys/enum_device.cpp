#include <ntddk.h>
#include <Windows.h>

NTKERNELAPI
	NTSTATUS
	ObReferenceObjectByName(
		IN PUNICODE_STRING ObjectName,
		IN ULONG Attributes,
		IN PACCESS_STATE PassedAccessState OPTIONAL,
		IN ACCESS_MASK DesiredAccess OPTIONAL,
		IN POBJECT_TYPE ObjectType,
		IN KPROCESSOR_MODE AccessMode,
		IN OUT PVOID ParseContext OPTIONAL,
		OUT PVOID *Object
	);
NTKERNELAPI
	PDEVICE_OBJECT
	NTAPI
	IoGetBaseFileSystemDeviceObject(
		IN PFILE_OBJECT FileObject
	);
extern POBJECT_TYPE IoDeviceObjectType;
extern POBJECT_TYPE *IoDriverObjectType;

typedef struct _OBJECT_CREATE_INFORMATION
{
	ULONG Attributes;
	HANDLE RootDirectory;
	PVOID ParseContext;
	KPROCESSOR_MODE ProbeMode;
	ULONG PagedPoolCharge;
	ULONG NonPagedPoolCharge;
	ULONG SecurityDescriptorCharge;
	PSECURITY_DESCRIPTOR SecurityDescriptor;
	PSECURITY_QUALITY_OF_SERVICE SecurityQos;
	SECURITY_QUALITY_OF_SERVICE SecurityQualityOfService;
} OBJECT_CREATE_INFORMATION, *POBJECT_CREATE_INFORMATION;

typedef struct _OBJECT_HEADER
{
	LONG PointerCount;
	union
	{
		LONG HandleCount;
		PSINGLE_LIST_ENTRY SEntry;
	};
	POBJECT_TYPE Type;
	UCHAR NameInfoOffset;
	UCHAR HandleInfoOffset;
	UCHAR QuotaInfoOffset;
	UCHAR Flags;
	union
	{
		POBJECT_CREATE_INFORMATION ObjectCreateInfo;
		PVOID QuotaBlockCharged;
	};

	PSECURITY_DESCRIPTOR SecurityDescriptor;
	QUAD Body;
} OBJECT_HEADER, *POBJECT_HEADER;

#define NUMBER_HASH_BUCKETS 37

typedef struct _OBJECT_DIRECTORY
{
	struct _OBJECT_DIRECTORY_ENTRY* HashBuckets[NUMBER_HASH_BUCKETS];
	struct _OBJECT_DIRECTORY_ENTRY** LookupBucket;
	BOOLEAN LookupFound;
	USHORT SymbolicLinkUsageCount;
	struct _DEVICE_MAP* DeviceMap;
} OBJECT_DIRECTORY, *POBJECT_DIRECTORY;

typedef struct _OBJECT_HEADER_NAME_INFO
{
	POBJECT_DIRECTORY Directory;
	UNICODE_STRING Name;
	ULONG Reserved;
#if DBG
	ULONG Reserved2;
	LONG DbgDereferenceCount;
#endif
} OBJECT_HEADER_NAME_INFO, *POBJECT_HEADER_NAME_INFO;

#define OBJECT_TO_OBJECT_HEADER( o ) \
	CONTAINING_RECORD( (o), OBJECT_HEADER, Body )

#define OBJECT_HEADER_TO_NAME_INFO( oh ) ((POBJECT_HEADER_NAME_INFO) \
    ((oh)->NameInfoOffset == 0 ? NULL : ((PCHAR)(oh) - (oh)->NameInfoOffset)))


//===========================================================================
//��ȡĳ���豸������Ϣ
//===========================================================================
VOID GetDeviceObjectInfo(PDEVICE_OBJECT pDeviceObj) {
	POBJECT_HEADER ObjHeader = NULL;
	POBJECT_HEADER_NAME_INFO ObjName = NULL;

	//�õ�����ͷ
	ObjHeader = OBJECT_TO_OBJECT_HEADER(pDeviceObj);
	ASSERT(pDeviceObj != NULL);

	if (ObjHeader) {

		//��ѯ�豸����
		ObjName = OBJECT_HEADER_TO_NAME_INFO(ObjHeader);

		if (ObjName && ObjName->Name.Buffer) {

			KdPrint(("��������:%wZ �豸����:%wZ ������ַ:%p �豸��ַ:%p\n",
				&pDeviceObj->DriverObject->DriverName,
				&ObjName->Name, pDeviceObj->DriverObject, pDeviceObj
				));
		}
		//---------------------------------------------------------------------------
		//����û�����Ƶ��豸��ӡNULL
		//---------------------------------------------------------------------------
	}
	else if (pDeviceObj->DriverObject) {

		KdPrint(("��������:%wZ �豸����:%wZ ������ַ:%p �豸��ַ:%p\n",
			&pDeviceObj->DriverObject->DriverName,
			NULL, pDeviceObj->DriverObject, pDeviceObj));
	}
	return;
}

//===========================================================================
//��ȡ�����豸ջ�ϵ��豸��Ϣ
//===========================================================================
VOID GetAttachedDeviceObjectInfo(PDEVICE_OBJECT pDeviceObj) {
	PDEVICE_OBJECT pAttrDeviceObj = NULL;

	ASSERT(pDeviceObj != NULL);

	//��ȡ���豸�ĵ�ַ
	pAttrDeviceObj = pDeviceObj->AttachedDevice;

	while (pAttrDeviceObj) {
		KdPrint(("\t����������:%wZ  ��������ַ:%p ���豸��ַ:%p\n",
			&pAttrDeviceObj->DriverObject->DriverName,
			pAttrDeviceObj->DriverObject, pAttrDeviceObj));

		//���ϱ���
		pAttrDeviceObj = pAttrDeviceObj->AttachedDevice;
	}
}
//===========================================================================
//ö���豸ջ
//===========================================================================
NTSTATUS EnumDeviceStack(PUNICODE_STRING pUSzDriverName) {
	NTSTATUS Status;
	PDEVICE_OBJECT pDeviceObj = NULL;
	PDRIVER_OBJECT pDriverObj = NULL;

	//ͨ�������������ƻ�ȡ��������ָ��
	Status = ObReferenceObjectByName(pUSzDriverName, OBJ_CASE_INSENSITIVE, NULL, 0,
		*IoDriverObjectType, KernelMode, NULL, (PVOID*)&pDriverObj);
	if (!NT_SUCCESS(Status) || !pDriverObj) {
		return STATUS_UNSUCCESSFUL;
	}

	//ͨ����������õ����豸����ָ��
	pDeviceObj = pDriverObj->DeviceObject;

	__try {

		while (pDeviceObj) {

			//��ȡ�豸��Ϣ
			GetDeviceObjectInfo(pDeviceObj);

			//������а�����֮�ϵ��豸
			if (pDeviceObj->AttachedDevice) {
				//��ȡ�����豸֮�ϵĵ��豸��Ϣ
				GetAttachedDeviceObjectInfo(pDeviceObj);

			}

			//��һ���жϵ�ǰ�豸�ϵ�VPB�е��豸
			if (pDeviceObj->Vpb && pDeviceObj->Vpb->DeviceObject) {

				//��ȡ�豸������Ϣ
				GetDeviceObjectInfo(pDeviceObj->Vpb->DeviceObject);

				//������а��豸
				if (pDeviceObj->Vpb->DeviceObject->AttachedDevice) {
					//��ȡ���豸��Ϣ
					GetAttachedDeviceObjectInfo(pDeviceObj->Vpb->DeviceObject);
				}

			}
			pDeviceObj = pDeviceObj->NextDevice;
		}

	}
	__finally {
		if (pDriverObj) {
			ObDereferenceObject(pDriverObj);
		}

	}
	return Status;
}

extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObj, PUNICODE_STRING pUSzRegPath) {
	NTSTATUS Status;
	UNICODE_STRING USzDriverName = RTL_CONSTANT_STRING(L"\\Driver\\ACPI");

	Status = EnumDeviceStack(&USzDriverName);
	if (!NT_SUCCESS(Status)) {
		KdPrint(("ö���豸ʧ��!\n"));
		return Status;
	}

	return STATUS_UNSUCCESSFUL;
}
typedef struct _OBJECT_DIRECTORY_ENTRY

{

	struct _OBJECT_DIRECTORY_ENTRY *NextEntry;

	PVOID Object;

}OBJECT_DIRECTORY_ENTRY, *POBJECT_DIRECTORY_ENTRY, **PPOBJECT_DIRECTORY_ENTRY;


typedef struct _OBJECT_DIRECTORY
{

	POBJECT_DIRECTORY_ENTRY HashTable[37];

	POBJECT_DIRECTORY_ENTRY CurrentEntry;

	BOOLEAN CurrentEntryValid;

	BYTE  Reserved1;

	WORD Reserved2;

	DWORD Reserved3;

}OBJECT_DIRECTORY, *POBJECT_DIRECTORY;

VOID init()

{

	UNICODE_STRING vDriverName = { 0 };

	RtlInitUnicodeString(&vDriverName, L"\\Driver");

	POBJECT_DIRECTORY vPObjectDirectory;

	NTSTATUS vStatus;

	vStatus = ObReferenceObjectByName(

		&vDriverName,

		OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,

		NULL,

		0,

		(POBJECT_TYPE)0x869d0040,

		KernelMode,

		NULL,

		(PVOID *)&vPObjectDirectory

	);

	if (NT_SUCCESS(vStatus))

	{

		KdPrint(("�ɹ�"));

		for (ULONG vI = 0; vI < NUMBER_HASH_BUCKETS; vI++)

		{

			PDRIVER_OBJECT vDriverObject = NULL;

			POBJECT_DIRECTORY_ENTRY vObjectDirectoryEntry = NULL;

			vObjectDirectoryEntry = vPObjectDirectory->HashTable[vI];

			while (vObjectDirectoryEntry && MmIsAddressValid((PVOID)vObjectDirectoryEntry))

			{

				vDriverObject = (PDRIVER_OBJECT)vObjectDirectoryEntry->Object;

				if (MmIsAddressValid((PVOID)vDriverObject))

				{

					if (MmIsAddressValid(&vDriverObject->DriverName))

					{

						if (wcsstr(vDriverObject->DriverName.Buffer, L"PCHunter32") != NULL)

						{

							KdPrint(("\r\n\r\n�ҵ���\r\n\r\n"));

							RtlInitUnicodeString(&vDriverObject->DriverName, L"\Driver\text32");

						}

						KdPrint(("��:%wZ\r\n", &vDriverObject->DriverName));

					}

				}

				vObjectDirectoryEntry = (vObjectDirectoryEntry->NextEntry);

			}

		}

		ObDereferenceObject(vPObjectDirectory);

	}

	else

		KdPrint(("vStatus = 0x%08X\r\n", vStatus));

	RtlInitUnicodeString(&vDriverName, L"\\FileSystem");

	vStatus = ObReferenceObjectByName(

		&vDriverName,

		OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,

		NULL,

		0,

		(POBJECT_TYPE)0x869d0040,

		KernelMode,

		NULL,

		(PVOID *)&vPObjectDirectory

	);

	if (NT_SUCCESS(vStatus))

	{

		KdPrint(("�ɹ�"));

		for (ULONG vI = 0; vI < NUMBER_HASH_BUCKETS; vI++)

		{

			PDRIVER_OBJECT vDriverObject = NULL;

			POBJECT_DIRECTORY_ENTRY vObjectDirectoryEntry = NULL;

			vObjectDirectoryEntry = vPObjectDirectory->HashTable[vI];

			while (vObjectDirectoryEntry && MmIsAddressValid((PVOID)vObjectDirectoryEntry))

			{

				vDriverObject = (PDRIVER_OBJECT)vObjectDirectoryEntry->Object;

				if (MmIsAddressValid((PVOID)vDriverObject))

				{

					if (MmIsAddressValid(&vDriverObject->DriverName))

					{

						KdPrint(("��:%wZ\r\n", &vDriverObject->DriverName));

					}

				}

				vObjectDirectoryEntry = vObjectDirectoryEntry->NextEntry;

			}

		}

		ObDereferenceObject(vPObjectDirectory);

	}

	else

		KdPrint(("vStatus = 0x%08X\r\n", vStatus));

}