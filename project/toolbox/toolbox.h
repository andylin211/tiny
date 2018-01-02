#pragma once
#include "resource.h"
#include "tinyfltk.h"
#include "tinyshell.h"
#include <assert.h>

class toolbox_t
{
	int _x, _y, _w, _h;
	char* ret_str;
	struct {
		int count;
		char* str;
		char** file;
		char** name;
	}script;
	
	Fl_Button** buttons;
	

public:
	int w() { return _w; }
	int h() { return _h; }
	static void script_cb(char* str, void* data)
	{
		toolbox_t* tb = (toolbox_t*)data;
		char* tmp = tb->ret_str;
		tb->ret_str = str_format_large("%s%s", tmp ? tmp: "", str);
		free(tmp);
	}
	void init()
	{
		exec_script_str("c:/tiny/shell/bash.exe", "PATH='c:/tiny/shell' \n find c:/tiny/script | grep '.sh$'", script_cb, this);
		script.str = ret_str;
		ret_str = 0;
		script.count = str_count(script.str, '\n');
		script.file = (char**)safe_malloc(sizeof(char*) * script.count);
		script.name = (char**)safe_malloc(sizeof(char*) * script.count);
		buttons = (Fl_Button**)safe_malloc(sizeof(Fl_Button*)*script.count);
		int p = 0;
		for (int i = 0; i < script.count; i++)
		{
			script.file[i] = &script.str[p];
			script.name[i] = &script.str[str_rfind(script.file[i], '/')+1];
			p = str_find_from(script.str, '\n', p);
			script.str[p] = 0;
			p++;

			buttons[i] = fltk_t::create_button(20, 50*i, 100, 40, script.name[i]);
		}
	}



public:
	toolbox_t(int x = 0, int y = 0, int w = 350, int h = 250)
		:_x(x), _y(y), _w(w), _h(h),
		ret_str(0),
		buttons(0)
	{
		init();
	}

	~toolbox_t()
	{
		free(script.str);
		free(script.file);
		free(script.name);
		free(buttons);
	}


};




