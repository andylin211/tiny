#include "ioctlfuzz.h"
#include <stdio.h>
#include "tinystr.h"
#include "tinyargs.h"
#include "md5.h"
#include <wchar.h>
#include <process.h>
#include <Windows.h>
#include "getopt.h"
#include "tinylog.h"
#include "ioctlfuzz_ui.h"

#define max_buf_len 512

#define max_log_size 10240

int g_busy = 0;

int g_cancel = 0;

void add_log(void* ui_context, wchar_t* text)
{
	int len = 0;
	wchar_t* p = 0;
	wchar_t* buffer = wcs_format_large(L"%s%s", ui_get_value(ui_context, L"log_textarea"), text);
	//tinylog(buffer);

	len = wcslen(buffer);
	if (len > max_log_size)
		p = &(buffer[wcs_rfind_from(buffer, L'\n', len - max_log_size) + 1]);
	else
		p = buffer;

	ui_set_value(ui_context, L"log_textarea", p);
	free(buffer);
}

ui_click(check_button, ui_context)
{
	wchar_t symlnk_name[max_buf_len];
	wchar_t* device_name = 0;
	HANDLE handle = INVALID_HANDLE_VALUE;
	wchar_t buf[max_buf_len];

	if (g_busy)
		return;

	device_name = ui_get_value(ui_context, L"device_input");	

	if (!device_name || wcslen(device_name) < 2)
		return;

	g_busy = 1;

	swprintf(symlnk_name, max_buf_len, L"\\\\.\\%s", device_name);
	handle = CreateFile(
		symlnk_name,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_SYSTEM,
		NULL);

	if (handle == INVALID_HANDLE_VALUE)
		MessageBox(ui_get_hwnd(ui_context), wcs_format(buf, max_buf_len, L"设备打开失败, 错误码 %d\r\n请确认设备名字正确(不需要\"\\\\.\\\")", GetLastError()), L"ioctl fuzz", MB_OK);
	else
		MessageBox(ui_get_hwnd(ui_context), L"设备打开成功, 可以开始扫描or测试", L"ioctl fuzz", MB_OK);

	CloseHandle(handle);
	g_busy = 0;
}

ui_click(output_button, ui_context)
{
	wchar_t* file = 0;

	if (g_busy)
		return;

	file = ui_ask_save_file(ui_context);
	if (file)
		ui_set_value(ui_context, L"output_input", file);
}

ui_task(scan, arg_list)
{
	args_t* args = (args_t*)arg_list;
	void* ui_context = args->p0;
	wchar_t* msg = (wchar_t*)args->p1;
	int current = (int)args->p2;
	int total = (int)args->p3;
	int progress = 0;
	wchar_t* large_msg = 0;
	wchar_t buf[max_buf_len];
	int is_dot = (int)args->p4;

	if (is_dot)
	{
		add_log(ui_context, L".");
	}
	else
	{
		if (msg)
		{
			if (wcslen(msg) <= 256)
				add_log(ui_context, wcs_format(buf, max_buf_len, L"%s\r\n", msg));
			else
			{
				large_msg = wcs_format_large(L"%s\r\n", msg);
				add_log(ui_context, large_msg);
				free(large_msg);
			}
		}

		if (total)
		{
			progress = current * 100 / total;
			ui_set_value(ui_context, L"progress", wcs_format(buf, max_buf_len, L"%d", progress));
			if (current == total)
				g_busy = 0;
		}
	}
	

	free(msg);
	free(args);
}

void scan_notify(int current, int total, void* notify_data)
{
	args_t* args = (args_t*)notify_data;
	args_t* p = (args_t*)safe_malloc(sizeof(args_t));
	void* ui_context = args->p0;

	p->p0 = args->p0;
	if (args->p2)
	{
		p->p4 = (void*)1;
	}
	else if (args->p1)
	{
		p->p1 = (wchar_t*)safe_malloc(sizeof(wchar_t) * (wcslen(args->p1) + 1));
		wcscpy(p->p1, args->p1);
	}
	p->p2 = (void*)current;
	p->p3 = (void*)total;

	ui_post_task(ui_context, "scan", p);
	
}

