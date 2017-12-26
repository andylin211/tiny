#include "ioctl_browser.h"
#include "ioctl.h"
#include "common.h"

Ioctl_Browser::Ioctl_Browser(int x, int y, int w, int h, const char *l )
	:Fl_Hold_Browser(x, y + 20, w, h-20, l)
{
	Fl_Box* box = new Fl_Box(FL_UP_BOX, x, y, w, 20, "");
	box->copy_label(label_conv("配置文件"));
	set_label_font_12(box);
	textfont(FL_COURIER);
	textsize(12);
	callback(&Ioctl_Browser::call_back, 0);
}

void Ioctl_Browser::call_back(Fl_Widget* o, long i)
{
	Ioctl_Browser* b = (Ioctl_Browser*)o;
	if (b->value())
	{
		const char* str = b->text(b->value());
		for (std::map<char*, std::vector<ioctl_line>*>::iterator i = b->map_line.begin(); i != b->map_line.end(); i++)
		{
			if (!strcmp(i->first, str))
			{
				b->win->update_table(i->second);
				break;
			}
		}
	}
}

wchar_t* type_str(wchar_t* str)
{
	static struct {
		wchar_t* v1;
		wchar_t* v2;
	} type_def[] =
	{
		{ L"FILE_DEVICE_UNKNOWN",L"Unknown" }, /* 基本上都是这个 */
		{ L"FILE_DEVICE_8042_PORT", L"8042 Port" },
		{ L"FILE_DEVICE_ACPI", L"Acpi" },
		{ L"FILE_DEVICE_BATTERY", L"Battery" },
		{ L"FILE_DEVICE_BEEP", L"Beep" },
		{ L"FILE_DEVICE_BUS_EXTENDER",L"Bus Extender" },
		{ L"FILE_DEVICE_CD_ROM",L"CD Rom" },
		{ L"FILE_DEVICE_CD_ROM_FILE_SYSTEM",L"CD Rom File System" },
		{ L"FILE_DEVICE_CHANGER",L"Changer" },
		{ L"FILE_DEVICE_CONTROLLER",L"Controller" },
		{ L"FILE_DEVICE_DATALINK",L"Data Link" },
		{ L"FILE_DEVICE_DFS",L"DFS" },
		{ L"FILE_DEVICE_DFS_FILE_SYSTEM",L"DFS File System" },
		{ L"FILE_DEVICE_DFS_VOLUME",L"DFS Volume" },
		{ L"FILE_DEVICE_DISK",L"Disk" },
		{ L"FILE_DEVICE_DISK_FILE_SYSTEM",L"Disk File System" },
		{ L"FILE_DEVICE_DVD",L"DVD" },
		{ L"FILE_DEVICE_FILE_SYSTEM",L"File System" },
		{ L"FILE_DEVICE_FIPS",L"FIPS" },
		{ L"FILE_DEVICE_FULLSCREEN_VIDEO",L"Fullscreen Video" },
		{ L"FILE_DEVICE_INPORT_PORT",L"Inport Port" },
		{ L"FILE_DEVICE_KEYBOARD",L"Keyboard" },
		{ L"FILE_DEVICE_KS",L"KS" },
		{ L"FILE_DEVICE_KSEC",L"KSEC" },
		{ L"FILE_DEVICE_MAILSLOT",L"Mailslot" },
		{ L"FILE_DEVICE_MASS_STORAGE",L"Mass Storage" },
		{ L"FILE_DEVICE_MIDI_IN",L"MIDI In" },
		{ L"FILE_DEVICE_MIDI_OUT",L"MIDI Out" },
		{ L"FILE_DEVICE_MODEM",L"Modem" },
		{ L"FILE_DEVICE_MOUSE",L"Mouse" },
		{ L"FILE_DEVICE_MULTI_UNC_PROVIDER ",L"Multi Unc Provider" },
		{ L"FILE_DEVICE_NAMED_PIPE",L"Named Pipe" },
		{ L"FILE_DEVICE_NETWORK",L"Network" },
		{ L"FILE_DEVICE_NETWORK_BROWSER",L"Network Browser" },
		{ L"FILE_DEVICE_NETWORK_FILE_SYSTEM",L"Network File System" },
		{ L"FILE_DEVICE_NETWORK_REDIRECTOR ",L"Network Redirector" },
		{ L"FILE_DEVICE_NULL",L"Null" },
		{ L"FILE_DEVICE_PARALLEL_PORT",L"Parallel Port" },
		{ L"FILE_DEVICE_PHYSICAL_NETCARD",L"Physical Netcard" },
		{ L"FILE_DEVICE_PRINTER",L"Printer" },
		{ L"FILE_DEVICE_SCANNER",L"Scanner" },
		{ L"FILE_DEVICE_SCREEN",L"Screen" },
		{ L"FILE_DEVICE_SERENUM",L"Serenum" },
		{ L"FILE_DEVICE_SERIAL_MOUSE_PORT",L"Serial Mouse Port" },
		{ L"FILE_DEVICE_SERIAL_PORT",L"Serial Port" },
		{ L"FILE_DEVICE_SMARTCARD",L"Smartcard" },
		{ L"FILE_DEVICE_SMB",L"SMB" },
		{ L"FILE_DEVICE_SOUND",L"Sound" },
		{ L"FILE_DEVICE_STREAMS",L"Streams" },
		{ L"FILE_DEVICE_TAPE",L"Tape" },
		{ L"FILE_DEVICE_TAPE_FILE_SYSTEM",L"Tape File System" },
		{ L"FILE_DEVICE_TERMSRV",L"Termsrv" },
		{ L"FILE_DEVICE_TRANSPORT",L"Transport" },
		{ L"FILE_DEVICE_VDM",L"VDM" },
		{ L"FILE_DEVICE_VIDEO",L"Video" },
		{ L"FILE_DEVICE_VIRTUAL_DISK",L"Virtual Disk" },
		{ L"FILE_DEVICE_WAVE_IN",L"Wave In" },
		{ L"FILE_DEVICE_WAVE_OUT",L"Wave Out" },
	};
	if (str)
		for (int i = 0; i < sizeof(type_def) / sizeof(type_def[0]); i++)
		{
			if (0 == wcscmp(type_def[i].v1, str))
				return type_def[i].v2;
		}

	return 0;
}

