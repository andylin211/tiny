#ifndef _tinystr_h_
#define _tinystr_h_
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <Windows.h>
#include <assert.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

	typedef enum
	{
		encoding_ansi = 0,
		encoding_utf8
	}str_encoding;

	void* safe_malloc(int size);
	char* wcs_to_str(wchar_t* text, int size, str_encoding encoding);
	char* wcs_to_str_tmp(wchar_t* text, int size, str_encoding encoding);/*not thread-safe*/
	wchar_t* str_to_wcs(char* string, int size, str_encoding encoding);
	wchar_t* str_to_wcs_tmp(char* string, int size, str_encoding encoding);/*not thread-safe*/
	int str_endwith(char* string, char* tail);
	int str_beginwith(char* string, char* head);
	int wcs_endwith(wchar_t* string, wchar_t* tail);
	int wcs_beginwith(wchar_t* string, wchar_t* head);
	char* str_format(char* buffer, int size, char* format, ...);
	wchar_t* wcs_format(wchar_t* buffer, int size, wchar_t* format, ...);
	char* str_vformat_large(char* format, va_list arg_list);
	char* str_format_large(char* format, ...);
	wchar_t* wcs_vformat_large(wchar_t* format, va_list arg_list);
	wchar_t* wcs_format_large(wchar_t* format, ...);
	int wcs_find(wchar_t* string, wchar_t ch);
	int str_find(char* string, char ch);
	int wcs_find_from(wchar_t* string, wchar_t ch, int from);
	int str_find_from(char* string, char ch, int from);
	int str_rfind(char* string, char ch);
	int wcs_rfind(wchar_t* string, wchar_t ch);
	int wcs_rfind_from(wchar_t* string, wchar_t ch, int from);
	wchar_t* wcs_erase(wchar_t* string, int from, int to);
	char* str_erase(char* string, int from, int to);
	wchar_t* wcs_remove(wchar_t* string, wchar_t ch);
	char* str_remove(char* string, char ch);
	int is_white_space(char ch);
	int is_white_spacew(wchar_t ch);
	char* str_trim_end(char* string);
	wchar_t* wcs_trim_end(wchar_t* string);
	int wcs_find_sub_from(wchar_t* string, wchar_t* sub, int from);
	int wcs_find_sub_count(wchar_t* string, wchar_t* sub);
	wchar_t* wcs_replace(wchar_t* string, wchar_t* sub, wchar_t* to);
	char* str_replace(char* string, char* sub, char* to);
	int str_count(char* string, char ch);
	int wcs_count(wchar_t* string, wchar_t ch);
	char* gbk2utf8_tmp(char* string); /* not thread-safe */
	char* gbk2utf8(char* string);
	char* utf82gbk_tmp(char* string); /* not thread-safe */
	char* utf82gbk(char* string);
	void str_replace_char(char* string, char from, char to);
	void wcs_replace_char(wchar_t* string, wchar_t from, wchar_t to);

#ifdef define_tiny_here

