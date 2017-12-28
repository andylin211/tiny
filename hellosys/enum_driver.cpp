#include "ntddk.h"  

typedef struct _LDR_DATA_TABLE_ENTRY {
	LIST_ENTRY InLoadOrderLinks;//这个成员把系统所有加载(可能是停止没被卸载)已经读取到内存中 我们关系第一个  我们要遍历链表 双链表 不管中间哪个节点都可以遍历整个链表 本驱动的驱动对象就是一个节点  
	LIST_ENTRY InMemoryOrderLinks;//系统已经启动 没有被初始化 没有调用DriverEntry这个历程的时候 通过这个链表进程串接起来  
	LIST_ENTRY InInitializationOrderLinks;//已经调用DriverEntry这个函数的所有驱动程序  
	PVOID DllBase;
	PVOID EntryPoint;//驱动的进入点 DriverEntry  
	ULONG SizeOfImage;
	UNICODE_STRING FullDllName;//驱动的满路径  
	UNICODE_STRING BaseDllName;//不带路径的驱动名字  
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
} LDR_DATA_TABLE_ENTRY, *PLDR_DATA_TABLE_ENTRY;//系统所有程序 驱动对象里都有这个结构 是驱动对象的成员qudongduixiang1->DriverSection(PVOID DriverSection) 这个结构体在什么时候有的 io管理器在加载我们驱动的时候 调用DriverEntry这个历程的时候 把我们驱动对象也加入到系统的一个全局链表中   
											   //就是为了方便io管理器进行维护或者方便对象管理器进行维护 为了查找方便 这个全局链表呢 把系统所有驱动程序串起来就是连接起来  

VOID xiezai1(PDRIVER_OBJECT qudongduixiang1)
{
	KdPrint(("驱动卸载历程\n"));
}

extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT qudongduixiang1, PUNICODE_STRING zhucebiao1)//遍历驱动名字  
{
	LDR_DATA_TABLE_ENTRY *jiegouti1, *linshi1;
	jiegouti1 = (LDR_DATA_TABLE_ENTRY*)qudongduixiang1->DriverSection;
	PLIST_ENTRY shuangxiangxunhuanlianbiao;
	shuangxiangxunhuanlianbiao = jiegouti1->InLoadOrderLinks.Flink;  //PLIST_ENTRY 双循环链表 循环起来的 首尾是相接的 //Flink 代表前一个节点  
	while (shuangxiangxunhuanlianbiao != &jiegouti1->InLoadOrderLinks)
	{
		linshi1 = (LDR_DATA_TABLE_ENTRY*)shuangxiangxunhuanlianbiao;//双向循环链表存放的结构体就是LDR_DATA_TABLE_ENTRY* 这个内核链表的特性  
		KdPrint(("%wZ -- %wZ\n", &linshi1->BaseDllName, &linshi1->FullDllName));//注意加取地址  
		shuangxiangxunhuanlianbiao = shuangxiangxunhuanlianbiao->Flink;
	}
	qudongduixiang1->DriverUnload = xiezai1;
	return STATUS_SUCCESS;
}