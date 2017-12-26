#include "ioctl_fuzz.h"
#include <Windows.h>
#include <Shlwapi.h>
#include "tinystr.h"
#include "tinyxml.h"
#include "getrand.h"
#include "tinyargs.h"
#include "tinylog.h"

#define max_buf_len 512

#define max_overflow_buf 10240

char inbuf[max_overflow_buf];

char oubuf[max_overflow_buf];

#define fuzz_sleep_time 0

#define test_random_count 0x1000

#define test_predefined_count 0x1000

#define update_tick 50

#define update_progress_tick 1000

HANDLE open_device(wchar_t* device_name)
{
	wchar_t symlnk_name[max_buf_len];

	if (!device_name)
		return INVALID_HANDLE_VALUE;

	swprintf(symlnk_name, max_buf_len, L"\\\\.\\%s", device_name);
	return CreateFile(
		symlnk_name,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_SYSTEM,
		NULL);
}

char* load_file(wchar_t* file)
{
	FILE* fp = 0;
	char* buffer = 0;
	int len = 0;

	if (!file)
		return 0;

	fp = _wfopen(file, L"rb");
	if (!fp)
		return 0;

	fseek(fp, 0, SEEK_END);
	len = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	buffer = (char*)malloc(len + 1);
	memset(buffer, 0, len + 1);
	fread(buffer, len, 1, fp);
	fclose(fp);

	return buffer;
}

xml_document* load_config(wchar_t* file)
{
	xml_document* doc = 0;
	char* buffer = 0;

	buffer = load_file(file);
	xml_load_document(buffer, encoding_ansi, &doc);
	free(buffer);

	return doc;
}

void save_config(xml_document* doc, wchar_t* file)
{
	FILE* fp = 0;

	if (!file || !doc)
		return;

	fp = _wfopen(file, L"wb");
	if (!fp)
		return;

	xml_write_document(fp, encoding_ansi, doc);
	fclose(fp);
}

unsigned long make_ioctl_code(unsigned long device_type, unsigned long function, unsigned long method, unsigned long access)
{
	return (((device_type) << 16) | ((access) << 14) | ((function) << 2) | (method));
}

#define file_max_access 4

wchar_t* access_wcs(int i)
{
	static wchar_t* str[] = { L"FILE_ANY_ACCESS", L"FILE_READ_ACCESS", L"FILE_WRITE_ACCESS", L"FILE_READ_ACCESS | FILE_WRITE_ACCESS" };
	return i < file_max_access ? str[i] : L"";
}

#define inbuf_len_max 3

int inbuf_len[inbuf_len_max] = { 0,4,256 };

#define notify_progress(current, total) do { \
		if (notify && notify_data) { \
			args->p2 = 0; \
			args->p1 = wcs_format(buf, max_buf_len, L"进度: %d %%", current * 100 / total); \
			notify(current,total,notify_data); \
		} \
		tinylog(wcs_format(buf, max_buf_len, L"进度: %d %%", current * 100 / total)); \
	} while (0)

#define notify_log(msg) do { \
		if (notify && notify_data && msg) { \
			args->p2 = 0; \
			args->p1 = msg; \
			notify(0,0,notify_data); \
		} \
		tinylog(msg); \
	} while (0)

#define notify_log_dot() do {	if (args) { args->p1 = 0; args->p2 = (void*)1;  notify(0,0,notify_data); } tinylog_dot();\
	} while (0)

#define notify_error(msg) do { \
		if (notify && notify_data && msg) { \
			args->p2 = 0; \
			args->p1 = msg; \
			notify(100,100,notify_data); \
		} \
		tinylog(msg); \
	} while (0)

