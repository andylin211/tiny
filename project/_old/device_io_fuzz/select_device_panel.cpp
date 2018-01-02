#include "select_device_panel.h"
#include "ntdll.h"
#include "tinystr.h"
#include <algorithm>
using namespace NT;
#pragma comment(lib, "ntdll.lib")

select_device_panel::select_device_panel(int x, int y, int w, int h)
{
	box_outline = new Fl_Box(x, y + 20, w, h - 20);
	set_label_font(box_outline);
	box_outline->copy_label(label_conv("Ñ¡ÔñÉè±¸"));
	box_outline->align(FL_ALIGN_TOP);
	box_outline->box(FL_GTK_UP_BOX);

	int p = 5;
	browser = new Fl_Hold_Browser(x + p, y + 20 + p, w - 2 * p, h - 2 * p - 20);
	set_text_font(browser);
	add_all_device();

	v_buf.clear();
}

select_device_panel::~select_device_panel()
{
	free_v_buf();
}

void select_device_panel::free_v_buf()
{
	for (std::vector<char*>::iterator i = v_buf.begin(); i != v_buf.end(); i++)
	{
		char* str = *i;
		if (str)
			free(str);
	}
	v_buf.clear();
}

bool cmp_4_sort(char* v1, char* v2)
{
	return (strcmp(v1, v2) < 0);
}

const int _max_device_count = 256;

void select_device_panel::add_all_device()
{
	HANDLE hDirectory = NULL;
	OBJECT_DIRECTORY_INFORMATION buffer[_max_device_count];
	int start = 0;
	BOOLEAN firstEntry = TRUE;

	free_v_buf();

	UNICODE_STRING name;
	RtlInitUnicodeString(&name, L"\\Device");

	OBJECT_ATTRIBUTES attr;
	InitializeObjectAttributes(&attr, &name, 0, nullptr, nullptr);

	NT::NTSTATUS status = NtOpenDirectoryObject(&hDirectory, DIRECTORY_QUERY | DIRECTORY_TRAVERSE, &attr);

	if (status < 0)
		return;

	ULONG index = 0, bytes = 0;
	do
	{
		status = NtQueryDirectoryObject(hDirectory, buffer, _max_device_count, FALSE, firstEntry, &index, &bytes);

		if (status < 0)
			break;

		for (int i = 0; i < (int)index - start; i++)
		{
			if (buffer[i].Name.Buffer[0] != L'{' && buffer[i].Name.Buffer[0] != L'0' && !wcscmp(buffer[i].TypeName.Buffer, L"Device"))
			{
				char* str = wcs_to_str(buffer[i].Name.Buffer, -1, encoding_utf8);
				v_buf.push_back(str);
			}
		}

		if (status == 0)
			break;

		/* more entries (STATUS_NEED_MORE_ENTRIES)*/
		start = index;
		firstEntry = FALSE;

	} while (1);

	std::sort(v_buf.begin(), v_buf.end(), cmp_4_sort);
	for (std::vector<char*>::iterator i = v_buf.begin(); i != v_buf.end(); i++)
		browser->add(*i);

	CloseHandle(hDirectory);
}