wchar_t* access_str(wchar_t* str)
{
	if (!str)
		return 0;

	if (!wcscmp(L"FILE_READ_ACCESS", str))
		return L"Read";

	if (!wcscmp(L"FILE_WRITE_ACCESS", str))
		return L"Write";

	if (!wcscmp(L"FILE_ANY_ACCESS", str))
		return L"Any";

	wchar_t buf[buf_len];
	wcsncpy(buf, str, buf_len);
	wcs_remove(buf, L' ');
	if (!wcscmp(L"FILE_READ_ACCESS|FILE_WRITE_ACCESS", buf))
		return L"读写";

	return 0;
}

wchar_t* method_str(wchar_t* str)
{
	if (!str)
		return 0;

	if (!wcscmp(L"METHOD_BUFFERED", str))
		return L"Buffered";

	if (!wcscmp(L"METHOD_IN_DIRECT", str))
		return L"In Direct";

	if (!wcscmp(L"METHOD_OUT_DIRECT", str))
		return L"Out Direct";

	if (!wcscmp(L"METHOD_NEITHER", str))
		return L"Neither";

	return 0;
}

void Ioctl_Browser::free_ioctl_line(ioctl_line* li)
{
	if (!li)
		return;

	free(li->device);
	free(li->code);
	free(li->type);
	free(li->func);
	free(li->method);
	free(li->access);
}

void Ioctl_Browser::free_map_line(std::vector<ioctl_line>* line)
{
	for (int i = 0; i < (int)line->size(); i++)
	{
		ioctl_line& li = line->at(i);
		free_ioctl_line(&li);
	}
	line->clear();
	delete line;
}

void Ioctl_Browser::add_map_line(wchar_t* file, std::vector<ioctl_line>* line)
{
	char* buf = wcs_to_str(file, -1, encoding_utf8);
	for (std::map<char*, std::vector<ioctl_line>*>::iterator i = map_line.begin(); i != map_line.end(); i++)
	{
		if (!strcmp(i->first, buf))
		{
			free_map_line(i->second);
			free(i->first);
			map_line.erase(i);
			break;
		}
	}
	map_line.insert(std::pair<char*, std::vector<ioctl_line>*>(buf, line));

	win->update_table(line);
	update_(buf);
}

void Ioctl_Browser::add_(wchar_t* file, xml_document* doc)
{
	if (!file || !doc)
		return;

	if (!doc)
		return;

	if (!doc->root_element)
		return;

	if (strcmp(doc->root_element->name, "ioctl"))
		return;

	std::vector<ioctl_line>* v_line = new std::vector<ioctl_line>();

	if (!is_list_empty(&doc->root_element->element_list))
	{
		for (list_entry_t* i = doc->root_element->element_list.flink; i != &doc->root_element->element_list; i = i->flink)
		{
			xml_element* ele_device = container_of(i, xml_element, list_entry);
			wchar_t* device = xml_query_attribute(ele_device, "name");
			if (device && !is_list_empty(&ele_device->element_list))
			{
				for (list_entry_t* j = ele_device->element_list.flink; j != &ele_device->element_list; j = j->flink)
				{
					xml_element* ele = container_of(j, xml_element, list_entry);
					wchar_t* code = xml_query_attribute(ele, "code");
					wchar_t* type = xml_query_attribute(ele, "type");
					wchar_t* function = xml_query_attribute(ele, "function");
					wchar_t* method = xml_query_attribute(ele, "method");
					wchar_t* access = xml_query_attribute(ele, "access");

					ioctl_line line;
					line.device = wcs_to_str(device, -1, encoding_utf8);
					line.code = wcs_to_str(code, -1, encoding_utf8);
					line.type = wcs_to_str(type_str(type), -1, encoding_utf8);
					line.func = wcs_to_str(function, -1, encoding_utf8);
					line.method = wcs_to_str(method_str(method), -1, encoding_utf8);
					line.access = wcs_to_str(access_str(access), -1, encoding_utf8);
					v_line->push_back(line);
				}
			}
		}
	}
	
	add_map_line(file, v_line);	
}

void Ioctl_Browser::update_(char* str)
{
	Fl::lock();
	clear();
	int index = 1;
	for (std::map<char*, std::vector<ioctl_line>*>::iterator i = map_line.begin(); i != map_line.end(); i++)
	{
		add(i->first);
		if (!strcmp(str, i->first))
			select(index);
		index++;
	}
	Fl::unlock();
}

void Ioctl_Browser::close_()
{
	Fl::lock();
	if (size() && value())
	{
		const char* buf = text(value());
		for (std::map<char*, std::vector<ioctl_line>*>::iterator i = map_line.begin(); i != map_line.end(); i++)
		{
			if (!strcmp(i->first, buf))
			{
				free_map_line(i->second);
				free(i->first);
				map_line.erase(i);
				break;
			}
		}
		remove(value());
		if (size())
		{
			select(1);
			win->update_table(map_line.begin()->second);
		}
		else
			win->update_table(0);
	}
	Fl::unlock();
}