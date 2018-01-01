#pragma once
#include <Windows.h>

#ifdef __cplusplus
extern "C" {
#endif

#define buf_len 512

	char* label_conv(char* gbk);

	void RunAs(LPCSTR _param);

	int is_evevated();

	int has_been_running();

	int is_xp();

	wchar_t* ask_open_file(HWND hwnd);

	wchar_t* ask_save_file(HWND hwnd);

	wchar_t* ask_open_path(HWND hwnd);

	char* read_file_raw(wchar_t* file);

#define set_label_font(w) w->labelsize(12);\
		w->labelfont(FL_HELVETICA)

#define set_text_font(w) w->textsize(12);\
		w->textfont(FL_HELVETICA)

	int ctoi(char ch);

	unsigned long make_ioctl_code(unsigned long device_type, unsigned long function, unsigned long method, unsigned long access);

	/* e.g. "0" "f" */
	char* hex_digit_str(int i);

#define max_device_type 45

	/* e.g. "unknown" */
	char* device_type_str(int i);

	char* device_type_unknown_str();

	/* e.g. "0x0022" */
	char* device_type_hex(int i);

	char* device_type_unknown_hex();

	/* e.g. "¶ÁÐ´" */
	char* access_str(int i);

	/* e.g. "buffered" */
	char* method_str(int i);

	/* e.g. "0x3" */
	char* hex_str_0x(int i);

#ifdef __cplusplus
}
#endif

