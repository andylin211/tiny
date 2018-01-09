#include <Windows.h>
#include "tiny.h"
#include "tinyopt.h"
#include "tinylog.h"

void usage()
{
	char* usage_str = ""
		"                                                                           \r\n"
		"Usage: ini.exe -f fullpath -s section -k key [-v value] [-h]               \r\n"
		"                                                                           \r\n"
		"    -f    specify ini format file full path                                \r\n"
		"    -s    specify section name                                             \r\n"
		"    -k    specify key name                                                 \r\n"
		"    -v    if set then write or modify; if not set then just read.          \r\n"
		"    -h    show this usage                                                  \r\n"
		"                                                                           \r\n";
	log_printa(usage_str);
}

void write_ini(char* file, char* section, char* key, char* value)
{
	assert(file && section && key && value);
	log_debuga("write ini: file=%s, section=%s, key=%s, value=%s", file, section, key, value);
	int ret = WritePrivateProfileStringA(section, key, value, file);
	if (!ret)
		fprintf(stderr, "error %d", GetLastError());
}

/*
 * write to stdout
 */
void read_ini(char* file, char* section, char* key)
{
	char buf[256];
	assert(file && section && key);
	log_debuga("read ini: file=%s, section=%, key=%s", file, section, key);
	int count = GetPrivateProfileStringA(section, key, "null", buf, 256, file);
	int lasterr = GetLastError();
	if (lasterr == 0x2)	
		fprintf(stderr, "file specified is not found!");
	else if (lasterr)
		fprintf(stderr, "error %d", lasterr);
	else
	{
		if (count)
			fprintf(stdout, buf);
	}
}

int main(int argc, char** argv)
{
	char opt = 0;
	char* arg = 0;
	char* file = 0;
	char* section = 0;
	char* key = 0;
	char* value = 0;

	for (;;)
	{
		opt = tinyopt(&arg, "f:s:k:v:h", argc, argv);
		if (!opt)
			break;
		switch (opt)
		{
		case 'f':
			assert(arg);
			file = arg;
			break;
		case 'h':
			usage();
			return 0;
		case 's':
			assert(arg);
			section = arg;
			break;
		case 'k':
			assert(arg);
			key = arg;
			break;
		case 'v':
			assert(arg);
			value = arg;
			break;
		default:
			break;
		}
	}

	if (file && section && key)
	{
		if (value)
			write_ini(file, section, key, value);
		else
			read_ini(file, section, key);
	}
	else
		usage();

	return 0;
}