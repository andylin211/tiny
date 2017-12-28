#include "ntddk.h"  

typedef struct _LDR_DATA_TABLE_ENTRY {
	LIST_ENTRY InLoadOrderLinks;//�����Ա��ϵͳ���м���(������ֹͣû��ж��)�Ѿ���ȡ���ڴ��� ���ǹ�ϵ��һ��  ����Ҫ�������� ˫���� �����м��ĸ��ڵ㶼���Ա����������� �������������������һ���ڵ�  
	LIST_ENTRY InMemoryOrderLinks;//ϵͳ�Ѿ����� û�б���ʼ�� û�е���DriverEntry������̵�ʱ�� ͨ�����������̴�������  
	LIST_ENTRY InInitializationOrderLinks;//�Ѿ�����DriverEntry���������������������  
	PVOID DllBase;
	PVOID EntryPoint;//�����Ľ���� DriverEntry  
	ULONG SizeOfImage;
	UNICODE_STRING FullDllName;//��������·��  
	UNICODE_STRING BaseDllName;//����·������������  
	ULONG Flags;
	USHORT LoadCount;
	USHORT TlsIndex;
	union {
		LIST_ENTRY HashLinks;
		struct {
			PVOID SectionPointer;
			ULONG CheckSum;
		};
	};
	union {
		struct {
			ULONG TimeDateStamp;
		};
		struct {
			PVOID LoadedImports;
		};
	};
} LDR_DATA_TABLE_ENTRY, *PLDR_DATA_TABLE_ENTRY;//ϵͳ���г��� ���������ﶼ������ṹ ����������ĳ�Աqudongduixiang1->DriverSection(PVOID DriverSection) ����ṹ����ʲôʱ���е� io�������ڼ�������������ʱ�� ����DriverEntry������̵�ʱ�� ��������������Ҳ���뵽ϵͳ��һ��ȫ��������   
											   //����Ϊ�˷���io����������ά�����߷���������������ά�� Ϊ�˲��ҷ��� ���ȫ�������� ��ϵͳ����������������������������  

VOID xiezai1(PDRIVER_OBJECT qudongduixiang1)
{
	KdPrint(("����ж������\n"));
}

extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT qudongduixiang1, PUNICODE_STRING zhucebiao1)//������������  
{
	LDR_DATA_TABLE_ENTRY *jiegouti1, *linshi1;
	jiegouti1 = (LDR_DATA_TABLE_ENTRY*)qudongduixiang1->DriverSection;
	PLIST_ENTRY shuangxiangxunhuanlianbiao;
	shuangxiangxunhuanlianbiao = jiegouti1->InLoadOrderLinks.Flink;  //PLIST_ENTRY ˫ѭ������ ѭ�������� ��β����ӵ� //Flink ����ǰһ���ڵ�  
	while (shuangxiangxunhuanlianbiao != &jiegouti1->InLoadOrderLinks)
	{
		linshi1 = (LDR_DATA_TABLE_ENTRY*)shuangxiangxunhuanlianbiao;//˫��ѭ�������ŵĽṹ�����LDR_DATA_TABLE_ENTRY* ����ں����������  
		KdPrint(("%wZ -- %wZ\n", &linshi1->BaseDllName, &linshi1->FullDllName));//ע���ȡ��ַ  
		shuangxiangxunhuanlianbiao = shuangxiangxunhuanlianbiao->Flink;
	}
	qudongduixiang1->DriverUnload = xiezai1;
	return STATUS_SUCCESS;
}