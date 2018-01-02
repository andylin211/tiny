#ifndef _tinyshell_h_
#define _tinyshell_h_

#include "tinystr.h"
#include <Windows.h>

#ifdef __cplusplus
extern "C" {
#endif

	typedef void(*exec_script_callback)(char* str, void* data);

	void exec_script_file(char* bash, char* script, char* args, exec_script_callback cb, void* data);

	void exec_script_str(char* bash, char* script, exec_script_callback cb, void* data);

#ifdef define_tiny_here

	static int create_pipe(HANDLE* read, HANDLE* write)
	{
		SECURITY_ATTRIBUTES sa;

		ZeroMemory(&sa, sizeof(sa));
		sa.nLength = sizeof(sa);
		sa.bInheritHandle = TRUE;
		sa.lpSecurityDescriptor = NULL;
		return CreatePipe(read, write, &sa, 0);
	}

	static int create_process(char* cmd, HANDLE write, HANDLE read, HANDLE* process)
	{
		BOOL ret = 0;
		STARTUPINFOA startup_info;
		PROCESS_INFORMATION process_info;

		ZeroMemory(&startup_info, sizeof(startup_info));
		startup_info.cb = sizeof(startup_info);
		startup_info.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
		startup_info.hStdOutput = write;
		startup_info.hStdError = write;
		startup_info.hStdInput = read;
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

		CloseHandle(process_info.hThread);
		*process = process_info.hProcess;
		return ret;
	}

	static int read_pipe(HANDLE process, HANDLE read, char** buffer)
	{
		DWORD nread = 0;
		DWORD navail = 0;
		DWORD nremain = 0;
		BOOL ret = 0;
		DWORD wait_ret = WaitForSingleObject(process, 10);

		// 获取管道内可读内容大小
		ret = PeekNamedPipe(read, NULL, 0, &nread, &navail, &nremain);
		if (ret && (navail != 0))
		{
			*buffer = (char*)safe_malloc(navail + 1);
			ZeroMemory(*buffer, navail);

			ReadFile(read, *buffer, navail, &nread, NULL);
			(*buffer)[nread] = '\0';
		}

		if (wait_ret != WAIT_TIMEOUT)
			return 0;

		return 1;
	}

	static void write_pipe(HANDLE write, char* str)
	{
		DWORD written = 0;
		WriteFile(write, str, strlen(str), &written, 0);
	}

	void exec_script_file(char* bash, char* script, char* args, exec_script_callback cb, void* data)
	{
		char cmd[256];
		wchar_t* wstr = 0;
		char* str = 0;
		HANDLE hread = NULL;
		HANDLE hwrite = NULL;
		HANDLE hprocess = NULL;
		char* buffer = 0;
		int ret = 0;

		str_format(cmd, 256, "\"%s\" \"%s\" \"%s\"", bash, script, args);

		do
		{
			if (!create_pipe(&hread, &hwrite))
				break;

			if (!create_process(cmd, hwrite, NULL, &hprocess))
				break;
			
			do
			{
				ret = read_pipe(hprocess, hread, &buffer);
				if (buffer)
				{
					if (cb)
					{
						wstr = str_to_wcs(buffer, -1, encoding_ansi);
						str = wcs_to_str(wstr, -1, encoding_utf8);
						cb(str, data);
						free(str);
						free(wstr);
					}
					else
						printf(buffer);

					free(buffer);
					buffer = 0;
				}
			} while (ret);
		} while (0);

		if (hread)
			CloseHandle(hread);

		if (hwrite)
			CloseHandle(hwrite);

		if (hprocess)
			CloseHandle(hprocess);
	}

	void exec_script_str(char* bash, char* script, exec_script_callback cb, void* data)
	{
		wchar_t* wstr = 0;
		char* str = 0;
		HANDLE hread = NULL;
		HANDLE hwrite = NULL;
		HANDLE hread1 = NULL;
		HANDLE hwrite1 = NULL;
		HANDLE hprocess = NULL;
		char* buffer = 0;
		int ret = 0;

		do
		{
			if (!create_pipe(&hread, &hwrite))
				break;

			if (!create_pipe(&hread1, &hwrite1))
				break;

			write_pipe(hwrite1, script);

			CloseHandle(hwrite1);

			if (!create_process(bash, hwrite, hread1, &hprocess))
				break;
			
			do
			{
				ret = read_pipe(hprocess, hread, &buffer);
				if (buffer)
				{
					if (cb)
					{
						wstr = str_to_wcs(buffer, -1, encoding_ansi);
						str = wcs_to_str(wstr, -1, encoding_utf8);
						cb(str, data);
						free(str);
						free(wstr);
					}
					else
						printf(buffer);

					free(buffer);
					buffer = 0;
				}
			} while (ret);
		} while (0);

		if (hwrite)
			CloseHandle(hwrite);

		if (hread)
			CloseHandle(hread);

		if (hread1)
			CloseHandle(hread1);

		if (hprocess)
			CloseHandle(hprocess);
	}

#endif

#ifdef __cplusplus
}
#endif

#endif