#pragma warning(push)
#pragma warning(disable:4996)

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

	wchar_t* str_to_wcs_tmp(char* string, int size, str_encoding encoding)
	{
		static wchar_t buf[256] = { 0 };
		unsigned int code_page = 0;
		int len = 0;

		if (!string || size < -1)
			return buf;

		memset(buf, 0, 256 * sizeof(wchar_t));

		code_page = (encoding == encoding_utf8) ? CP_UTF8 : CP_ACP;

		len = MultiByteToWideChar(code_page, 0, string, size, NULL, 0);
		if (len > 256 - 1)
			return buf;

		MultiByteToWideChar(code_page, 0, string, size, buf, len);
		return buf;
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

	char* wcs_to_str_tmp(wchar_t* text, int size, str_encoding encoding)
	{
		unsigned int code_page = 0;
		static char buf[256] = { 0 };
		int len = 0;

		if (!text || size < -1)//bug: || (int)wcslen(text) < size)
			return 0;

		memset(buf, 0, 256);

		code_page = (encoding == encoding_utf8) ? CP_UTF8 : CP_ACP;

		len = WideCharToMultiByte(code_page, 0, text, size, NULL, 0, NULL, NULL);
		if (len > 256 - 1)
			return buf;

		WideCharToMultiByte(code_page, 0, text, size, buf, len, NULL, NULL);

		return buf;
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
		int len = 256;

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
		int len = 256;

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
		int len = 256;

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
		int len = 256;

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
		int len = (int)wcslen(string);
		if (!string)
			return -1;

		for (i = 0; i < len; i++)
		{
			if (string[i] == ch)
				return i;
		}

		return -1;
	}

	int str_find(char* string, char ch)
	{
		int i = 0;
		int len = (int)strlen(string);
		if (!string)
			return -1;

		for (i = 0; i < len; i++)
		{
			if (string[i] == ch)
				return i;
		}

		return -1;
	}

	int wcs_find_from(wchar_t* string, wchar_t ch, int from)
	{
		int i = 0;
		int len = (int)wcslen(string);
		if (!string || from < 0)
			return -1;

		for (i = from; i < len; i++)
		{
			if (string[i] == ch)
				return i;
		}

		return -1;
	}

	int str_find_from(char* string, char ch, int from)
	{
		int i = 0;
		int len = (int)strlen(string);
		if (!string || from < 0)
			return -1;

		for (i = from; i < len; i++)
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

	char* str_remove(char* string, char ch)
	{
		int i = 0;

		if (!string)
			return 0;

		while (1)
		{
			i = str_find_from(string, ch, i);
			if (i != -1)
				str_erase(string, i, i);
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

	int str_find_sub_from(char* string, char* sub, int from)
	{
		int i = from;
		int len = 0;

		if (!string || !sub || from < 0)
			return -1;

		len = strlen(sub);

		for (;;)
		{
			i = str_find_from(string, sub[0], i);
			if (-1 == i)
				break;

			if (0 == strncmp(&string[i], sub, len))
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

	int str_find_sub_count(char* string, char* sub)
	{
		int i = 0;
		int pos = 0;
		int len = 0;

		if (!string || !sub)
			return 0;

		len = strlen(sub);

		for (;;)
		{
			pos = str_find_sub_from(string, sub, pos);
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

	char* str_replace(char* string, char* sub, char* to)
	{
		int i = 0;
		int count = 0;
		int len1 = 0;
		int len2 = 0;
		char* buf = 0;
		int buf_len = 0;
		int pos = 0;
		int last_pos = 0;

		if (!string || !sub || !to)
			return 0;

		len1 = strlen(sub);
		len2 = strlen(to);

		count = str_find_sub_count(string, sub);
		if (!count)
			return 0;

		buf_len = strlen(string) + (len2 - len1) * count + 1;
		buf = (char*)safe_malloc(sizeof(char) * buf_len);

		for (i = 0; i < count; i++)
		{
			last_pos = pos;
			pos = str_find_sub_from(string, sub, last_pos);

			strncat(buf, &string[last_pos], pos - last_pos);
			strcat(buf, to);

			pos += len1;
		}
		strcat(buf, &string[pos]);

		return buf;
	}

	int str_count(char* string, char ch)
	{
		int count = 0;
		if (!string)
			return 0;

		while (*string)
		{
			if (ch == *string)
				count++;
			string++;
		}
		return count;
	}

	int wcs_count(wchar_t* string, wchar_t ch)
	{
		int count = 0;
		if (!string)
			return 0;

		while (*string)
		{
			if (ch == *string)
				count++;
			string++;
		}
		return count;
	}

	char* gbk2utf8_tmp(char* string)
	{
		wchar_t* wstr = str_to_wcs(string, -1, encoding_ansi);
		return wcs_to_str_tmp(wstr, -1, encoding_utf8);
	}

	char* utf82gbk_tmp(char* string)
	{
		wchar_t* wstr = str_to_wcs(string, -1, encoding_utf8);
		return wcs_to_str_tmp(wstr, -1, encoding_ansi);
	}

	char* gbk2utf8(char* string)
	{
		wchar_t* wstr = str_to_wcs(string, -1, encoding_ansi);
		return wcs_to_str(wstr, -1, encoding_utf8);
	}

	char* utf82gbk(char* string)
	{
		wchar_t* wstr = str_to_wcs(string, -1, encoding_utf8);
		return wcs_to_str(wstr, -1, encoding_ansi);
	}

	void str_replace_char(char* string, char from, char to)
	{
		while (*string)
		{
			if (*string == from)
				*string = to;
			string++;
		}
	}

	void wcs_replace_char(wchar_t* string, wchar_t from, wchar_t to)
	{
		while (*string)
		{
			if (*string == from)
				*string = to;
			string++;
		}
	}
#pragma warning(pop)

#endif // define_tiny_here

#ifdef __cplusplus
}
#endif

#endif // _tinystr_h_