#include "tinystr.h"
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <Windows.h>
#include <assert.h>
#include <stdlib.h>

#define max_buf_size 512

void* safe_malloc(int size)
{
	void* p = malloc(size);
	assert(p);
	memset(p, 0, size);
	return p;
}

wchar_t* str_to_wcs(char* string, int size, str_encoding encoding)
{
	wchar_t* str = 0;
	unsigned int code_page = 0;
	int len = 0;

	if (!string || size < -1)// || (int)strlen(string) < size) /*bug: 开启页堆才报错的！ gflags /p /enable ioctlfuzz.exe /full */
		return 0;

	code_page = (encoding == encoding_utf8) ? CP_UTF8 : CP_ACP;

	len = MultiByteToWideChar(code_page, 0, string, size, NULL, 0);
	str = (wchar_t*)safe_malloc(sizeof(wchar_t) * (len + 1));
	MultiByteToWideChar(code_page, 0, string, size, str, len);

	return str;
}

char* wcs_to_str(wchar_t* text, int size, str_encoding encoding)
{
	unsigned int code_page = 0;
	char* str = 0;
	int len = 0;

	if (!text || size < -1)//bug: || (int)wcslen(text) < size)
		return 0;

	code_page = (encoding == encoding_utf8) ? CP_UTF8 : CP_ACP;

	len = WideCharToMultiByte(code_page, 0, text, size, NULL, 0, NULL, NULL);
	str = (char*)safe_malloc(len + 1);
	WideCharToMultiByte(code_page, 0, text, size, str, len, NULL, NULL);

	return str;
}

int str_endwith(char* string, char* tail)
{
	int len1 = 0;
	int len2 = 0;

	if (!string || !tail)
		return 0;

	len1 = strlen(string);
	len2 = strlen(tail);

	if (len1 < len2)
		return 0;

	return (0 == strcmp(&(string[len1 - len2]), tail));
}

int str_beginwith(char* string, char* head)
{
	int len1 = 0;
	int len2 = 0;

	if (!string || !head)
		return 0;

	len1 = strlen(string);
	len2 = strlen(head);

	if (len1 < len2)
		return 0;

	return (0 == strncmp(string, head, len2));
}

int wcs_endwith(wchar_t* string, wchar_t* tail)
{
	int len1 = 0;
	int len2 = 0;

	if (!string || !tail)
		return 0;

	len1 = wcslen(string);
	len2 = wcslen(tail);

	if (len1 < len2)
		return 0;

	return (0 == wcscmp(&(string[len1 - len2]), tail));
}

int wcs_beginwith(wchar_t* string, wchar_t* head)
{
	int len1 = 0;
	int len2 = 0;

	if (!string || !head)
		return 0;

	len1 = wcslen(string);
	len2 = wcslen(head);

	if (len1 < len2)
		return 0;

	return (0 == wcsncmp(string, head, len2));
}

char* str_format(char* buffer, int size, char* format, ...)
{
	va_list args;

	if (!buffer || !size)
		return 0;

	va_start(args, format);
	_vsnprintf_s(buffer, size, size, format, args);
	va_end(args);

	return buffer;
}

wchar_t* wcs_format(wchar_t* buffer, int size, wchar_t* format, ...)
{
	va_list args;

	if (!buffer || !size)
		return 0;

	va_start(args, format);
	_vsnwprintf_s(buffer, size, size, format, args);
	va_end(args);

	return buffer;
}

char* str_vformat_large(char* format, va_list arg_list)
{
	char* buffer = 0;
	int len = max_buf_size;

	buffer = (char*)safe_malloc((len + 1) * sizeof(char));
	while (-1 == _vsnprintf(buffer, len, format, arg_list))
	{
		free(buffer);
		len *= 2;
		buffer = (char*)safe_malloc((len + 1) * sizeof(char));
	}

	return buffer;
}


