#include "Fl.H"
#include "Fl_Window.H"
#include "Fl_Sys_Menu_Bar.H"
#include "Fl_RGB_Image.H"
#include "fl_utf8.h"
#include "Fl_Box.H"
#include "Fl_Button.H"
#include "Fl_Hold_Browser.H"
#include "Fl_Table_Row.H"
#include "fl_draw.H"
#include "Fl_Text_Buffer.H"
#include "Fl_Multiline_Output.H"
#include "Fl_Check_Button.H"
#include "resource.h"

const int buf_len = 256;

class test_bmp : public Fl_RGB_Image
{
public:
	test_bmp(int res) :Fl_RGB_Image(0, 0, 0)
	{
		char* buf = read_resource(res);
		do
		{
			if (!buf)
				break;

			if (strncmp("BM", buf, 2))
				break;

			int off = *(int*)(buf+10);
			int width = *(int*)(buf + 18);
			int height = *(int*)(buf + 22);
			int bits = *(int*)(buf + 28);

			if (bits != 24 || height <= 0 || !width)
				break;

			d(4);
			w(width);
			h(height);
			array = new uchar[width * height * 4];

			char* p = (char*)array;
			int feed = width * 3 % 4;
			if (feed)
				feed = 4 - feed;
			int row_bits = width * 3 + feed;
			for (int y = height - 1; y >= 0; y--)
			{
				for (int x = 0; x < width; x++) 
				{
					int t = y * width * 4 + x * 4;
					int s = off + (height - 1 - y) * row_bits + x * 3;
					p[t + 2] = buf[s + 0];
					p[t + 1] = buf[s + 1];
					p[t + 0] = buf[s + 2];
					p[t + 3] = (char)0xff;
					if (p[t + 2] == (char)0xff && p[t + 1] == (char)0xff && p[t + 0] == (char)0xff)
						p[t + 3] = 0;
				}
			}
		} while (0);

		free(buf);
	}

	char* read_resource(int id)
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

class test_table : public Fl_Table_Row
{
public:
	test_table(int x, int y, int w, int h)
		:Fl_Table_Row(x,y,w,h)
	{
	}

