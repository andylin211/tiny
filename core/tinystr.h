#ifndef _tinystr_h_
#define _tinystr_h_
#include <wchar.h>

#ifdef __cplusplus
extern "C" {
#endif

	typedef enum
	{
		encoding_ansi = 0,
		encoding_utf8
	}str_encoding;

	void* safe_malloc(int size);

	wchar_t* str_to_wcs(char* string, int size, str_encoding encoding);

	char* wcs_to_str(wchar_t* string, int size, str_encoding encoding);

	int str_endwith(char* string, char* tail);

	int wcs_endwith(wchar_t* string, wchar_t* tail);

	int str_beginwith(char* string, char* head);

	int wcs_beginwith(wchar_t* string, wchar_t* head);

	char* str_format(char* buffer, int size, char* format, ...);

	wchar_t* wcs_format(wchar_t* buffer, int size, wchar_t* format, ...);

	wchar_t* wcs_vformat_large(wchar_t* format, va_list arg_list);

	wchar_t* wcs_format_large(wchar_t* format, ...);

	int wcs_find(wchar_t* string, wchar_t ch);

	int str_find(char* string, char ch);

	int wcs_find_from(wchar_t* string, wchar_t ch, int from);

	int wcs_rfind(wchar_t* string, wchar_t ch);

	int str_rfind(char* string, char ch);

	int wcs_rfind_from(wchar_t* string, wchar_t ch, int from);

	wchar_t* wcs_erase(wchar_t* string, int from, int to);

	char* str_erase(char* string, int from, int to);

	wchar_t* wcs_remove(wchar_t* string, wchar_t ch);

	int is_white_space(char ch);

	int is_white_spacew(wchar_t ch);

	char* str_trim_end(char* string);

	wchar_t* wcs_trim_end(wchar_t* string);

	int wcs_find_sub_from(wchar_t* string, wchar_t* sub, int from);

	int wcs_find_sub_count(wchar_t* string, wchar_t* sub);

	wchar_t* wcs_replace(wchar_t* string, wchar_t* sub, wchar_t* to);

#ifdef __cplusplus
}
#endif

#endif