char* str_format_large(char* format, ...)
{
	char* buffer = 0;
	va_list args;
	int len = max_buf_size;

	va_start(args, format);
	buffer = (char*)safe_malloc((len + 1) * sizeof(char));
	while (-1 == _vsnprintf(buffer, len, format, args))
	{
		free(buffer);
		len *= 2;
		buffer = (char*)safe_malloc((len + 1) * sizeof(char));
	}
	va_end(args);

	return buffer;
}

wchar_t* wcs_vformat_large(wchar_t* format, va_list arg_list)
{
	wchar_t* buffer = 0;
	int len = max_buf_size;

	buffer = (wchar_t*)safe_malloc((len + 1) * sizeof(wchar_t));
	while (-1 == _vsnwprintf(buffer, len, format, arg_list))
	{
		free(buffer);
		len *= 2;
		buffer = (wchar_t*)safe_malloc((len + 1) * sizeof(wchar_t));
	}
	
	return buffer;
}


wchar_t* wcs_format_large(wchar_t* format, ...)
{
	wchar_t* buffer = 0;
	va_list args;
	int len = max_buf_size;

	va_start(args, format);
	buffer = (wchar_t*)safe_malloc((len + 1) * sizeof(wchar_t));
	while (-1 == _vsnwprintf(buffer, len, format, args))
	{
		free(buffer);
		len *= 2;
		buffer = (wchar_t*)safe_malloc((len + 1) * sizeof(wchar_t));
	}
	va_end(args);

	return buffer;
}

int wcs_find(wchar_t* string, wchar_t ch)
{
	int i = 0;

	if (!string)
		return -1;

	for (i = 0; i < (int)wcslen(string); i++)
	{
		if (string[i] == ch)
			return i;
	}

	return -1;
}


int str_find(char* string, char ch)
{
	int i = 0;

	if (!string)
		return -1;

	for (i = 0; i < (int)strlen(string); i++)
	{
		if (string[i] == ch)
			return i;
	}

	return -1;
}

int wcs_find_from(wchar_t* string, wchar_t ch, int from)
{
	int i = 0;

	if (!string || from < 0)
		return -1;

	for (i = from; i < (int)wcslen(string); i++)
	{
		if (string[i] == ch)
			return i;
	}

	return -1;
}

int wcs_rfind(wchar_t* string, wchar_t ch)
{
	int i = 0;

	if (!string)
		return -1;

	for (i = wcslen(string) - 1; i >= 0; i--)
	{
		if (string[i] == ch)
			return i;
	}

	return -1;
}

int str_rfind(char* string, char ch)
{
	int i = 0;

	if (!string)
		return -1;

	for (i = strlen(string) - 1; i >= 0; i--)
	{
		if (string[i] == ch)
			return i;
	}

	return -1;
}

int wcs_rfind_from(wchar_t* string, wchar_t ch, int from)
{
	int i = 0;
	int len = 0;

	if (!string || from < 0)
		return -1;

	len = wcslen(string);

	for (i = (from < len) ? from : len - 1; i >= 0; i--)
	{
		if (string[i] == ch)
			return i;
	}

	return -1;
}

/*
* from 0~len
* to -1~no limit
*/
wchar_t* wcs_erase(wchar_t* string, int from, int to)
{
	int i = 0;
	int len = 0;
	
	if (!string)
		return 0;

	len = wcslen(string);

	if (from < 0 || to < -1 || from > len)
		return string;	

	if (to == -1)
	{
		string[from] = 0;
	}
	else if (from <= to)
	{
		for (i = 0; i < len - to; i++)
		{
			string[from + i] = string[to + i + 1];
		}
		string[from + i] = 0;
	}

	return string;
}

/*
* from 0~len
* to -1~no limit
*/
char* str_erase(char* string, int from, int to)
{
	int i = 0;
	int len = 0;

	if (!string)
		return 0;

	len = strlen(string);

	if (from < 0 || to < -1 || from > len)
		return string;

	if (to == -1)
	{
		string[from] = 0;
	}
	else if (from <= to)
	{
		for (i = 0; i < len - to; i++)
		{
			string[from + i] = string[to + i + 1];
		}
		string[from + i] = 0;
	}

	return string;
}


