#pragma once

#include "FL\fl_utf8.h"
#include <Shlwapi.h>
#include <ShlObj.h>
#include <assert.h>
#include "tinystr.h"

#define buf_len 512

#ifdef __cplusplus
extern "C" {
#endif

	char* label_conv(char* gbk);

	void RunAs(LPCSTR _param);

	int is_evevated();

	int has_been_running();

	int is_xp();

	wchar_t* ask_open_file(HWND hwnd);

	wchar_t* ask_save_file(HWND hwnd);

	wchar_t* ask_open_path(HWND hwnd);

	char* read_file_raw(wchar_t* file);

#define set_label_font_12(w) w->labelsize(12);\
		w->labelfont(FL_HELVETICA)

#define set_text_font_12(w) w->textsize(12);\
		w->textfont(FL_HELVETICA)

	void set_menu_label(char* utf8, char* gbk);

	typedef struct {
		wchar_t* v1;
		wchar_t* v2;
	} type_def_t;

	extern type_def_t type_def[];

	extern int type_def_size;

	wchar_t* type_str(wchar_t* str);

	wchar_t* access_str(wchar_t* str);

	wchar_t* method_str(wchar_t* str);
#ifdef __cplusplus
}
#endif