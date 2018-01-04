#pragma once

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Table_Row.H>
#include "tinystr.h"
#include "tinylog.h"

class table_t : public Fl_Table_Row
{
public:
	table_t(int x, int y, int w, int h, const char *l = 0) 
		:Fl_Table_Row(x, y, w, h, l)
	{
		end();
		callback(&event_callback, (void*)this);
		//col_resize(1);
		col_header(1);
		type(SELECT_SINGLE);
		cols(2);
		rows(222);
		col_width(0, 100);
		col_width(1, w - 100-20);
		row_height_all(20);
	}

	//void update(ioctl_table* t);

private:
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
		if (Fl::event() == 2)
		{
			int _ms = _gap_ms(t, last_t);
			printf("%d\n", _ms);
			if (_ms < 300 && last_c == C && last_r == R)
			{
				printf("DOUBLE CLICK!!!");
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
		
		printf("'%s' callback: ", (label() ? label() : "?"));
		printf("Row=%d Col=%d Context=%d Event=%d InteractiveResize? %d\n",
			R, C, (int)context, (int)Fl::event(), (int)is_interactive_resize());
	}

	void draw_cell(TableContext context, int r, int c, int x, int y, int w, int h)
	{
		static char s[40];
		sprintf_s(s, 40, "%d/%d", r, c);		// text for each cell
		Fl_Color bgcolor = FL_WHITE;
		Fl_Color fgcolor = FL_BLACK;

		switch (context)
		{
		case CONTEXT_STARTPAGE:
			fl_font(FL_HELVETICA, 12);
			return;

		case CONTEXT_COL_HEADER:
			fl_push_clip(x, y, w, h);
			{
				fl_font(FL_HELVETICA, 12);
				fl_draw_box(FL_THIN_UP_BOX, x, y, w, h, col_header_color());
				fl_color(FL_BLACK);
				switch (c)
				{
				case 0:
					fl_draw(gbk2utf8_tmp("Ãû³Æ"), x, y, w, h, FL_ALIGN_CENTER);
					break;
				case 1:
					fl_draw(gbk2utf8_tmp("ÃèÊö"), x, y, w, h, FL_ALIGN_CENTER);
					break;
				default:
					break;
				}

			}
			fl_pop_clip();
			return;

		case CONTEXT_ROW_HEADER:
			fl_push_clip(x, y, w, h);
			{
				fl_draw_box(FL_THIN_UP_BOX, x, y, w, h, row_header_color());
				fl_color(FL_BLACK);
				fl_draw(s, x, y, w, h, FL_ALIGN_CENTER);
			}
			fl_pop_clip();
			return;

		case CONTEXT_CELL:
		{
			fl_push_clip(x, y, w, h);
			{
				// BG COLOR
				fl_color(row_selected(r) ? selection_color() : bgcolor);
				fl_rectf(x, y, w, h);

				// TEXT
				fl_color(fgcolor);
				fl_draw(gbk2utf8_tmp("²âÊÔtest"), x, y, w, h, FL_ALIGN_CENTER);
				
				// BORDER
				//fl_color(color());
				//fl_rect(x, y, w, h);
			}
			fl_pop_clip();
			return;
		}

		case CONTEXT_TABLE:
			fprintf(stderr, "TABLE CONTEXT CALLED\n");
			return;

		case CONTEXT_ENDPAGE:
		case CONTEXT_RC_RESIZE:
		case CONTEXT_NONE:
			return;
		}
	}
};