void scan_in_range(wchar_t* device, int from, int to, wchar_t* output, fuzz_notify_func notify, args_t* notify_data)
{
	int i = 0, j = 0, k = 0;
	unsigned long ioctl_code = 0;
	list_entry_t* list_entry = 0;
	xml_element* ele = 0;
	xml_element* ele_device = 0;
	xml_element* ele_ioctl = 0;
	xml_document* doc = 0;	
	HANDLE handle = INVALID_HANDLE_VALUE;
	int status = 0;
	int retlen = 0;
	args_t* args = (args_t*)notify_data;
	char* buffer = 0;
	wchar_t* large_msg = 0;
	wchar_t buf[max_buf_len];

	notify_log(L">>>>>>");
	notify_log(wcs_format(buf, max_buf_len, L"scan \"%s\" (0x%x ~ 0x%x)", device, from, to));	

	handle = open_device(device);
	if (handle == INVALID_HANDLE_VALUE)
	{
		notify_error(wcs_format(buf, max_buf_len, L"打开设备错误 \"%s\", 错误码: %d", device, GetLastError()));
		return;
	}
	else
	{
		notify_log(wcs_format(buf, max_buf_len, L"打开设备成功 \"%s\"", device));
	}

	doc = load_config(output);
	if (!doc)
	{
		notify_log(wcs_format(buf, max_buf_len, L"\"%s\" 不存在, 新建", output));
		doc = xml_new_document("ioctl");
	}
	else
	{
		notify_log(wcs_format(buf, max_buf_len, L"\"%s\" 已存在, 读取", output));
	}
	
	if (!is_list_empty(&doc->root_element->element_list))
	{
		for (list_entry = doc->root_element->element_list.flink; list_entry != &doc->root_element->element_list; list_entry = list_entry->flink)
		{
			ele = container_of(list_entry, xml_element, list_entry);
			if (0 == wcscmp(xml_query_attribute(ele, "name"), device))
			{
				ele_device = ele;
				break;
			}
		}
	}

	if (!ele_device)
	{
		ele_device = xml_new_element("device");
		xml_set_attribute(ele_device, "name", device);

		insert_into_list(&doc->root_element->element_list, &ele_device->list_entry);
		notify_log(L"添加新设备配置");
	}
	else
	{
		notify_log(L"已有该设备配置, 更新");
	}

	memset(inbuf, 0, max_overflow_buf);	

	/* each function */
	for (i = from; i <= to; i++)
	{
		notify_progress(i-from, to-from);
		/* each access */
		for (k = 0; k < file_max_access; k++)
		{
			ioctl_code = make_ioctl_code(FILE_DEVICE_UNKNOWN, i, METHOD_BUFFERED, k);
			notify_log(wcs_format(buf, max_buf_len, L"扫描 ioctl code 0x%x...", ioctl_code));

			/* each length */
			for (j = 0; j < inbuf_len_max; j++)
			{				
				status = DeviceIoControl(handle,
					ioctl_code,
					inbuf,
					inbuf_len[j],
					oubuf,
					inbuf_len[j],
					&retlen,
					NULL);

				//wprintf(L"0x%x, func=0x%x, access=%s, len=%d\n", ioctl_code, i, access_str(k), inbuf_len[j]);

				/* succeed */
				if (status)
				{
					notify_log(wcs_format(buf, max_buf_len, L"发现一个可用的ioctl code 0x%x", ioctl_code));
					
					if (ele_device)
					{
						ele_ioctl = 0;
						if (!is_list_empty(&ele_device->element_list))
						{
							for (list_entry = ele_device->element_list.flink; list_entry != &ele_device->element_list; list_entry = list_entry->flink)
							{
								ele = container_of(list_entry, xml_element, list_entry);
								if (0 == wcscmp(xml_query_attribute(ele, "code"), wcs_format(buf, max_buf_len, L"%x", ioctl_code)))
								{
									ele_ioctl = ele;
									break;
								}
							}
						}

						if (!ele_ioctl)
						{
							ele = xml_new_element("ioctl");
							xml_set_attribute(ele, "code", wcs_format(buf, max_buf_len, L"%x", ioctl_code));
							xml_set_attribute(ele, "type", L"FILE_DEVICE_UNKNOWN");
							xml_set_attribute(ele, "function", wcs_format(buf, max_buf_len, L"%x", i));
							xml_set_attribute(ele, "method", L"METHOD_BUFFERED");
							xml_set_attribute(ele, "access", access_wcs(k));

							insert_into_list(&ele_device->element_list, &ele->list_entry);
							notify_log(L"添加");
						}
						else
						{
							notify_log(L"已存在，忽略");
						}
					}
					break;
				}
				else
				{
					notify_log(wcs_format(buf, max_buf_len, L"error code: %d", GetLastError()));
				}

				Sleep(fuzz_sleep_time);
			}
		}
	}

	save_config(doc, output);
	notify_log(wcs_format(buf, max_buf_len, L"保存结果至 \"%s\"", output));
	
	buffer = load_file(output);
	if (buffer)
	{
		large_msg = str_to_wcs(buffer, -1, encoding_ansi);
		notify_log(large_msg);
		free(large_msg);
		free(buffer);
	}

	xml_free_document(doc);

	CloseHandle(handle);
}

