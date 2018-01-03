#ifndef _tinyshell_h_
#define _tinyshell_h_

#include "tinystr.h"
#include <Windows.h>

#ifdef __cplusplus
extern "C" {
#endif

	typedef void(*exec_script_callback)(wchar_t* output, wchar_t* error, void* data);

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

	static int create_process(char* cmd, HANDLE hwrite_output, HANDLE hwrite_error, HANDLE hread_input, HANDLE* process)
	{
		BOOL ret = 0;
		STARTUPINFOA startup_info;
		PROCESS_INFORMATION process_info;

		ZeroMemory(&startup_info, sizeof(startup_info));
		startup_info.cb = sizeof(startup_info);
		startup_info.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
		startup_info.hStdOutput = hwrite_output;
		startup_info.hStdError = hwrite_error;
		startup_info.hStdInput = hread_input;
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

	static int read_pipe(HANDLE process, HANDLE hread, char** buffer)
	{
		DWORD nread = 0;
		DWORD navail = 0;
		DWORD nremain = 0;
		BOOL ret = 0;
		DWORD wait_ret = WaitForSingleObject(process, 10);

		// 获取管道内可读内容大小
		ret = PeekNamedPipe(hread, NULL, 0, &nread, &navail, &nremain);
		if (ret && (navail != 0))
		{
			*buffer = (char*)safe_malloc(navail + 1);
			ZeroMemory(*buffer, navail);

			ReadFile(hread, *buffer, navail, &nread, NULL);
			(*buffer)[nread] = '\0';
		}

		if (wait_ret != WAIT_TIMEOUT)
			return 0;

		return 1;
	}

	static void write_pipe(HANDLE hwrite, char* buffer)
	{
		DWORD written = 0;
		WriteFile(hwrite, buffer, strlen(buffer), &written, 0);
	}

	static int response(HANDLE hprocess, HANDLE hread, int output, exec_script_callback cb, void* data)
	{
		char* str = 0;
		wchar_t* wstr = 0;
		char* buffer = 0;
		int ret = read_pipe(hprocess, hread, &buffer);
		if (buffer)
		{
			if (cb)
			{
				wstr = str_to_wcs(buffer, -1, encoding_ansi);
				output ? cb(wstr, 0, data) : cb(0, wstr, data);
				free(wstr);
			}
			else
				fprintf(output ? stdout : stderr, buffer);

			free(buffer);
			buffer = 0;
		}
		return ret;
	}

	static int output_response(HANDLE hprocess, HANDLE hread, exec_script_callback cb, void* data)
	{
		return response(hprocess, hread, 1, cb, data);
	}

	static int error_response(HANDLE hprocess, HANDLE hread, exec_script_callback cb, void* data)
	{
		return response(hprocess, hread, 0, cb, data);
	}

	void exec_script_file(char* bash, char* file, char* args, exec_script_callback cb, void* data)
	{
		char cmd[256];
		HANDLE hread_output = NULL;
		HANDLE hwrite_output = NULL;
		HANDLE hread_error = NULL;
		HANDLE hwrite_error = NULL;
		HANDLE hprocess = NULL;
		int ret1 = 0;
		int ret2 = 0;

		str_format(cmd, 256, "\"%s\" \"%s\" \"%s\"", bash, file, args);

		do
		{
			if (!create_pipe(&hread_output, &hwrite_output))
				break;

			if (!create_pipe(&hread_error, &hwrite_error))
				break;

			if (!create_process(cmd, hwrite_output, hwrite_error, NULL, &hprocess))
				break;
			
			do
			{
				ret1 = output_response(hprocess, hread_output, cb, data);
				ret2 = error_response(hprocess, hread_error, cb, data);
			} while (ret1 || ret2);
		} while (0);

		if (hread_output)
			CloseHandle(hread_output);

		if (hread_error)
			CloseHandle(hread_error);

		if (hwrite_output)
			CloseHandle(hwrite_output);

		if (hwrite_error)
			CloseHandle(hwrite_error);

		if (hprocess)
			CloseHandle(hprocess);
	}

	void exec_script_str(char* bash, char* script, exec_script_callback cb, void* data)
	{
		HANDLE hread_output = NULL;
		HANDLE hwrite_output = NULL;
		HANDLE hread_error = NULL;
		HANDLE hwrite_error = NULL;
		HANDLE hread_input = NULL;
		HANDLE hwrite_input = NULL;
		HANDLE hprocess = NULL;
		int ret1 = 0;
		int ret2 = 0;

		do
		{
			if (!create_pipe(&hread_output, &hwrite_output))
				break;

			if (!create_pipe(&hread_error, &hwrite_error))
				break;

			if (!create_pipe(&hread_input, &hwrite_input))
				break;

			write_pipe(hwrite_input, script);

			CloseHandle(hwrite_input);

			if (!create_process(bash, hwrite_output, hwrite_error, hread_input, &hprocess))
				break;
			
			do
			{
				ret1 = output_response(hprocess, hread_output, cb, data);
				ret2 = error_response(hprocess, hread_error, cb, data);
			} while (ret1 || ret2);
		} while (0);

		if (hread_output)
			CloseHandle(hread_output);

		if (hread_error)
			CloseHandle(hread_error);

		if (hwrite_output)
			CloseHandle(hwrite_output);

		if (hwrite_error)
			CloseHandle(hwrite_error);

		if (hread_input)
			CloseHandle(hread_input);

		if (hprocess)
			CloseHandle(hprocess);
	}

#endif

#ifdef __cplusplus
}
#endif

#endif
