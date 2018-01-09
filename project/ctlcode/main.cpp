#include "tinyopt.h"
#include "tinylog.h"
#include "ctlcode.h"
#include <stdlib.h>

void usage()
{
	char* usage_str = ""
		"                                                                           \r\n"
		"Usage: ctlcode.exe [-c code] [-t type -a access -f function -m method] [-h]\r\n"
		"                                                                           \r\n"
		"    -c    I/O control code. hex number without \"0x\"                      \r\n"
		"    -t    specify device type. hex number without \"0x\"                   \r\n"
		"    -a    specify access type. 0-3                                         \r\n"
		"    -f    specify function code. hex number without \"0x\"                 \r\n"
		"    -m    specify method type. 0-3                                         \r\n"
		"    -h    show this usage                                                  \r\n"
		"                                                                           \r\n";
	log_printa(usage_str);
}

int main(int argc, char** argv)
{
	char opt = 0;
	char* arg = 0;
	int code = -1;
	int type = -1;
	int access = -1;
	int func = -1;
	int method = -1;

	for (;;)
	{
		opt = tinyopt(&arg, "a:c:f:hm:t:", argc, argv);
		if (!opt)
			break;
		switch (opt)
		{
		case 'a':
			assert(arg);
			access = strtol(arg, 0, 16);
			break;
		case 'c':
			assert(arg && strlen(arg) < 256);
			code = strtol(arg, 0, 16);
			break;
		case 'f':
			assert(arg);
			func = strtol(arg, 0, 16);
			break;
		case 'h':
			usage();
			return 0;
		case 'm':
			assert(arg);
			method = strtol(arg, 0, 16);
			break;
		case 't':
			assert(arg);
			type = strtol(arg, 0, 16);
			break;
		default:
			break;
		}
	}

	if (code != -1)
	{
		ctlcode_t::code(code);
	}
	else if (type != -1 && access != -1 && func != -1 && method != -1)
	{
		ctlcode_t::component(type, access, func, method);
	}
	else
		usage();

	return 0;
}