void scan(wchar_t* device, wchar_t* output, fuzz_notify_func notify, args_t* notify_data)
{
	scan_in_range(device, 0x800, 0xfff, output, notify, notify_data);
}

/**************************************************************************************************************/

void fuzz_with_random_data(HANDLE device_handle, unsigned long ioctl_code, fuzz_notify_func notify, args_t* notify_data, int percent_min, int percent_max)
{
	int i = 0, j = 0;
	int inlen = 0, oulen = 0;
	int retlen = 0;
	int status = 0; 
	int progress = 0;
	args_t* args = (args_t*)notify_data;
	wchar_t buf[max_buf_len];
	unsigned tick = GetTickCount();
	unsigned tick2 = GetTickCount();

	notify_log(wcs_format(buf, max_buf_len, L"fuzz 随机数据测试, ioctl = 0x%x", ioctl_code, i));

	for (i = 0; i < test_random_count; i++)
	{
		inlen = oulen = getrand(0, max_buf_len);

		memset(inbuf, 0x00, max_buf_len);
		for (j = 0; j < (int)inlen; j++)
			inbuf[j] = (char)getrand(0x00, 0xff);

		tinylog(L"fuzz 0x%x", ioctl_code);
		
		status = DeviceIoControl(device_handle,
			ioctl_code,
			inbuf,
			inlen,
			oubuf,
			oulen,
			&retlen,
			NULL);

		if (GetTickCount() - update_tick > tick || GetTickCount() < tick)
		{
			notify_log_dot();
			tick = GetTickCount();
		}
		if (GetTickCount() - update_progress_tick > tick2 || GetTickCount() < tick2)
		{
			tick2 = GetTickCount();
			progress = percent_min + (int)(1.0 * (percent_max - percent_min) * i * 1.0  / test_random_count);
			notify_progress(progress, 100);
		}

		Sleep(fuzz_sleep_time);
	}	

	notify_progress(percent_max, 100);
}

void fuzz_overflow(HANDLE device_handle, unsigned long ioctl_code, fuzz_notify_func notify, args_t* notify_data, int percent_min, int percent_max)
{
	int i = 0;
	int retlen = 0;
	int status = 0;
	args_t* args = (args_t*)notify_data;
	wchar_t buf[max_buf_len];
	unsigned tick2 = GetTickCount();
	unsigned tick = GetTickCount();
	int progress = 0;

	memset(inbuf, 0x41, max_overflow_buf);

	notify_log(wcs_format(buf, max_buf_len, L"fuzz 溢出测试, ioctl = 0x%x", ioctl_code));

	for (i = 0; i <= max_overflow_buf; i += 0x100)
	{
		tinylog(L"fuzz 0x%x", ioctl_code);

		status = DeviceIoControl(device_handle,
			ioctl_code,
			inbuf,
			i,
			oubuf,
			i,
			&retlen,
			NULL);

		if (GetTickCount() - update_tick > tick || GetTickCount() < tick)
		{
			notify_log_dot();
			tick = GetTickCount();
		}

		if (GetTickCount() - update_progress_tick > tick2 || GetTickCount() < tick2)
		{
			tick2 = GetTickCount();
			progress = percent_min + (int)(1.0 * (percent_max - percent_min) * i * 1.0 / max_overflow_buf);
			notify_progress(progress, 100);
		}

		Sleep(fuzz_sleep_time);
	}

	notify_progress(percent_max, 100);
}

CHAR ascii_string10[0x10];
CHAR ascii_string100[0x100];
CHAR ascii_string1000[0x1000];

WCHAR unicode_string10[0x10];
WCHAR unicode_string100[0x100];
WCHAR unicode_string1000[0x1000];

DWORD table_dwords[0x100];

DWORD fuzz_constants[] = {
	0x00000000, 0x00000001, 0x00000004, 0xFFFFFFFF,
	0x00001000, 0xFFFF0000, 0xFFFFFFFE, 0xFFFFFFF0,
	0xFFFFFFFC, 0x70000000, 0x7FFEFFFF, 0x7FFFFFFF,
	0x80000000,
	(DWORD)ascii_string10,
	(DWORD)ascii_string100,
	(DWORD)ascii_string1000,
	(DWORD)unicode_string10,
	(DWORD)unicode_string100,
	(DWORD)unicode_string1000,
	(DWORD)table_dwords
};

