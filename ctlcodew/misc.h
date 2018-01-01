#pragma once
#include "FL\fl_utf8.H"
#include "FL\Fl_Box.H"
#include "FL\Fl_Button.H"
#include "FL\FL_Hold_Browser.H"
#include "FL\Fl_Return_Button.H"
#include "FL\Fl_Input.H"
#include "bmp_t.h"

class misc
{
public:
	static const int buf_len = 256;
	
	static char* gbk2utf8(char* gbk) {
		static char buf[buf_len];
		memset(buf, 0, buf_len);
		if (gbk)
			fl_utf8from_mb(buf, buf_len, gbk, strlen(gbk));
		return buf;
	}

	static Fl_Box* create_outbox(int x, int y, int w, int h)
	{
		Fl_Box* b = new Fl_Box(x, y, w, h);
		b->box(FL_GTK_UP_BOX);
		b->align(FL_ALIGN_TOP);
		return b;
	}

	static Fl_Box* create_imgbox(int x, int y, int w, int h, int id)
	{
		Fl_Box* b = new Fl_Box(x, y, w, h);
		bmp_t* image = new bmp_t(id);
		b->image(image);
		return b;
	}

	static Fl_Box* create_labelbox(int x, int y, int w, int h, char* label)
	{
		Fl_Box* b = new Fl_Box(x, y, w, h);
		b->labelfont(FL_COURIER);
		b->labelsize(12);
		b->copy_label(gbk2utf8(label));
		return b;
	}
	
	static Fl_Button* create_button(int x, int y, int w, int h, char* label, bool is_return_type = 0)
	{
		Fl_Button* b = is_return_type ? new Fl_Return_Button(x,y,w,h) : new Fl_Button(x, y, w, h);
		b->labelsize(12);
		b->labelfont(FL_HELVETICA);
		b->copy_label(gbk2utf8(label));
		return b;
	}

	static Fl_Hold_Browser* create_browser(int x, int y, int w, int h)
	{
		Fl_Hold_Browser* b = new Fl_Hold_Browser(x, y, w, h);
		b->textfont(FL_HELVETICA);
		b->textsize(12);
		return b;
	}

	static Fl_Input* create_input(int x, int y, int w, int h, char* label=0)
	{
		Fl_Input* i = new Fl_Input(x, y, w, h);
		i->textfont(FL_COURIER);
		i->textsize(12);
		i->labelfont(FL_HELVETICA);
		i->labelsize(12);
		i->copy_label(gbk2utf8(label));
		return i;
	}

	static void make_screen_center(Fl_Widget* v)
	{
		int x, y, w, h;
		Fl::screen_work_area(x, y, w, h);
		v->position(w / 2 - v->w() / 2, h / 2 - v->h() / 2);
	}
};