	void draw_cell(TableContext context, int r = 0, int c = 0, int x = 0, int y = 0, int w = 0, int h = 0)
	{
		static char s[40];
		sprintf(s, "%d of %d", r, c);		// text for each cell
		Fl_Color cell_fgcolor = FL_BLACK;
		Fl_Color cell_bgcolor = FL_WHITE;
		switch (context)
		{
		case CONTEXT_STARTPAGE:
			fl_font(FL_HELVETICA, 16);
			return;

		case CONTEXT_COL_HEADER:
			fl_push_clip(x, y, w, h);
			{
				fl_draw_box(FL_THIN_UP_BOX, x, y, w, h, col_header_color());
				fl_color(FL_BLACK);
				fl_draw(s, x, y, w, h, FL_ALIGN_CENTER);

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
				fl_color(row_selected(r) ? selection_color() : cell_bgcolor);
				fl_rectf(x, y, w, h);

				// TEXT
				fl_color(cell_fgcolor);
				fl_draw(s, x, y, w, h, FL_ALIGN_CENTER);
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
};

class test_win : public Fl_Window
{
public:
	test_win()
		: Fl_Window(640, 480)
	{
		copy_label(label_conv("窗口程序测试"));
		set_menu_bar();
		set_tool_bar();
		set_browser();
		set_table();
		set_log();
		set_status();
		end();
	}
private:
	const int width = 640;
	const int height = 480;
	char* label_conv(char* gbk) {
		static char buf[buf_len];
		fl_utf8from_mb(buf, buf_len, gbk, strlen(gbk));
		return buf;
	}
	/* menu bar */
	void set_menu_bar()
	{
		set_menu_label(menu_file, "文件(&F)");
		set_menu_label(menu_open, "打开(&O)");
		set_menu_label(menu_save, "保存(&S)");
		set_menu_label(menu_close, "关闭(&C)");
		set_menu_label(menu_quit, "退出(&Q)");
		set_menu_label(menu_scan, "扫描(&S)");
		set_menu_label(menu_scan_device, "扫描设备(&D)");
		set_menu_label(menu_scan_file, "扫描代码(&S)");
		set_menu_label(menu_fuzz, "F&uzz");
		set_menu_label(menu_fuzz_go, "开始(&S)");
		set_menu_label(menu_fuzz_pause, "暂停(&P)");
		set_menu_label(menu_fuzz_stop, "停止(&Q)");
		Fl_Menu_Bar* menu_bar = new Fl_Menu_Bar(0, 0, width + 1, menu_bar_height);
		menu_bar->menu(menu_table);
	}
	static void set_menu_label(char* utf8, char* gbk) { 
		fl_utf8from_mb(utf8, buf_len, gbk, strlen(gbk)); 
	}
	const int menu_bar_height = 30;
	char menu_file[buf_len];
	char menu_open[buf_len];
	char menu_save[buf_len];
	char menu_close[buf_len];
	char menu_quit[buf_len];
	char menu_scan[buf_len];
	char menu_scan_device[buf_len];
	char menu_scan_file[buf_len];
	char menu_fuzz[buf_len];
	char menu_fuzz_go[buf_len];
	char menu_fuzz_pause[buf_len];
	char menu_fuzz_stop[buf_len];
	Fl_Menu_Item menu_table[16] = {
		{ menu_file, 0,0,0,FL_SUBMENU },
		{ menu_open,	FL_CTRL + 'o', 0, 0 },
		{ menu_save,	FL_CTRL + 's', 0, 0 },
		{ menu_close,	0,	0, 0, FL_MENU_DIVIDER },
		{ menu_quit,	FL_ALT + 'q', 0, 0 },
		{ 0 },
		{ menu_scan, 0,0,0,FL_SUBMENU },
		{ menu_scan_device,	FL_ALT + 'd', 0, 0 },
		{ menu_scan_file,	0, 0, 0 },
		{ 0 },
		{ menu_fuzz, 0,0,0,FL_SUBMENU },
		{ menu_fuzz_go,	FL_ALT + FL_SHIFT + 'f', 0, 0 },
		{ menu_fuzz_pause,	0, 0, 0 },
		{ menu_fuzz_stop,	0, 0, 0 },
		{ 0 },
		{ 0 }
	};
	/* tool bar */
	Fl_Button* tb_open;
	Fl_Button* tb_save;
	Fl_Button* tb_close;
	Fl_Button* tb_scan;
	Fl_Button* tb_fuzz;
	Fl_Button* tb_pause;
	Fl_Button* tb_stop;
	Fl_Button* tb_shutdown;
	const int tool_bar_height = 60;
	const int tool_bar_width = 50;
	void set_tool_bar()
	{
		Fl_Box* box = new Fl_Box(FL_UP_BOX, 0, menu_bar_height, width+1, tool_bar_height, "");
		tb_open = add_tool_button("打开", IDR_OPEN);
		tb_save = add_tool_button("保存", IDR_SAVE);
		tb_close = add_tool_button("关闭", IDR_CLOSE);
		tb_scan = add_tool_button("扫描", IDR_SCAN);
		tb_fuzz = add_tool_button("Fuzz", IDR_FUZZ);
		tb_pause = add_tool_button("暂停", IDR_PAUSE);
		tb_stop = add_tool_button("停止", IDR_STOP);
		tb_shutdown = add_tool_button("退出", IDR_QUIT);
		tb_save->deactivate();
		tb_close->deactivate();
		tb_pause->deactivate();
		tb_stop->deactivate();
	}
	Fl_Button* add_tool_button(char* label, int res)
	{
		static int i = 0;
		test_bmp* bmp = new test_bmp(res);
		Fl_Image *dergb;
		dergb = bmp->copy();
		dergb->inactive();

		Fl_Button* btn = new Fl_Button(tool_bar_width * i, menu_bar_height, tool_bar_width, tool_bar_height-1);
		i++;
		btn->image(bmp);
		btn->deimage(dergb);
		btn->copy_label(label_conv(label));
		return btn;
	}
	/* browser */
	Fl_Hold_Browser* browser;
	const int browser_width = 140;
	const int browser_height = 250;
	void set_browser()
	{
		Fl_Box* box = new Fl_Box(FL_UP_BOX, 0, menu_bar_height + tool_bar_height, browser_width, 20, "");
		box->copy_label(label_conv("配置文件"));
		browser = new Fl_Hold_Browser(0, menu_bar_height + tool_bar_height + 20, browser_width, browser_height - 20);
		browser->add("tssk32.xml");
		browser->add("tssk64.xml");
		browser->add("tssyskit32.xml");
		browser->add("tssyskit64.xml");
	}
	/* table */
	test_table* table;
	
	void set_table()
	{
		int x = browser_width;
		int y = tool_bar_height + menu_bar_height;
		int w = width - browser_width;
		int h = browser_height;
		table = new test_table(x, y, w, h);
		table->end();
		table->col_header(1);
		table->cols(10);
		table->rows(20);
		table->row_height_all(20);
		//table->col_width(0, 18);
	}
	/* log */
	Fl_Multiline_Output* log;
	Fl_Text_Buffer* log_buf;
	void set_log()
	{
		int i = menu_bar_height + tool_bar_height + browser_height;
		Fl_Box* box = new Fl_Box(FL_UP_BOX, 0, i, width, 20, "");
		box->copy_label(label_conv("日志"));
		log = new Fl_Multiline_Output(0, i+20, width, height - 40 - i);
	}
	/* status */
	Fl_Box* status_box;
	const int status_height = 20;
	void set_status()
	{
		status_box = new Fl_Box(FL_UP_BOX, 0, height-20, width + 1, 20, "ready.");
		status_box->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
	}
};


int main(int argc, char* argv[])
{
	test_win window;
	window.show(1, argv);
	return Fl::run();
}