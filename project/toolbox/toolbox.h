#pragma once
#include "resource.h"
#include "tinyfltk.h"
#include "tinyshell.h"
#include <assert.h>
#include "FL\Fl_Text_Editor.H"
#include "FL\Fl_Text_Buffer.H"
#include "FL\Fl_Text_Display.H"

const int buf_len = 256;
#define TS 14 // default editor textsize
Fl_Text_Display::Style_Table_Entry styletable[] = {	// Style table
	{ FL_BLACK,      FL_COURIER,           TS }, // A - Plain
	{ FL_DARK_GREEN, FL_HELVETICA_ITALIC,  TS }, // B - Line comments
	{ FL_DARK_GREEN, FL_HELVETICA_ITALIC,  TS }, // C - Block comments
	{ FL_BLUE,       FL_COURIER,           TS }, // D - Strings
	{ FL_DARK_RED,   FL_COURIER,           TS }, // E - Directives
	{ FL_DARK_RED,   FL_COURIER_BOLD,      TS }, // F - Types
	{ FL_BLUE,       FL_COURIER_BOLD,      TS }, // G - Keywords
};

class toolbox_t
{
	int _x, _y, _w, _h;
	wchar_t* script_output;
	wchar_t* script_error;
	struct {
		int count;
		wchar_t* str;
		wchar_t** file;
		wchar_t** name;
	}script;
	
	Fl_Button** buttons;

	char bash_dir[buf_len];
	char bash_path[buf_len];
	char script_dir[buf_len];
	char icon_dir[buf_len];

public:
	int w() { return _w; }
	int h() { return _h; }
	void clean_script()
	{
		if (script_error)
		{
			free(script_error);
			script_error = 0;
		}
		if (script_output)
		{
			free(script_output);
			script_output = 0;
		}
	}
	static void script_cb(wchar_t* output, wchar_t* error, void* data)
	{
		toolbox_t* tb = (toolbox_t*)data;
		wchar_t* tmp = 0;
		if (output)
		{
			tmp = tb->script_output;
			tb->script_output = wcs_format_large(L"%s%s", tmp ? tmp : L"", output);
			free(tmp);
		}
		if (error)
		{
			tmp = tb->script_error;
			tb->script_error = wcs_format_large(L"%s%s", tmp ? tmp : L"", error);
			free(tmp);
		}
		
	}
	void do_script(char* script)
	{
		char str[buf_len];
		clean_script();
		str_format(str, buf_len, "PATH=\"c:/windows/system32;%s\"\n%s", bash_dir, script);
		exec_script_str(bash_path, str, script_cb, this);
	}
	void init()
	{
		char str[buf_len];
		do_script(str_format(str, buf_len, "gfind \"%s\" | grep '.sh$'", script_dir));

		if (script_error)
			wprintf(script_error);
		if (!script_output)
			return;
		script.str = script_output;
		script_output = 0;
		script.count = wcs_count(script.str, L'\n');
		script.file = (wchar_t**)safe_malloc(sizeof(wchar_t*) * script.count);
		script.name = (wchar_t**)safe_malloc(sizeof(wchar_t*) * script.count);
		buttons = (Fl_Button**)safe_malloc(sizeof(Fl_Button*)*script.count);
		int p = 0;
		wchar_t* ss = script.str;
		for (int i = 0; i < script.count; i++)
		{
			ss = &ss[p];
			script.file[i] = ss;
			p = wcs_find(ss, L'\n');
			ss[p] = 0;
			p++;
			script.name[i] = &ss[wcs_rfind(script.file[i], L'/') + 1];

			buttons[i] = fltk_t::create_button(20, 100 * i + 20, 100, 100, 0);
			buttons[i]->copy_label(wcs_to_str_tmp(script.name[i], -1, encoding_utf8));
		}
	}

	static void style_unfinished_cb(int, void*) {}

	void test()
	{
		Fl_Text_Buffer* textbuf = new Fl_Text_Buffer;
		textbuf->text("hello world!!");

		Fl_Text_Buffer* stylebuf = new Fl_Text_Buffer;
		stylebuf->text("ABCDEFGAAAAAA");

		Fl_Text_Editor* editor = new Fl_Text_Editor(0, 0, _w, _h);
		editor->textsize(12);
		editor->textfont(FL_HELVETICA);
		editor->buffer(textbuf);

		editor->highlight_data(stylebuf, styletable, sizeof(styletable) / sizeof(styletable[0]),
			'A', style_unfinished_cb, 0);
	}

public:
	toolbox_t(int x = 0, int y = 0, int w = 350, int h = 350)
		:_x(x), _y(y), _w(w), _h(h),
		script_output(0),
		buttons(0)
	{
		strcpy_s(bash_dir, buf_len, "c:/tiny/shell");
		strcpy_s(bash_path, buf_len, "c:/tiny/shell/bash.exe");
		strcpy_s(script_dir, buf_len, "c:/script");
		strcpy_s(icon_dir, buf_len, "c:/icon");
		//init();
		test();
	}

	~toolbox_t()
	{
		free(script.str);
		free(script.file);
		free(script.name);
		free(buttons);
		clean_script();
	}


};




