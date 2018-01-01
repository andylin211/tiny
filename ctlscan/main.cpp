
#include "ctlscan.h"
#include "tinystr.h"
#include "tinylog.h"
#include "tinysys.h"
#include "tinyopt.h"


void usage()
{
	char* usage_str =
		"                                                                    \r\n"
		"Usage: ctlscan.exe -d \"device name\" [-c code] [-f from -t to] [-h]\r\n"
		"                                                                    \r\n"
		"    -d    device name to be scanned without \"\\\\.\\\"             \r\n"
		"    -c    I/O control code sent to device. hex number without \"0x\"\r\n"
		"    -f,-t instead of specifying I/O control code, specify function  \r\n"
		"          range. hex number without \"0x\". e.g. -f 800 -t 900      \r\n"
		"    -h    show this usage                                           \r\n";
	log_printa(usage_str);
}

int main(int argc, char* argv[])
{
	char opt = 0;
	char* arg = 0;
	char device_name[256] = { 0 };
	int code = -1;
	int from = -1, to = -1;

/*	if (!is_elevated())
		return run_as();

	if (has_been_running(L"9F69256D-5F91-455D-96DE-5D8C5036DBAE"))
		return 0;*/

	for (;;)
	{
		opt = tinyopt(&arg, "c:d:f:ht:", argc, argv);
		if (!opt)
			break;
		switch (opt)
		{
		case 'c':
			assert(arg);
			code = strtol(arg, 0, 16);
			break;
		case 'd':
			assert(arg && strlen(arg) < 256);
			memcpy(device_name, arg, strlen(arg));
			break;
		case 'f':
			assert(arg);
			from = strtol(arg, 0, 16);
			break;
		case 'h':
			usage();
			return 0;
		case 't':
			assert(arg);
			to = strtol(arg, 0, 16);
			break;
		default:
			break;
		}
	}
	
	if (device_name && device_name[0] != 0)
	{
		ctlscan_t obj(device_name);
		if (code != -1)
			obj.scan(code);
		else if (device_name && from != -1 && to != -1)
			obj.scan(from, to);
		else
			usage();
	}
	else
		usage();

	return 0;
}

