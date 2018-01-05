#pragma once
#include "resource.h"
#include "tinyfltk.h"
#include "tinyshell.h"
#include <assert.h>
#include "FL\Fl_Text_Editor.H"
#include "FL\Fl_Text_Buffer.H"
#include "FL\Fl_Text_Display.H"
#include "FL\Fl_Help_View.H"
#include "FL\Fl_Multiline_Output.H"
#include "table.h"
#include <vector>

const int outp_height = 160;
const int buf_len = 256;
Fl_Text_Display::Style_Table_Entry styletable[] = {
	{ 0xD0D0D000,	FL_COURIER,           12 }, // A - Plain
	{ 0xE0000000,	FL_COURIER,           12 }, // B - Error
	{ 0x00E00000,	FL_COURIER,           12 }, // C - Shell
};

class toolbox_t: public Fl_Window
{
	int _x, _y, _w, _h;
	wchar_t* script_output;
	wchar_t* script_error;
	std::vector<char*> vscript_str;//utf8, \t, \r\n, \n
	char* script_str;/* need free */
	
	char curr_dir[buf_len];
	char bash_dir[buf_len];
	char bash_path[buf_len];
	char script_dir[buf_len];

	Fl_Input* inpu;
	Fl_Return_Button* butt;
	table_t* tabl;
	Fl_Text_Buffer* tbuf;
	Fl_Text_Buffer* sbuf;
	Fl_Text_Display* outp;
	enum {
		output_plain = 0,
		output_error,
		output_shell,
	}output_type;


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
		if (script_str)
		{
			free(script_str);
			script_str = 0;
		}
		vscript_str.clear();
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
	char* skip_token(char* p)
	{
		while (*p)
		{
			if (*p == '\t' || *p == '\n')
			{
				*p++ = 0;
				return p;
			}
			if (*p == '\r')
			{
				*p++ = 0;
				*p++ = 0;
				return p;
			}
			p++;
		}
		return p;
	}
	void update_vscript_str()
	{
		script_str = wcs_to_str(script_output, -1, encoding_utf8);
		if (!script_str)
			return;
		char* p = script_str;
		while (*p)
		{
			vscript_str.push_back(p);
			p = skip_token(p);
		}
	}
	void do_script(char* script, bool need_output=0)
	{
		if (!script)
			return;
		clean_script();
		shell(script);
		char* buf = str_format_large("PATH=\"c:/windows/system32;%s\"\n%s", bash_dir, script);
		exec_script_str(bash_path, buf, script_cb, this);
		if (need_output)
			update_vscript_str();
		error(script_error);
		output(script_output);
		free(buf);
	}

	char* read_raw_file(char* file)
	{
		FILE* fp = 0;
		fopen_s(&fp, file, "rb");
		fseek(fp, 0, SEEK_END);
		int len = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		char* buf = (char*)safe_malloc(len + 1);
		fread(buf, len, 1, fp);
		buf[len] = 0;
		fclose(fp);
		return buf;
	}

	void do_script_file(char* script, char* args=0, bool need_output=0)
	{
		if (!script)
			return;
		clean_script();
		shell(script);
		exec_script_file(bash_path, script, args, script_cb, this);
		if (need_output)
			update_vscript_str();
		error(script_error);
		output(script_output);
	}

	static char* skip_line(char* p)
	{
		while (*p)
		{
			if (*p == '\n')
				return ++p;
			p++;
		}
		return p;
	}

	void init_data()
	{
 		do_script_file("c:/test1.sh", "", true);
		tabl->update_data(vscript_str, &script_str);
	}

	static void inpu_cb(Fl_Widget*, void* v)
	{
		toolbox_t* tb = (toolbox_t*)v;
		tb->do_script_file("c:/test2.sh", utf82gbk_tmp((char*)tb->inpu->value()), true);
		tb->tabl->update_data(tb->vscript_str, &tb->script_str);
	}

	static void butt_cb(Fl_Widget*, void* v)
	{

	}

