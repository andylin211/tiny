#pragma once

#include "FL\fl_utf8.h"
#include <Shlwapi.h>
#include <ShlObj.h>
#include <assert.h>

#define buf_len 512

/* 一般只在某个位置调用，所以inline也没什么影响 */

inline char* label_conv(char* gbk) {
	static char buf[buf_len];
	fl_utf8from_mb(buf, buf_len, gbk, strlen(gbk));
	return buf;
}

inline void RunAs(LPCSTR _param)
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

inline int is_evevated()
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

inline int has_been_running()
{
	DWORD error = 0;
	HANDLE mutex = CreateMutex(NULL, FALSE, L"8130DAD1-4E8D-43C4-8A49-70C242533BBD");
	error = GetLastError();
	if (ERROR_ALREADY_EXISTS == error || ERROR_ACCESS_DENIED == error)
		return 1;
	return 0;
}

inline int is_xp()
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

inline wchar_t* ask_open_file(HWND hwnd)
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

inline wchar_t* ask_save_file(HWND hwnd)
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

inline wchar_t* ask_open_path(HWND hwnd)
{
	static wchar_t path[buf_len] = { 0 };
	BROWSEINFO bi = { 0 };

	bi.hwndOwner = hwnd;
	bi.pszDisplayName = path;
	bi.ulFlags = BIF_NEWDIALOGSTYLE;
	LPITEMIDLIST idl = SHBrowseForFolder(&bi);
	return SHGetPathFromIDList(idl, path) ? path : 0;
};

inline char* read_file_raw(wchar_t* file)
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

#define set_label_font_12(w) w->labelsize(12);\
		w->labelfont(FL_HELVETICA)

inline void set_menu_label(char* utf8, char* gbk)
{
	fl_utf8from_mb(utf8, buf_len, gbk, strlen(gbk));
}


inline wchar_t* type_str(wchar_t* str)
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

inline wchar_t* access_str(wchar_t* str)
{
	if (!str)
		return 0;

	if (!wcscmp(L"FILE_READ_ACCESS", str))
		return L"读";

	if (!wcscmp(L"FILE_WRITE_ACCESS", str))
		return L"写";

	if (!wcscmp(L"FILE_ANY_ACCESS", str))
		return L"任意";

	wchar_t buf[buf_len];
	wcsncpy(buf, str, buf_len);
	wcs_remove(buf, L' ');
	if (!wcscmp(L"FILE_READ_ACCESS|FILE_WRITE_ACCESS", buf))
		return L"读写";

	return 0;
}

inline wchar_t* method_str(wchar_t* str)
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