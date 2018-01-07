#ifndef _tinysys_h_
#define _tinysys_h_

#include <Windows.h>
#pragma warning(push)
#pragma warning(disable:4091)
#include <ShlObj.h>
#pragma warning(pop)
#include <Shlwapi.h>
#include "tinystr.h"
#include "tinylog.h"

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
	void launch(char* file, char* args, int wait);
	void current_dir(char* buf, int size);
	int write_raw_file(char* file, char* buf, int len);
	int read_raw_file(char* file, char** buf);

#ifdef define_tiny_here
#pragma warning(push)
#pragma warning(disable:4996)
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
		ofn.lpstrTitle = L"保存为";
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

	void launch(char* file, char* args, int wait)
	{
		BOOL ret = 0;
		STARTUPINFOA startup_info;
		PROCESS_INFORMATION process_info;
		char* cmd = 0;

		if (!file || !args)
		{
			log_errora("lauch: invalid parameters");
			return;
		}

		cmd = str_format_large("\"%s\" %s", file, args);
		log_debuga("cmd: %s", cmd);

		ZeroMemory(&startup_info, sizeof(startup_info));
		startup_info.cb = sizeof(startup_info);
		startup_info.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
		startup_info.wShowWindow = SW_HIDE;

		ZeroMemory(&process_info, sizeof(process_info));
		ret = CreateProcessA(
			NULL,		// 不在此指定可执行文件的文件名  
			cmd,		// 命令行参数，不能是const，否则会crash！
			NULL,		// 默认进程安全性  
			NULL,		// 默认进程安全性  
			TRUE,		// 指定当前进程内句柄可以被子进程继承  
			0,			// 不为新进程创建一个新的控制台窗口  
			NULL,		// 使用本进程的环境变量  
			NULL,		// 使用本进程的驱动器和目录  
			&startup_info,
			&process_info);
		if (!ret)
		{
			log_errora("fail to create process: %s, %d", cmd, GetLastError());
		}

		CloseHandle(process_info.hThread);

		if (wait)
		{
			log_debuga("wait for child process");
			WaitForSingleObject(process_info.hProcess, -1);
			log_debuga("wait done");
		}
		
		CloseHandle(process_info.hProcess);
		free(cmd);
	}

	void current_dir(char* buf, int size)
	{
		unsigned ret = 0;
		if (!buf || size <= 0)
		{
			log_errora("current_dir invalid parameters");
			return;
		}

		if (GetModuleFileNameA(NULL, buf, size))
		{
			str_replace_char(buf, '\\', '/');
			str_erase(buf, str_rfind(buf, '/'), -1);
		}
		else
			log_errora("fail to get module file name: %d", GetLastError());
	}

	int write_raw_file(char* file, char* buf, int len)
	{
		FILE* fp = 0;
		int size = 0;

		if (!file || !buf || len <= 0)
		{
			log_errora("write raw file invalid parameters");
			return 0;
		}
		
		fp = fopen(file, "wb");
		if (!fp)
		{
			log_errora("fail to write file: %s, %d", file, GetLastError());
			return 0;
		}

		size = fwrite(buf, len, 1, fp);
		if (!size)
			log_errora("fail to write content to file %s: %d", file, GetLastError());

		fclose(fp);
		return size;
	}

	int read_raw_file(char* file, char** buf)
	{
		FILE* fp = 0;
		int len = 0;
		if (!file || !buf)
		{
			log_errora("read raw file invalid parameters");
			return 0;
		}

		fp = fopen(file, "rb");
		if (!fp)
		{
			log_errora("fail to read file: %s, %d", file, GetLastError());
			return 0;
		}

		fseek(fp, 0, SEEK_END);
		len = ftell(fp);
		if (!len)
		{
			log_errora("fail to ftell: %s, %d", file, GetLastError());
			return 0;
		}

		*buf = (char*)safe_malloc(len + 1);

		fseek(fp, 0, SEEK_SET);

		fread(*buf, len, 1, fp);

		fclose(fp);

		return len;
	}

#pragma warning(pop)
#endif // define_tiny_here


#ifdef __cplusplus
}
#endif

#endif //_tinysys_h_