	static void tabl_cb(Fl_Widget*, void* v)
	{
		toolbox_t* tb = (toolbox_t*)v;
		tb->do_script_file(tb->tabl->script_file);
	}

	static void style_unfinished_cb(int, void*) 
	{

	}

	void _output(char* str)
	{
		tbuf->append(str);
		tbuf->call_modify_callbacks();
		outp->scroll(tbuf->count_lines(0, tbuf->length()), 0);
	}

	void output(char* str)
	{
		if (!str)
			return;
		output_type = output_plain;
		_output(str);
	}

	void output(wchar_t* str)
	{
		if (!str)
			return;
		char* s = wcs_to_str(str, -1, encoding_utf8);
		output_type = output_plain;
		_output(s);
		free(s);
	}

	void shell(char* str)
	{
		char* buf = 0;
		if (!str)
			return;
		buf = str_format_large("$ %s\r\n", str);
		char* tmp = gbk2utf8(buf);
		output_type = output_shell;
		_output(tmp);
		free(tmp);
		free(buf);
	}

	void error(char* str)
	{
		if (!str)
			return;
		output_type = output_error;
		_output(str);
	}

	void error(wchar_t* str)
	{
		if (!str)
			return;
		char* s = wcs_to_str(str, -1, encoding_utf8);
		output_type = output_error;
		_output(s);
		free(s);
	}

	void style_update(int pos, int ni)/*ninserted, ndeleted*/
	{
		if (ni == 0) {
			sbuf->unselect();
			return;
		}

		char* style = new char[ni + 1];
		memset(style, 'A' + output_type, ni);
		style[ni] = '\0';

		sbuf->replace(pos, pos, style);
		delete[] style;

		outp->redisplay_range(pos, pos+ni);
	}

	static void style_update(int pos, int ninserted, int ndeleted, int /*nrestyled*/, const char* /*deletedtext*/, void* arg)
	{
		((toolbox_t*)arg)->style_update(pos, ninserted);
	}
	
	void init_control()
	{
		inpu = new Fl_Input(2, 2, _w-4, 20);
		inpu->box(FL_FLAT_BOX);
		inpu->textfont(FL_HELVETICA);
		inpu->textsize(12);
		inpu->callback(inpu_cb, this);
		inpu->when(FL_WHEN_CHANGED);

		butt = new Fl_Return_Button(_w + 2, 2, 58, 20);//Òþ²ØµÄ
		butt->callback(butt_cb, this);
		
		tabl = new table_t(0, 25, _w, _h - outp_height - 25);
		tabl->cols(2);
		tabl->dbclick_callback(tabl_cb, this);

		tbuf = new Fl_Text_Buffer;
		tbuf->add_modify_callback(style_update, this);

		sbuf = new Fl_Text_Buffer;

		outp = new Fl_Text_Display(0, _h - outp_height, _w, outp_height);
		outp->color(FL_BLACK);
		outp->box(FL_THIN_DOWN_BOX);
		outp->textsize(12);
		outp->textfont(FL_COURIER);
		outp->textcolor(0xD0D0D000);
		outp->buffer(tbuf);

		outp->highlight_data(sbuf, styletable, sizeof(styletable) / sizeof(styletable[0]),
			'A', style_unfinished_cb, 0);

		resizable(tabl);
	}

public:
	toolbox_t(int x = 0, int y = 0, int w = 600, int h = 400)
		:Fl_Window(w, h, "ToolBox"),
		_x(x), _y(y), _w(w), _h(h),
		script_output(0),
		script_error(0),
		script_str(0)
	{
		fltk_t::make_screen_center(this);

		GetModuleFileNameA(NULL, curr_dir, buf_len);
		str_replace_char(curr_dir, '\\', '/');
		str_erase(curr_dir, str_rfind(curr_dir, '/'), -1);
		strcpy_s(bash_dir, buf_len, "c:/tiny/shell");
		strcpy_s(bash_path, buf_len, "c:/tiny/shell/bash.exe");
		strcpy_s(script_dir, buf_len, "c:/script");

		init_control();

		end();

		init_data();
	}

	~toolbox_t()
	{
		clean_script();
	}
};