unsigned __stdcall scan_thread(void* arg_list)
{
	args_t* args = (args_t*)arg_list;
	void* ui_context = args->p0;
	wchar_t* device = (wchar_t*)args->p1;
	wchar_t* output = (wchar_t*)args->p2;
	int from = (int)args->p3;
	int to = (int)args->p4;

	if (from && to)
		scan_in_range(device, from, to, output, scan_notify, args);
	else
		scan(device, output, scan_notify, args);

	free(device);
	free(output);
	free(args);

	_endthreadex(0);
	return 0;
}

ui_click(scan_button, ui_context)
{	
	wchar_t* device = 0;
	wchar_t* output = 0;
	wchar_t* from = 0;
	wchar_t* to = 0;
	int from_i, to_i;
	args_t* args = 0;
	HANDLE thread = NULL;

	if (g_busy)
		return;

	g_busy = 1;
	
	device = ui_get_value(ui_context, L"device_input");
	output = ui_get_value(ui_context, L"output_input");
	from = ui_get_value(ui_context, L"from_input");
	to = ui_get_value(ui_context, L"to_input");
	from_i = wcstol(from, 0, 16);
	to_i = wcstol(to, 0, 16);

	if (!device)
		return;

	if (!output && (!from || !to))
		return;

	args = (args_t*)safe_malloc(sizeof(args_t));
	args->p0 = ui_context;
	args->p1 = (wchar_t*)safe_malloc(sizeof(wchar_t) * (wcslen(device) + 1));
	wcscpy(args->p1, device);
	args->p2 = (wchar_t*)safe_malloc(sizeof(wchar_t) * (wcslen(output) + 1));
	wcscpy(args->p2, output);
	args->p3 = (void*)from_i;
	args->p4 = (void*)to_i;

	thread = (HANDLE)_beginthreadex(NULL, 0, scan_thread, (void*)args, 0, NULL);
	CloseHandle(thread);
}

ui_click(config_button, ui_context)
{
	wchar_t* file = 0;

	if (g_busy)
		return;

	file = ui_ask_open_file(ui_context);
	if (file)
		ui_set_value(ui_context, L"config_input", file);
}


unsigned __stdcall fuzz_thread(void* arg_list)
{
	args_t* args = (args_t*)arg_list;
	void* ui_context = args->p0;
	wchar_t* device = (wchar_t*)args->p1;
	wchar_t* config = (wchar_t*)args->p2;
	unsigned long ioctl = (unsigned long)args->p3;

	if (ioctl)
		fuzz_one_ioctl(device, ioctl, scan_notify, args);
	else
		fuzz(device, config, scan_notify, args);

	free(device);
	free(config);
	free(args);

	_endthreadex(0);
	return 0;
}

ui_click(fuzz_button, ui_context)
{
	wchar_t* device = 0;
	wchar_t* config = 0;
	wchar_t* ioctl = 0;
	unsigned long ioctl_i = 0;
	args_t* args = 0;
	HANDLE thread = NULL;

	if (g_busy)
		return;

	device = ui_get_value(ui_context, L"device_input");
	config = ui_get_value(ui_context, L"config_input");
	ioctl = ui_get_value(ui_context, L"ioctl_input");
	ioctl_i = wcstol(ioctl, 0, 16);

	if (!device)
		return;

	if (!config && !ioctl_i)
		return;

	g_busy = 1;

	args = (args_t*)safe_malloc(sizeof(args_t));
	args->p0 = ui_context;
	args->p1 = (wchar_t*)safe_malloc(sizeof(wchar_t) * (wcslen(device) + 1));
	wcscpy(args->p1, device);
	args->p2 = (wchar_t*)safe_malloc(sizeof(wchar_t) * (wcslen(config) + 1));
	wcscpy(args->p2, config);
	args->p3 = (void*)ioctl_i;

	thread = (HANDLE)_beginthreadex(NULL, 0, fuzz_thread, (void*)args, 0, NULL);
	CloseHandle(thread);
}

ui_click(clear_button, ui_context)
{
	ui_set_value(ui_context, L"log_textarea", L"");
}

