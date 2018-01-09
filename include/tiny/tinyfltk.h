#pragma once
#include "FL\fl_utf8.H"
#include "FL\Fl_Box.H"
#include "FL\Fl_Button.H"
#include "FL\FL_Hold_Browser.H"
#include "FL\Fl_Return_Button.H"
#include "FL\Fl_Input.H"
#include "FL\Fl_RGB_Image.H"
#include <Windows.h>

class fltk_bmp_t : public Fl_RGB_Image
{
public:
	fltk_bmp_t(int res_id) :Fl_RGB_Image(0, 0, 0)
	{
		char* buf = read_resource(res_id);
		int width = 0;
		int height = 0;
		Fl_RGB_Image::array = (uchar*)parse_bmp(buf, width, height);
		d(4);
		w(width);
		h(height);
		free(buf);
	}

	static char* parse_bmp(char* buf, int& w, int& h)
	{
		char* ret = 0;
		w = h = 0;

		do
		{
			if (!buf || strncmp("BM", buf, 2))
				break;

			int offset = *(int*)(buf + 10);
			w = *(int*)(buf + 18);
			h = *(int*)(buf + 22);
			int depth = *(int*)(buf + 28);

			if (depth != 24 || h <= 0 || !w)
				break;

			ret = new char[w * h * 4];

			/* (feed + w * 3) % 4 = 0 */
			int feed = w * 3 % 4;
			feed = feed ? 4 - feed : feed;

			int row_bytes = w * 3 + feed;

			char* p = ret;
			for (int y = h - 1; y >= 0; y--)
			{
				for (int x = 0; x < w; x++)
				{
					int t = y * w * 4 + x * 4;
					int s = offset + (h - 1 - y) * row_bytes + x * 3;
					p[t + 2] = buf[s + 0];
					p[t + 1] = buf[s + 1];
					p[t + 0] = buf[s + 2];
					p[t + 3] = (char)0xff;
					if (p[t + 2] == (char)0xff && p[t + 1] == (char)0xff && p[t + 0] == (char)0xff)
						p[t + 3] = 0;
				}
			}

		} while (0);

		return ret;
	}

	static char* read_resource(int id)
	{
		HGLOBAL hglobal = NULL;
		HINSTANCE hinst;
		HRSRC hrsrc;
		unsigned int size = 0;
		char* buffer = 0;
		char* res = 0;

		hinst = NULL;
		hrsrc = FindResource(hinst, MAKEINTRESOURCE(id), L"RAW");
		GetLastError();
		if (!hrsrc)
			return 0;

		size = SizeofResource(hinst, hrsrc);
		if (!size)
			return 0;

		hglobal = LoadResource(hinst, hrsrc);
		if (!hglobal)
			return 0;

		res = (char*)LockResource(hglobal);
		if (!res)
			return 0;

		buffer = (char*)malloc(size);
		memcpy(buffer, res, size);

		if (hglobal)
			FreeResource(hglobal);

		return buffer;
	}
};

class fltk_t
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
		fltk_bmp_t* image = new fltk_bmp_t(id);
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

