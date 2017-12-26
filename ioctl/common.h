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