wchar_t* wcs_remove(wchar_t* string, wchar_t ch)
{
	int i = 0;

	if (!string)
		return 0;

	while (1)
	{
		i = wcs_find_from(string, ch, i);
		if (i != -1)
			wcs_erase(string, i, i);
		else
			break;
	}

	return string;
}

int is_white_space(char ch)
{
	return (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r');
}

int is_white_spacew(wchar_t ch)
{
	return (ch == L' ' || ch == L'\t' || ch == L'\n' || ch == L'\r');
}


char* str_trim_end(char* string)
{
	int i = 0;
	if (!string)
		return 0;

	for (i = strlen(string) - 1; i >= 0; i--)
	{
		if (!is_white_space(string[i]))
			break;
		string[i] = 0;
	}
	return string;
}

wchar_t* wcs_trim_end(wchar_t* string)
{
	int i = 0;
	if (!string)
		return 0;

	for (i = wcslen(string) - 1; i >= 0; i--)
	{
		if (!is_white_spacew(string[i]))
			break;
		string[i] = 0;
	}
	return string;
}

int wcs_find_sub_from(wchar_t* string, wchar_t* sub, int from)
{
	int i = from;
	int len = 0;

	if (!string || !sub || from < 0)
		return -1;

	len = wcslen(sub);

	for (;;)
	{
		i = wcs_find_from(string, sub[0], i);
		if (-1 == i)
			break;

		if (0 == wcsncmp(&string[i], sub, len))
			return i;

		i++;
	}

	return -1;
}

int wcs_find_sub_count(wchar_t* string, wchar_t* sub)
{
	int i = 0;
	int pos = 0;
	int len = 0;

	if (!string || !sub)
		return 0;

	len = wcslen(sub);

	for (;;)
	{
		pos = wcs_find_sub_from(string, sub, pos);
		if (-1 == pos)
			break;

		i++;
		pos += len;
	}

	return i;
}

wchar_t* wcs_replace(wchar_t* string, wchar_t* sub, wchar_t* to)
{
	int i = 0;
	int count = 0;
	int len1 = 0;
	int len2 = 0;
	wchar_t* buf = 0;
	int buf_len = 0;
	int pos = 0;
	int last_pos = 0;

	if (!string || !sub || !to)
		return 0;

	len1 = wcslen(sub);
	len2 = wcslen(to);

	count = wcs_find_sub_count(string, sub);
	if (!count)
		return 0;

	buf_len = wcslen(string) + (len2 - len1) * count + 1;
	buf = (wchar_t*)safe_malloc(sizeof(wchar_t) * buf_len);

	for (i = 0; i < count; i++)
	{
		last_pos = pos;
		pos = wcs_find_sub_from(string, sub, last_pos);

		wcsncat(buf, &string[last_pos], pos - last_pos);
		wcscat(buf, to);

		pos += len1;
	}
	wcscat(buf, &string[pos]);

	return buf;
}

#ifdef _tinystr_test_
int main()
{
	/*wchar_t str1[] = L"bacdsfsdfsd";
	wchar_t str2[] = L"bacdsfsdfsd";
	wchar_t str3[] = L"bacdsfsdfsd";
	wchar_t str4[] = L"hello world";
	wprintf(L"%s\n", wcs_erase(str1, 0, -1));
	wprintf(L"%s\n", wcs_erase(str2, 5, 8));
	wprintf(L"%s\n", wcs_erase(str3, 10, 11));
	wprintf(L"%s\n", wcs_remove(str4, L'o'));*/
	wchar_t* str = wcs_replace(L"hello world, yellow", L"llo", L"&&");
	free(str);

	return 0;
}
#endif