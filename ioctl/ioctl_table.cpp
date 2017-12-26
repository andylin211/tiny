#include "ioctl_table.h"
#include "common.h"
#include "FL\Fl_Menu_Button.H"

char menu_fuzz_sel[buf_len];

Fl_Menu_Item pulldown[] = {
	{ menu_fuzz_sel },
	{ "Fuzz &All",	FL_ALT + FL_SHIFT + 'f' },
	{ 0 }
};

Ioctl_Table::Ioctl_Table(int x, int y, int w, int h, const char *l) 
	:Fl_Table_Row(x, y, w, h, l)
{
	end();
	callback(&event_callback, (void*)this);
	col_resize(1);
	col_header(1);
	v_line = 0;
	Fl_Menu_Button *mb = new Fl_Menu_Button(x, y+20, w, h-20);
	mb->type(Fl_Menu_Button::POPUP3);
	mb->box(FL_NO_BOX);
	set_menu_label(menu_fuzz_sel, "&Fuzz选中项");
	for (int i = 0; i < sizeof(pulldown) / sizeof(pulldown[0]); i++)
	{
		pulldown[i].labelfont(FL_HELVETICA);
		pulldown[i].labelsize(12);
	}
	mb->menu(pulldown);
	//mb.callback(test_cb);
}

void Ioctl_Table::event_callback(Fl_Widget* o, void* v)
{
	Ioctl_Table* t = (Ioctl_Table*)v;
	t->event_callback2();
}

void Ioctl_Table::event_callback2()
{
	int R = callback_row(),
		C = callback_col();
	TableContext context = callback_context();
	printf("'%s' callback: ", (label() ? label() : "?"));
	printf("Row=%d Col=%d Context=%d Event=%d InteractiveResize? %d\n",
		R, C, (int)context, (int)Fl::event(), (int)is_interactive_resize());
}

void Ioctl_Table::draw_cell(TableContext context, int r, int c, int x, int y, int w, int h)
{
	static char s[40];
	sprintf(s, "%d/%d", r, c);		// text for each cell
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
				fl_draw(label_conv("设备名"), x, y, w, h, FL_ALIGN_CENTER);
				break;
			case 1:
				fl_draw(label_conv("控制码"), x, y, w, h, FL_ALIGN_CENTER);
				break;
			case 2:
				fl_draw(label_conv("设备类型"), x, y, w, h, FL_ALIGN_CENTER);
				break;
			case 3:
				fl_draw(label_conv("函数号"), x, y, w, h, FL_ALIGN_CENTER);
				break;
			case 4:
				fl_draw(label_conv("缓存类型"), x, y, w, h, FL_ALIGN_CENTER);
				break;
			case 5:
				fl_draw(label_conv("权限"), x, y, w, h, FL_ALIGN_CENTER);
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
			if (v_line && r < (int)v_line->size())
			{
				char** str = (char**)(&v_line->at(r));
				fl_draw(str[c], x, y, w, h, FL_ALIGN_CENTER);
			}
				
			// BORDER
			fl_color(color());
			fl_rect(x, y, w, h);
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

void Ioctl_Table::update(std::vector<ioctl_line> *v_line)
{
	Fl::lock();
	this->v_line = v_line;
	if (v_line && (int)v_line->size() / cols() > min_row_)
		rows(v_line->size() / cols());
	else
		rows(min_row_);
	Fl::unlock();
	redraw();
}
