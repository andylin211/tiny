#ifndef _tinyui_h_
#define _tinyui_h_

#include <wchar.h>

#ifdef __cplusplus
extern "C" {
#endif

	void ui();

	void* ui_get_hwnd(void* ui_context);

	/* value */
	void ui_set_value(void* ui_context, wchar_t* name, wchar_t* value);

	wchar_t* ui_get_value(void* ui_context, wchar_t* name);

	/* dialog */
	wchar_t* ui_ask_open_file(void* ui_context);

	wchar_t* ui_ask_save_file(void* ui_context);

	wchar_t* ui_ask_open_path(void* ui_context);

	/* button click handler */
	#define ui_click(name, ui_context) __declspec(dllexport) void on_##name##_click(void* ui_context)

	/* disable/enable */
	void ui_disable(void* ui_context, wchar_t* name);

	void ui_enable(void* ui_context, wchar_t* name);

	/* show/hide */
	void ui_show(void* ui_context, wchar_t* name);

	void ui_hide(void* ui_context, wchar_t* name);

	/* post task */
	#define ui_task(name, data) __declspec(dllexport) void on_##name##_task(void* arg_list)

	void ui_post_task(void* ui_context, char* name, void* arg_list);

#ifdef __cplusplus
}
#endif

#endif