void initialize_junk_data()
{
	int i;
	memset(ascii_string10, 0x41, 0x10);
	memset(ascii_string100, 0x41, 0x100);
	memset(ascii_string1000, 0x41, 0x1000);

	wmemset(unicode_string10, 0x0041, 0x10);
	wmemset(unicode_string100, 0x0041, 0x100);
	wmemset(unicode_string1000, 0x0041, 0x1000);

	for (i = 0; i<(sizeof(table_dwords) / 4); i++)
		table_dwords[i] = 0xFFFF0000;
}


void fuzz_with_predefined_data(HANDLE device_handle, unsigned long ioctl_code, fuzz_notify_func notify, args_t* notify_data, int percent_min, int percent_max)
{
	int i = 0, j = 0;
	DWORD fuzz_data = 0;
	int inlen = 0, oulen = 0;
	int status = 0;
	int retlen = 0;
	int progress = 0;
	args_t* args = (args_t*)notify_data;
	wchar_t buf[max_buf_len];
	unsigned tick = GetTickCount();
	unsigned tick2 = GetTickCount();

	initialize_junk_data();

	notify_log(wcs_format(buf, max_buf_len, L"fuzz 预定义数据测试, ioctl = 0x%x", ioctl_code));

	for (i = 0; i < max_buf_len; i = i + 4)
	{
		for (j = 0; j < max_buf_len; j++)
			inbuf[j] = (BYTE)getrand(0x00, 0xff);

		for (j = 0; j < (sizeof(fuzz_constants) / 4); j++)
		{
			fuzz_data = fuzz_constants[j];

			// Choose a random element 
			inbuf[i] = (char)(fuzz_data & 0x000000ff);
			inbuf[i + 1] = (char)((fuzz_data & 0x0000ff00) >> 8);
			inbuf[i + 2] = (char)((fuzz_data & 0x00ff0000) >> 16);
			inbuf[i + 3] = (char)((fuzz_data & 0xff000000) >> 24);

			tinylog(L"fuzz 0x%x", ioctl_code);

			status = DeviceIoControl(device_handle,
				ioctl_code,
				inbuf,
				max_buf_len,
				oubuf,
				max_buf_len,
				&retlen,
				NULL);

			if (GetTickCount() - update_tick > tick || GetTickCount() < tick)
			{
				notify_log_dot();
				tick = GetTickCount();
			}

			Sleep(fuzz_sleep_time);
		}

		if (GetTickCount() - update_progress_tick > tick2 || GetTickCount() < tick2)
		{
			tick2 = GetTickCount();
			progress = percent_min + (int)(0.8 * (percent_max - percent_min) * i * 1.0 / max_buf_len);
			notify_progress(progress, 100);
		}
	}

	for (j = 0; j < max_buf_len; j = j + 4)
	{
		inlen = oulen = getrand(0, max_buf_len);

		memset(inbuf, 0x00, max_buf_len);
		for (i = 0; i < inlen; i = i + 4)
		{
			fuzz_data = fuzz_constants[getrand(0, (sizeof(fuzz_constants) / 4) - 1)];

			inbuf[i] = (char)(fuzz_data & 0x000000ff);
			inbuf[i + 1] = (char)((fuzz_data & 0x0000ff00) >> 8);
			inbuf[i + 2] = (char)((fuzz_data & 0x00ff0000) >> 16);
			inbuf[i + 3] = (char)((fuzz_data & 0xff000000) >> 24);
		}
		tinylog(L"fuzz 0x%x", ioctl_code);

		status = DeviceIoControl(device_handle,
			ioctl_code,
			inbuf,
			inlen,
			oubuf,
			oulen,
			&retlen,
			NULL);

		if (GetTickCount() - update_tick > tick || GetTickCount() < tick)
		{
			notify_log_dot();
			tick = GetTickCount();
		}


		if (GetTickCount() - update_progress_tick > tick2 || GetTickCount() < tick2)
		{
			tick2 = GetTickCount();
			progress = (int)(percent_min + 0.8 * (percent_max - percent_min) + 0.2 * (percent_max - percent_min) * (j * 1.0 / max_buf_len));
			notify_progress(progress, 100);
		}

		Sleep(fuzz_sleep_time);
	}

	notify_progress(percent_max, 100);
}


