#include "tinylog.h"
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <io.h>
#include <windows.h>
#include "tinystr.h"
#include <stdlib.h>

#define target_file "C:\\tinylog.txt"

#define code_page CP_ACP

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

void tinylog(wchar_t* msg, ...)
{
	FILE* file = 0;
	wchar_t *buf = 0;
	wchar_t *buf2 = 0;
	va_list msg_args;
	SYSTEMTIME st = { 0 };	

	if (!msg)
		return;

	va_start(msg_args, msg);
	buf = wcs_vformat_large(msg, msg_args);
	va_end(msg_args);
	
	GetLocalTime(&st);
	buf2 = wcs_format_large(L"[%04u%02u%02u %02u:%02u:%02u.%03u]%s\r\n", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, buf);

	write_string(stdout, buf2);

	file = fopen(target_file, "ab");
	if (!file)
		return;
	
	write_string(file, buf2);
	fclose(file);
	free(buf);
	free(buf2);
}

void tinylog_dot()
{
	FILE* file = 0;
	file = fopen(target_file, "ab");
	if (!file)
		return;

	write_string(file, L".");
	write_string(stdout, L".");
	fclose(file);
}

#ifdef tinylog_test

int main()
{
	int i = 0;
	tinylog(L"hello world!");
	tinylog(L"hello world!%s %d ====", L"what ’ƒÕ∏ﬂŒ¬", 123);
	for (i = 0; i < 100; i++)
		tinylog_dot();
	return 0;
}

#endif
