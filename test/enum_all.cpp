#include <Windows.h>
#include <string>
#pragma comment(lib, "ntdll.lib")
#include "ntdll.h"
using namespace NT;

void enum_all_device()
{
	wchar_t* root = L"\\Device";
	long status = 0;
	UNICODE_STRING name;
	OBJECT_ATTRIBUTES attr;
	HANDLE hDirectory;
	BOOLEAN firstEntry = TRUE;
#define _max_device_count 256
	OBJECT_DIRECTORY_INFORMATION buffer[_max_device_count];
	int i = 0;
	int start = 0;
	ULONG index = 0, bytes = 0;

	int j = 0;
	do
	{
		//v_dev.clear();

		RtlInitUnicodeString(&name, root);
		
		InitializeObjectAttributes(&attr, &name, 0, nullptr, nullptr);

		status = NtOpenDirectoryObject(&hDirectory, DIRECTORY_QUERY | DIRECTORY_TRAVERSE, &attr);

		if (status < 0)
			break;

		status = NtQueryDirectoryObject(hDirectory, buffer, _max_device_count, FALSE, firstEntry, &index, &bytes);

		if (status < 0)
			break;

		for (i = 0; i < (int)index - start; i++)
		{
			printf("%d %d====%S\n", j, i, buffer[i].Name.Buffer);
			j++;
		}

		if (status == 0)
			break;
		
		// more entries (STATUS_NEED_MORE_ENTRIES)
		start = index;
		firstEntry = FALSE;
	} while (1);
	 

	CloseHandle(hDirectory);

}

int main()
{
	enum_all_device();
	return 0;
}