void fuzz_one_ioctl(wchar_t* device, unsigned long ioctl, fuzz_notify_func notify, args_t* notify_data)
{
	HANDLE handle = INVALID_HANDLE_VALUE;

	if (!device)
		return;

	handle = open_device(device);
	if (handle == INVALID_HANDLE_VALUE)
		return;

	fuzz_overflow(handle, ioctl, notify, notify_data, 0, 10);

	fuzz_with_predefined_data(handle, ioctl, notify, notify_data, 10, 30);

	fuzz_with_random_data(handle, ioctl, notify, notify_data, 30, 100);

	CloseHandle(handle);
}

void fuzz(wchar_t* device, wchar_t* config, fuzz_notify_func notify, args_t* notify_data)
{
	xml_document* doc = 0;
	xml_element* ele = 0;
	xml_element* ele_device = 0;
	list_entry_t* list_entry = 0;
	unsigned long ioctl = 0;
	wchar_t* msg = 0;
	args_t* args = (args_t*)notify_data;
	wchar_t buf[max_buf_len];
	int count = 0;
	int i = 0;
	HANDLE handle = INVALID_HANDLE_VALUE;
	int progress = 0;
	int delta = 0;

	notify_log(L">>>>>>");
	notify_log(wcs_format(buf, max_buf_len, L"使用配置 \"%s\" fuzz 测试设备 \"%s\"", config, device));

	if (!device || !config)
	{
		notify_error(L"设备名空 或者 配置文件空");
		return;
	}

	handle = open_device(device);
	if (handle == INVALID_HANDLE_VALUE)
	{
		notify_error(wcs_format(buf, max_buf_len, L"打开设备错误 \"%s\", 错误码: %d", device, GetLastError()));
		return;
	}
	else
	{
		notify_log(wcs_format(buf, max_buf_len, L"打开设备成功 \"%s\"", device));
	}

	doc = load_config(config);
	if (!doc)
	{
		notify_error(wcs_format(buf, max_buf_len, L"加载配置失败，\"%s\"请检查配置文件是否正确", config));
		return;
	}
	else
		notify_log(wcs_format(buf, max_buf_len, L"加载配置成功 \"%s\"", config));

	if (is_list_empty(&doc->root_element->element_list))
	{
		notify_error(L"加载配置失败，请检查配置文件是否正确");
		return;
	}

	for (list_entry = doc->root_element->element_list.flink; list_entry != &doc->root_element->element_list; list_entry = list_entry->flink)
	{
		ele = container_of(list_entry, xml_element, list_entry);
		if (0 == wcscmp(xml_query_attribute(ele, "name"), device))
		{
			ele_device = ele;
			break;
		}
	}

	if (!ele_device)
	{
		notify_error(wcs_format(buf, max_buf_len, L"加载配置失败，配置文件中不存在设备名为 \"%s\" 的配置", device));
		return;
	}

	if (is_list_empty(&ele_device->element_list))
	{
		notify_error(wcs_format(buf, max_buf_len, L"加载配置失败，设备 \"%s\" 的配置为空", device));
		return;
	}

	for (list_entry = ele_device->element_list.flink; list_entry != &ele_device->element_list; list_entry = list_entry->flink)
	{
		count++;
	}

	notify_log(wcs_format(buf, max_buf_len, L"共有 %d 个ioctl code需要fuzz 测试", count));

	for (list_entry = ele_device->element_list.flink; list_entry != &ele_device->element_list; list_entry = list_entry->flink)
	{
		ele = container_of(list_entry, xml_element, list_entry);
		ioctl = wcstol(xml_query_attribute(ele, "code"), 0, 16);
		if (ioctl)
		{
			progress = i * 100 / count;
			delta = 100 / count;

			fuzz_overflow(handle, ioctl, notify, notify_data, progress, (int)(progress + 0.1 * delta));

			fuzz_with_predefined_data(handle, ioctl, notify, notify_data, (int)(progress + 0.1 * delta), (int)(progress + 0.3 * delta));

			fuzz_with_random_data(handle, ioctl, notify, notify_data, (int)(progress + 0.3 * delta), progress + delta);
		}

		i++;
	}

	notify_progress(100, 100);

	CloseHandle(handle);

	xml_free_document(doc);
}