void usage(char *program_name)
{
	printf("  Usage                                                             \n");
	printf("  -----                                                             \n");
	printf("  %s -d <device name> [-c <config> | -i <ioctl code>]               \n", program_name);
	printf("  %s -s -d <device name> -o <output> [-f <from> -t <to>]            \n", program_name);
	printf("                                                                    \n");
	printf("  Options                                                           \n");
	printf("  -------                                                           \n");
	printf("    Basic                                                           \n");
	printf("    -----                                                           \n");
	printf("    -s    scan mode (when set) or fuzz mode                         \n");
	printf("    -d    device name (without \\\\.\\)                             \n");
	printf("    -h    display this help                                         \n");
	printf("    -q    quiet			                                            \n");
	printf("    -u    ui mode                                                   \n");
	printf("                                                                    \n");
	printf("    Fuzz Mode		                                                \n");
	printf("    --------                                                        \n");
	printf("    -c    config file, fuzz using config file                       \n");
	printf("    -i    ioctl code, fuzz only this code (hex, eg: 22a444)         \n");
	printf("                                                                    \n");
	printf("    Scan Mode		                                                \n");
	printf("    --------                                                        \n");
	printf("    -o    output file (must set), storing scanning result           \n");
	printf("    -f    scan ioctl code FROM this function (hex, eg: 800)         \n");
	printf("    -t    scan ioctl code TO this function (hex, eg: 8ff)           \n");
	printf("                                                                    \n");
	printf("  Examples                                                          \n");
	printf("  --------                                                          \n");
	printf("    Fuzzing a given IOCTL (quiet mode):                             \n");
	printf("      > %s -d tssk -i 22a444 -q                                     \n", program_name);
	printf("                                                                    \n");
	printf("    Fuzzing using config file:                                      \n");
	printf("      > %s -d tssk -c config.xml                                    \n", program_name);
	printf("                                                                    \n");
	printf("    Scanning in a given ioctl range:                                \n");
	printf("      > %s -s -d tssk -f 800 -t 900 -o output.xml                   \n", program_name);
	printf("                                                                    \n");
	printf("    Scanning without given range:                                   \n");
	printf("      > %s -s -d tssk -o output.xml                                 \n", program_name);
	printf("                                                                    \n");
}

int main(int argc, char** argv)
{
	option_context context;
	wchar_t* device = 0;
	unsigned long from = 0, to = 0;
	unsigned long ioctl = 0;
	wchar_t* config = 0;
	wchar_t* output = 0;
	int is_scan = 0;
	int is_ui = 0;

	init_option_context(&context, argc, argv, "c:d:f:hi:o:qst:u");
	while (get_option(&context))
	{
		switch (context.option_char)
		{
		case 'c':
			config = str_to_wcs(context.the_argument, -1, encoding_ansi);
			break;
		case 'd':
			device = str_to_wcs(context.the_argument, -1, encoding_ansi);
			break;
		case 'f':
			from = strtol(context.the_argument, 0, 16);
			break;
		case 'h':
			usage(argv[0]);
			return 0;
		case 'i':
			ioctl = strtol(context.the_argument, 0, 16);
			break;
		case 'o':
			output = str_to_wcs(context.the_argument, -1, encoding_ansi);
			break;
		case 'q':
			break;
		case 's':
			is_scan = 1;
			break;
		case 't':
			to = strtol(context.the_argument, 0, 16);
			break;
		case 'u':
			is_ui = 1;
			break;
		default:
			break;
		}
	}

	if (is_ui)
		ui();
	else if (is_scan)
	{
		if (device && output)
		{
			if (from && to)
				scan_in_range(device, from, to, output, 0, 0);
			else
				scan(device, output, 0, 0);
		}
		else
			usage(argv[0]);
	}
	else
	{
		if (device)
		{
			if (config)
				fuzz(device, config, 0, 0);
			else if (ioctl)
				fuzz_one_ioctl(device, ioctl, 0, 0);
			else
				usage(argv[0]);
		}
		else
			usage(argv[0]);
	}

	free(config);
	free(device);
	free(output);

	return 0;
}