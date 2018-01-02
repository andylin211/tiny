#include "tinyopt.h"
#include "tinylog.h"
#include "ctlfuzz.h"
#include <stdlib.h>

void usage()
{
	char* usage_str = ""
		"                                                               \r\n"
		"Usage: ctlfuzz.exe [-d device -c code -i inlen -o oulen]  [-h] \r\n"
		"                                                               \r\n"
		"    -d    device name to be scanned without \"\\\\.\\\"        \r\n"
		"    -c    I/O control code. hex number without \"0x\"          \r\n"
		"    -i    specify in buffer size (byte)                        \r\n"
		"    -o    specify out buffer size (byte)                       \r\n"
		"    -h    show this usage                                      \r\n"
		"                                                               \r\n";
	log_printa(usage_str);
}

int main(int argc, char** argv)
{
	char opt = 0;
	char* arg = 0;
	char device[256] = { 0 };
	int code = 0;
	int inlen = 256;
	int oulen = 256;

	for (;;)
	{
		opt = tinyopt(&arg, "c:d:i:ho:", argc, argv);
		if (!opt)
			break;
		switch (opt)
		{
		case 'c':
			assert(arg && strlen(arg) < 256);
			code = strtol(arg, 0, 16);
			break;
		case 'd':
			assert(arg && strlen(arg) < 256);
			strcpy_s(device, 256, arg);
			str_remove(device, '\"');
			break;
		case 'h':
			usage();
			return 0;
		case 'i':
			assert(arg);
			inlen = strtol(arg, 0, 10);
			break;
		case 'o':
			assert(arg);
			oulen = strtol(arg, 0, 10);
			break;
		default:
			break;
		}
	}

	if (code && device)
	{
		ctlfuzz_t fuzz(device);
		fuzz.fuzz(code, inlen, oulen);
	}
	else
		usage();

	return 0;
}