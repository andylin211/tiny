#include "global.h"
#include "FL\fl_utf8.h"
#include <stdio.h>
#include <ShlObj.h>
#include <assert.h>

char* label_conv(char* gbk) {
	static char buf[buf_len];
	fl_utf8from_mb(buf, buf_len, gbk, strlen(gbk));
	return buf;
}

void RunAs(LPCSTR _param)
{
	SHELLEXECUTEINFOA sei = { 0 };
	CHAR path[MAX_PATH] = { 0 };
	GetModuleFileNameA(NULL, path, MAX_PATH);


	sei.cbSize = sizeof(sei);
	sei.lpVerb = "runas";
	sei.lpFile = path;
	sei.nShow = SW_SHOWNORMAL;
	sei.lpParameters = _param;
	ShellExecuteExA(&sei);
}

int is_evevated()
{
	HANDLE hToken = NULL;
	BOOL bElevated = FALSE;
	TOKEN_ELEVATION tokenEle;
	DWORD dwRetLen = 0;

	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
		return 0;

	if (GetTokenInformation(hToken, TokenElevation, &tokenEle, sizeof(tokenEle), &dwRetLen))
		bElevated = tokenEle.TokenIsElevated;

	CloseHandle(hToken);
	hToken = NULL;

	return bElevated;
}

int has_been_running()
{
	DWORD error = 0;
	HANDLE mutex = CreateMutex(NULL, FALSE, L"8130DAD1-4E8D-43C4-8A49-70C242533BBD");
	error = GetLastError();
	if (ERROR_ALREADY_EXISTS == error || ERROR_ACCESS_DENIED == error)
		return 1;
	return 0;
}

int is_xp()
{
	OSVERSIONINFO osVer = { 0 };

	osVer.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if (GetVersionEx((OSVERSIONINFO*)&osVer))
	{
		if (osVer.dwMajorVersion == 5 && osVer.dwMinorVersion == 1)
			return 1;
	}

	return 0;
}

wchar_t* ask_open_file(HWND hwnd)
{
	static wchar_t file[buf_len] = { 0 };
	OPENFILENAME ofn = { 0 };

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwnd;
	ofn.nFilterIndex = 1;
	ofn.lpstrFile = file;
	ofn.nMaxFile = buf_len;
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;
	return GetOpenFileName(&ofn) ? file : 0;
}

wchar_t* ask_save_file(HWND hwnd)
{
	static wchar_t file[buf_len] = { 0 };
	OPENFILENAME ofn = { 0 };

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwnd;
	ofn.nFilterIndex = 1;
	ofn.lpstrFile = file;
	ofn.nMaxFile = buf_len;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	ofn.lpstrTitle = L"保存为";
	return GetSaveFileName(&ofn) ? file : 0;
}

wchar_t* ask_open_path(HWND hwnd)
{
	static wchar_t path[buf_len] = { 0 };
	BROWSEINFO bi = { 0 };

	bi.hwndOwner = hwnd;
	bi.pszDisplayName = path;
	bi.ulFlags = BIF_NEWDIALOGSTYLE;
	LPITEMIDLIST idl = SHBrowseForFolder(&bi);
	return SHGetPathFromIDList(idl, path) ? path : 0;
};

char* read_file_raw(wchar_t* file)
{
	FILE* fp = 0;
	char* buf = 0;
	int len = 0;

	if (!file)
		return 0;

	fp = _wfopen(file, L"rb");
	if (!fp)
		return 0;

	fseek(fp, 0, SEEK_END);
	len = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	buf = (char*)malloc(len + 1);
	assert(buf);

	fread(buf, len, 1, fp);
	fclose(fp);

	return buf;
}

char* hex_digit_str(int i)
{
	static char* _hex_digit_str[] = { "0", "1", "2","3","4","5","6","7","8","9","a","b","c","d","e","f", };
	assert(i >= 0 && i <= 15);
	return _hex_digit_str[i];
}

