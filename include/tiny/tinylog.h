#ifndef _tinylog_h_
#define _tinylog_h_
#include <time.h>
#include <io.h>
#include "tinystr.h"

#ifdef __cplusplus
extern "C" {
#endif

	void log_debug(wchar_t* msg, ...);

	void log_print(wchar_t* msg, ...);

	void log_error(wchar_t* msg, ...);

	extern void log_debuga(char* msg, ...);

	extern void log_printa(char* msg, ...);

	extern void log_errora(char* msg, ...);

#ifdef define_tiny_here

#define target_file "C:\\tinylog.txt"

#define code_page CP_ACP

#pragma warning(push)
#pragma warning(disable:4996)
	static void lock_file(FILE *file)
	{
		OVERLAPPED overlapped = { 0 };
		HANDLE handle = (HANDLE)_get_osfhandle(_fileno(file));
		LockFileEx(handle, LOCKFILE_EXCLUSIVE_LOCK, 0, -1, -1, &overlapped);
	}

	static void unlock_file(FILE *file)
	{
		OVERLAPPED overlapped = { 0 };
		HANDLE handle = (HANDLE)_get_osfhandle(_fileno(file));
		UnlockFileEx(handle, 0, -1, -1, &overlapped);
	}

	static void write_string(FILE *file, wchar_t *string)
	{
		char* str = wcs_to_str(string, -1, encoding_ansi);

		lock_file(file);
		fwrite(str, strlen(str), 1, file);
		unlock_file(file);

		free(str);
	}

	static void write_stringa(FILE *file, char *string)
	{
		lock_file(file);
		fwrite(string, strlen(string), 1, file);
		unlock_file(file);
	}

	static int _log_flag = 0;

	static void _log(int error, int file, int output, wchar_t* msg, va_list msg_args)
	{
		FILE* fp = 0;
		wchar_t *buf = 0;
		wchar_t *buf2 = 0;
		SYSTEMTIME st = { 0 };

		if (!msg)
			return;

		buf = wcs_vformat_large(msg, msg_args);

		if (error)
		{
			write_string(stderr, buf);
			write_string(stderr, L"\r\n");
		}


		if (output)
		{
			write_string(stdout, buf);
			write_string(stdout, L"\r\n");
		}

		if (file)
		{
			GetLocalTime(&st);
			buf2 = wcs_format_large(L"[%04u%02u%02u %02u:%02u:%02u.%03u]%s\r\n", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, buf);
			if (!_log_flag)
			{
				++_log_flag;
				fp = fopen(target_file, "wb");
			}
			else
				fp = fopen(target_file, "ab");
			if (fp)
			{
				write_string(fp, buf2);
				fclose(fp);
			}
			free(buf2);
		}

		free(buf);
	}

	static void _loga(int error, int file, int output, char* msg, va_list msg_args)
	{
		FILE* fp = 0;
		char *buf = 0;
		char *buf2 = 0;
		SYSTEMTIME st = { 0 };

		if (!msg)
			return;

		buf = str_vformat_large(msg, msg_args);

		if (error)
		{
			write_stringa(stderr, buf);
			write_stringa(stderr, "\r\n");
		}


		if (output)
		{
			write_stringa(stdout, buf);
			write_stringa(stdout, "\r\n");
		}

		if (file)
		{
			GetLocalTime(&st);
			buf2 = str_format_large("[%04u%02u%02u %02u:%02u:%02u.%03u]%s\r\n", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, buf);
			if (!_log_flag)
			{
				++_log_flag;
				fp = fopen(target_file, "wb");
			}
			else
				fp = fopen(target_file, "ab");
			if (fp)
			{
				write_stringa(fp, buf2);
				fclose(fp);
			}
			free(buf2);
		}

		free(buf);
	}

	void log_debug(wchar_t* msg, ...)
	{
		va_list args;
		va_start(args, msg);
		_log(0, 1, 0, msg, args);
		va_end(args);
	}

	void log_print(wchar_t* msg, ...)
	{
		va_list args;
		va_start(args, msg);
		_log(0, 1, 1, msg, args);
		va_end(args);
	}

	void log_error(wchar_t* msg, ...)
	{
		va_list args;
		va_start(args, msg);
		_log(1, 1, 0, msg, args);
		va_end(args);
	}

	void log_debuga(char* msg, ...)
	{
		va_list args;
		va_start(args, msg);
		_loga(0, 1, 0, msg, args);
		va_end(args);
	}


	void log_printa(char* msg, ...)
	{
		va_list args;
		va_start(args, msg);
		_loga(0, 1, 1, msg, args);
		va_end(args);
	}

	void log_errora(char* msg, ...)
	{
		va_list args;
		va_start(args, msg);
		_loga(1, 1, 0, msg, args);
		va_end(args);
	}

#pragma warning(pop)

#endif // define_tiny_here

#ifdef __cplusplus
}
#endif


#endif // _tinylog_h_
