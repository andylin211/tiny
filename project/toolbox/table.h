#pragma once

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Table_Row.H>
#include "tinystr.h"
#include "tinylog.h"
#include <stdio.h>
#include <vector>

class table_t : public Fl_Table_Row
{
	Fl_Callback* dbclick_cb;
	void* dbclick_data;
	char* script_str;
	std::vector<char*> vscript_str;
public:
	char* script_file;
public:
	table_t(int x, int y, int w, int h, const char *l = 0)
		:Fl_Table_Row(x, y, w, h, l),
		script_str(0),
		script_file(0)
	{
		end();
		callback(&event_callback, (void*)this);
		col_resize(1);
		col_header(1);
		type(SELECT_SINGLE);
		col_width(0, 200);
		col_width(1, 300);
		col_width(2, w - col_width(0) - col_width(1) - 20);
		row_height_all(20);
		cols(3);
	}

	~table_t()
	{
		free_data();
		if (script_file)
			free(script_file);
	}

	void dbclick_callback(Fl_Callback* cb, void* v)
	{
		dbclick_cb = cb;
		dbclick_data = v;
	}

	void update_data(std::vector<char*> vstr, char** str)
	{
		free_data();
		script_str = *str;
		vscript_str = vstr;
		*str = 0;// table 负责释放
		int nline = (int)vscript_str.size() / 3;
		rows(nline);
		redraw();
	}

private:
	void free_data()
	{
		if (script_str)
		{
			free(script_str);
			script_str = 0;
		}
	}

	static void event_callback(Fl_Widget*, void*v)
	{
		table_t* t = (table_t*)v;
		t->event_callback2();
	}

	// ns 9 
	static int _gap_ms(long t_new, long t_old)
	{
		return (t_new - t_old);
	}
	void resize(int x, int y, int w, int h)
	{
		//col_width(1, w - col_width(0) - col_width(2) - 20);
		col_width(2, w -col_width(1) - col_width(0) - 20);
		Fl_Table::resize(x, y, w, h);
	}
	void event_callback2()
	{
		static int last_r = -1;
		static int last_c = -1;
		static clock_t last_t = { 0 };
		clock_t t = { 0 };
		t = clock();
		
		int R = callback_row(),
			C = callback_col();
		TableContext context = callback_context();
		if (Fl::event() == 2 && CONTEXT_CELL == context)
		{
			int _ms = _gap_ms(t, last_t);
			printf("%d\n", _ms);
			if (_ms < 300 && last_c == C && last_r == R)
			{
				if (script_file)
					free(script_file);
				
				script_file = utf82gbk(vscript_str.at(R * 3 + 2));
				if (dbclick_cb)
					dbclick_cb(this, dbclick_data);
				memset(&last_t, 0, sizeof(last_t));
				last_r = -1;
				last_c = -1;
			}
			else
			{
				memcpy(&last_t, &t, sizeof(t));
				last_r = R;
				last_c = C;
			}
		}
		
		printf("Row=%d Col=%d Context=%d Event=%d InteractiveResize? %d\n", R, C, (int)context, (int)Fl::event(), (int)is_interactive_resize());
	}

	void draw_cell(TableContext context, int r, int c, int x, int y, int w, int h)
	{
		static char s[40];
		sprintf_s(s, 40, "%d/%d", r, c);		// text for each cell
		Fl_Color bgcolor = FL_WHITE;
		Fl_Color bgcolor1 = 0xF8F8F800;
		Fl_Color fgcolor = FL_BLACK;

		switch (context)
		{
		case CONTEXT_COL_HEADER:
			fl_push_clip(x, y, w, h);
			{
				fl_font(FL_HELVETICA, 12);
				fl_draw_box(FL_THIN_UP_BOX, x, y, w, h, col_header_color());
				fl_color(FL_BLACK);
				switch (c)
				{
				case 0:
					fl_draw(gbk2utf8_tmp("名称"), x, y, w, h, FL_ALIGN_CENTER);
					break;
				case 1:
					fl_draw(gbk2utf8_tmp("描述"), x, y, w, h, FL_ALIGN_CENTER);
					break;
				case 2:
					fl_draw(gbk2utf8_tmp("路径"), x, y, w, h, FL_ALIGN_CENTER);
					break; 
				default:
					break;
				}

			}
			fl_pop_clip();
			break;
		case CONTEXT_CELL:
		{
			fl_push_clip(x, y, w, h);
			{
				// BG COLOR
				if (c == 0)
					fl_color(row_selected(r) ? selection_color() : bgcolor1);
				else
					fl_color(row_selected(r) ? selection_color() : bgcolor);
				
				fl_rectf(x, y, w, h);

				fl_font(FL_HELVETICA, 12);
				// TEXT
				fl_color(fgcolor);
				if ((int)vscript_str.size() > 3 * r + c)
					fl_draw(vscript_str.at(3 * r + c), x+4, y, w-8, h, FL_ALIGN_LEFT);
				
				// BORDER
				//fl_color(color());
				//fl_rect(x, y, w, h);
			}
			fl_pop_clip();
			break;
		}
		case CONTEXT_STARTPAGE:
		case CONTEXT_ROW_HEADER:
		case CONTEXT_TABLE:
		case CONTEXT_ENDPAGE:
		case CONTEXT_RC_RESIZE:
		case CONTEXT_NONE:
		default:
			break;
		}
	}
};