char* device_type_str(int i)
{
	static char* _device_type_str[] = {
	"beep",//0x1
	"cd rom",
	"cd rom file system",
	"controller",
	"datalink",
	"dfs",
	"disk",
	"disk file system",
	"file system",
	"inport port",
	"keyboard",
	"mailslot",
	"midi in",
	"midi out",
	"mouse",
	"multi unc provider",
	"named pipe",
	"network",
	"network browser",
	"network file system",
	"null",
	"parallel port",
	"physical netcard",
	"printer",
	"scanner",
	"serial mouse port",
	"serial port",
	"screen",
	"sound",
	"streams",
	"tape",
	"tape file system",
	"transport",
	"unknown",
	"video",
	"virtual disk",
	"wave in",
	"wave out",
	"8042 port",
	"network redirector",
	"battery",
	"bus extender",
	"modem",
	"vdm",
	"mass storage",
	"smb",
	"ks",
	"changer",
	"smartcard",
	"acpi",
	"dvd",
	"fullscreen video",
	"dfs file system",
	"dfs volume",
	"serenum",
	"termsrv",
	"ksec",
	"fips",
	"infiniband",
	"vmbus",
	"crypt provider",
	"wpd",
	"bluetooth",
	"mt composite",
	"mt transport",
	"biometric	",
	"pmi",
	};
	assert(i >= 0 && i < 45);
	return _device_type_str[i];
}

char* device_type_unknown_str()
{
	return "unknown";
}

char* device_type_unknown_hex()
{
	return "0x0022";
}

char* device_type_hex(int i)
{
	static char* _device_type_hex[] = {
	"0x0001",
	"0x0002",
	"0x0003",
	"0x0004",
	"0x0005",
	"0x0006",
	"0x0007",
	"0x0008",
	"0x0009",
	"0x000a",
	"0x000b",
	"0x000c",
	"0x000d",
	"0x000e",
	"0x000f",
	"0x0010",
	"0x0011",
	"0x0012",
	"0x0013",
	"0x0014",
	"0x0015",
	"0x0016",
	"0x0017",
	"0x0018",
	"0x0019",
	"0x001a",
	"0x001b",
	"0x001c",
	"0x001d",
	"0x001e",
	"0x001f",
	"0x0020",
	"0x0021",
	"0x0022",
	"0x0023",
	"0x0024",
	"0x0025",
	"0x0026",
	"0x0027",
	"0x0028",
	"0x0029",
	"0x002a",
	"0x002b",
	"0x002c",
	"0x002d",
	"0x002e",
	"0x002f",
	"0x0030",
	"0x0031",
	"0x0032",
	"0x0033",
	"0x0034",
	"0x0035",
	"0x0036",
	"0x0037",
	"0x0038",
	"0x0039",
	"0x003A",
	"0x003B",
	"0x003E",
	"0x003F",
	"0x0040",
	"0x0041",
	"0x0042",
	"0x0043",
	"0x0044",
	"0x0045",
	};
	assert(i >= 0 && i < 45);
	return _device_type_hex[i];
}

#define _gbk_to_utf8(buf, src)	fl_utf8from_mb(buf, buf_len, src, strlen(src))

char* access_str(int i)
{
	static char _access_read[buf_len];
	static char _access_write[buf_len];
	static char _access_any[buf_len];
	static char _access_both[buf_len];
	static char* _access_str_def[] = {
		_access_any,
		_access_read,
		_access_write,
		_access_both
	};
	static int first = 1;
	if (first)
	{
		first = 0;
		_gbk_to_utf8(_access_any, "任意");
		_gbk_to_utf8(_access_read, "只读");
		_gbk_to_utf8(_access_write, "只写");
		_gbk_to_utf8(_access_both, "读写");
	}
	assert(i >= 0 && i <= 3);
	return _access_str_def[i];
}

char* method_str(int i) 
{
	static char* _method_str_def[] = {
		"buffered",
		"in direct",
		"out direct",
		"neither",
	};
	assert(i >= 0 && i <= 3);
	return _method_str_def[i];
}

char* hex_str_0x(int i)
{
	static char* _hex_str_0x[] = {
		"0x0",
		"0x1",
		"0x2",
		"0x3",
	};
	assert(i >= 0 && i <= 3);
	return _hex_str_0x[i];
}

int ctoi(char ch)
{
	if (ch >= '0' && ch <= '9')
		return ch - '0';
	if (ch >= 'a' && ch <= 'f')
		return ch - 'a' + 10;
	if (ch >= 'A' && ch <= 'F')
		return ch - 'A' + 10;
	return 0;
}

unsigned long make_ioctl_code(unsigned long device_type, unsigned long function, unsigned long method, unsigned long access)
{
	return (((device_type) << 16) | ((access) << 14) | ((function) << 2) | (method));
}
