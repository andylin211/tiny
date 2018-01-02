#ifndef _tinysys_h_
#define _tinysys_h_

#include <Windows.h>
#pragma warning(push)
#pragma warning(disable:4091)
#include <ShlObj.h>
#pragma warning(pop)
#include <Shlwapi.h>
#include "tinystr.h"

#ifdef __cplusplus
extern "C" {
#endif

	int run_as();
	int is_elevated();
	int has_been_running(wchar_t* app_guid);
	int is_xp();
	wchar_t* ask_open_path(HWND hwnd);
	wchar_t* ask_save_file(HWND hwnd);
	wchar_t* ask_open_path(HWND hwnd);
	COORD cursor_position();
	void print_at(char* desc, COORD coord);

#ifdef define_tiny_here
	int run_as()
	{
		SHELLEXECUTEINFOA sei = { 0 };
		CHAR path[MAX_PATH] = { 0 };
		char args[256];
		GetModuleFileNameA(NULL, path, MAX_PATH);

		strcpy_s(args, 256, GetCommandLineA());
		str_erase(args, 0, str_find(args, ' '));

		sei.cbSize = sizeof(sei);
		sei.lpVerb = "runas";
		sei.lpFile = path;
		sei.nShow = SW_SHOWNORMAL;
		sei.lpParameters = args;
		ShellExecuteExA(&sei);
		return 0;
	}

	int is_elevated()
	{
		HANDLE hToken = NULL;
		BOOL bElevated = FALSE;
		TOKEN_ELEVATION tokenEle;
		DWORD dwRetLen = 0;

		if (is_xp())
			return 1;

		if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
			return 0;

		if (GetTokenInformation(hToken, TokenElevation, &tokenEle, sizeof(tokenEle), &dwRetLen))
			bElevated = tokenEle.TokenIsElevated;

		CloseHandle(hToken);
		hToken = NULL;

		return bElevated;
	}

	int has_been_running(wchar_t* app_guid)
	{
		DWORD error = 0;
		HANDLE mutex;
		if (!app_guid)
			return 0;

		mutex = CreateMutex(NULL, FALSE, app_guid);
		error = GetLastError();
		if (ERROR_ALREADY_EXISTS == error || ERROR_ACCESS_DENIED == error)
			return 1;
		return 0;
	}
#pragma warning(push)
#pragma warning(disable:4996)
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
#pragma warning(pop)

	wchar_t* ask_open_file(HWND hwnd)
	{
		static wchar_t file[256] = { 0 };
		OPENFILENAME ofn = { 0 };

		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = hwnd;
		ofn.nFilterIndex = 1;
		ofn.lpstrFile = file;
		ofn.nMaxFile = 256;
		ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;
		return GetOpenFileName(&ofn) ? file : 0;
	}

	wchar_t* ask_save_file(HWND hwnd)
	{
		static wchar_t file[256] = { 0 };
		OPENFILENAME ofn = { 0 };

		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = hwnd;
		ofn.nFilterIndex = 1;
		ofn.lpstrFile = file;
		ofn.nMaxFile = 256;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
		ofn.lpstrTitle = L"±£´æÎª";
		return GetSaveFileName(&ofn) ? file : 0;
	}

	wchar_t* ask_open_path(HWND hwnd)
	{
		static wchar_t path[256] = { 0 };
		BROWSEINFO bi = { 0 };

		bi.hwndOwner = hwnd;
		bi.pszDisplayName = path;
		bi.ulFlags = BIF_NEWDIALOGSTYLE;
		LPITEMIDLIST idl = SHBrowseForFolder(&bi);
		return SHGetPathFromIDList(idl, path) ? path : 0;
	};

	COORD cursor_position()
	{
		HANDLE hd = GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_SCREEN_BUFFER_INFO buf_info;
		GetConsoleScreenBufferInfo(hd, &buf_info);
		return buf_info.dwCursorPosition;
	}

	void print_at(char* desc, COORD coord)
	{
		HANDLE hd = GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_CURSOR_INFO info;
		GetConsoleCursorInfo(hd, &info);
		info.bVisible = 0;
		SetConsoleCursorInfo(hd, &info);
		SetConsoleCursorPosition(hd, coord);
		fprintf(stdout, desc);
		info.bVisible = 1;
		SetConsoleCursorInfo(hd, &info);
	}
	
#endif // define_tiny_here


#ifdef __cplusplus
}
#endif

#endif //_tinysys_h_

