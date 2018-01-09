#include "tiny.h"
#include "tinylog.h"
#include "tinyopt.h"
#include "tinysys.h"
#pragma comment(lib, "version")
#pragma comment(lib, "shlwapi.lib")

static char temp[256];
static char curr[256];

void usage()
{
	char* usage_str = ""
		"                                                                           \r\n"
		"Usage: version.exe -f fullpath [-v a.b.c.d] [-h]                           \r\n"
		"                                                                           \r\n"
		"    -f    specify file with full path                                      \r\n"
		"    -v    if set then write or modify; if not set then just read.          \r\n"
		"    -h    show this usage                                                  \r\n"
		"                                                                           \r\n";
	log_printa(usage_str);
}

void compile(char* rc_file, char* res_file)
{
	char path[256];
	char args[256];

	if (!rc_file || !res_file)
		return;


	str_format(path, 256, "%s/rc.exe", curr);
	str_format(args, 256, "/l 0x409 /fo \"%s\" %s", res_file, rc_file);

	if (!PathFileExistsA(path))
	{
		log_errora("cannot find rc.exe, which should be at the same directory: %s", curr);
		return;
	}
		
	launch(path, args, 1);
}

void prepare(char* rc_file, char* version)
{
	char format[] = {
		"VS_VERSION_INFO VERSIONINFO\r\n"
		"FILEVERSION %d,%d,%d,%d\r\n"
		"BEGIN\r\n"
		"BLOCK \"VarFileInfo\"\r\n"
		"BEGIN\r\n"
		"VALUE \"Translation\", 0x409, 0x4E4\r\n"
		"END\r\n"
		"BLOCK \"StringFileInfo\"\r\n"
		"BEGIN\r\n"
		"BLOCK \"040904E4\"\r\n"
		"BEGIN\r\n"
		"VALUE \"FileVersion\", \"%s\"\r\n"
		"END\r\n"
		"END\r\n"
		"END\r\n"
	};
	char buf[1024];
	int ms1, ms2, ls1, ls2;

	if (!rc_file || !version)
		return;
	
	sscanf_s(version, "%d.%d.%d.%d", &ms1, &ms2, &ls1, &ls2);

	str_format(buf, 1024, format, ms1, ms2, ls1, ls2, version);
	write_raw_file(rc_file, buf, strlen(buf));
}

void read_version(char* file)
{
	unsigned size = 0;
	char* buf = 0;
	VS_FIXEDFILEINFO* info = 0;
	short ms1, ms2, ls1, ls2;

	if (!file)
		return;

	do
	{
		size = GetFileVersionInfoSizeA(file, 0);
		buf = (char*)safe_malloc(size + 1);
		if (!GetFileVersionInfoA(file, 0, size, buf))
		{
			log_debuga("fail to get file version info: %d", GetLastError());
			break;
		}
			
		if (!VerQueryValueA(buf, "\\", (LPVOID*)&info, &size))
		{
			log_debuga("fail to query version value: %d", GetLastError());
			break;
		}

		ms1 = info->dwFileVersionMS >> 16;
		ms2 = info->dwFileVersionMS & 0xffff;
		ls1 = info->dwFileVersionLS >> 16;
		ls2 = info->dwFileVersionLS & 0xffff;

		log_printa("%d.%d.%d.%d", ms1, ms2, ls1, ls2);
	} while (0);

	free(buf);
}

unsigned short version_lang(char* file)
{
	unsigned size = 0;
	char* buf = 0;
	struct {
		short lang;
		short codepage;
	}*translation;
	short lang = 0x409;

	do
	{
		size = GetFileVersionInfoSizeA(file, 0);
		if (!size)
		{
			log_debuga("fail to get file version info size: %s, %d. ignore", file, GetLastError());
			break;
		}

		buf = (char*)safe_malloc(size + 1);
		if (!GetFileVersionInfoA(file, 0, size, buf))
		{
			log_debuga("fail to get file version info: %s, %d. ignore", file, GetLastError());
			break;
		}

		VerQueryValueA(buf, "\\VarFileInfo\\Translation", (LPVOID*)&translation, &size);
		if (!size)
		{
			log_debuga("fail to query version value: %d", GetLastError());
			break;
		}

		lang = translation->lang;

	} while (0);
		
	free(buf);
	return lang;
}

void update(char* file, char* buf, int len)
{
	HANDLE hres = 0;

	do
	{
		hres = BeginUpdateResourceA(file, TRUE);
		if (!hres)
		{
			log_errora("fail to begin update: %d", GetLastError());
			break;
		}

		if (!UpdateResource(hres, RT_VERSION, MAKEINTRESOURCE(VS_VERSION_INFO), version_lang(file), buf, len))
		{
			log_errora("fail to update: %d", GetLastError());
			break;
		}
		
		if (!EndUpdateResource(hres, FALSE))
		{
			log_errora("fail to end update: %d", GetLastError());
			break;
		}
	} while (0);	
}

int locate_version_info(char* buf, int len)
{
	unsigned* pu = 0;
	char* p = 0;
	int i = 0;
	for (i = 0; i < len - 3; i += 4)
	{
		pu = (unsigned*)&(buf[i]);
		if (*pu == 0xFEEF04BD)
			break;
	}

	if (*pu == 0xFEEF04BD)
	{
		p = &(buf[i - 40]);
		if (p[2] == 0x34 && p[3] == 0)
			return i - 40;
	}
	
	log_errora("version info not found!");
	return -1;
}

void write_version(char* file, char* version)
{
	char* buf = 0;
	int len = 0;
	int p = 0;
	char rc_file[256];
	char res_file[256];

	if (!file || !version)
		return;

	do
	{
		str_format(rc_file, 256, "%stmp.rc", temp);
		prepare(rc_file, version);

		str_format(res_file, 256, "%stmp.res", temp);
		compile(rc_file, res_file);

		len = read_raw_file(res_file, &buf);
		if (!len)
			break;

		p = locate_version_info(buf, len);
		update(file, &buf[p], len - p);
	} while (0);
	
	free(buf);
}

int main(int argc, char* argv[])
{
	char opt = 0;
	char* arg = 0;
	char* file = 0;
	char* version = 0;

	GetTempPathA(256, temp);
	current_dir(curr, 256);

	for (;;)
	{
		opt = tinyopt(&arg, "f:v:h", argc, argv);
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
		case 'v':
			assert(arg);
			version = arg;
			break;
		default:
			break;
		}
	}

	if (file)
	{
		if (version)
		{
			write_version(file, version);
		}
		else
		{
			read_version(file);
		}
	}
	else
		usage();

	return 0;
}
