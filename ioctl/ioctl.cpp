#include "ioctl.h"
#include "tinystr.h"
#include "resource.h"
#include "ioctl_fuzz.h"
#include "FL\Fl_Input.H"

//Scan_Window::Scan_Window()
//	:Fl_Window(400, 300)
//{
//	//set_flag(MENU_WINDOW);
//}

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

Fl_Menu_Item menu_table[] = {
	{ menu_file, 0,0,0,FL_SUBMENU },
	{ menu_open,	FL_CTRL + 'o', &Ioctl_Window::open_cb, 0 },
	{ menu_save,	FL_CTRL + 's', &Ioctl_Window::save_cb, 0 },
	{ menu_close,	FL_CTRL + FL_ALT + 'c',	&Ioctl_Window::close_cb, 0, FL_MENU_DIVIDER },
	{ menu_quit,	FL_CTRL + 'q', &Ioctl_Window::quit_cb, 0 },
	{ 0 },
	{ menu_scan, 0,0,0,FL_SUBMENU },
	{ menu_scan_device,	FL_ALT + 'd', 0, 0 },
	{ menu_scan_file,	0, &Ioctl_Window::scan_cb, 0 },
	{ 0 },
	{ menu_fuzz, 0,0,0,FL_SUBMENU },
	{ menu_fuzz_go,	FL_ALT + FL_SHIFT + 'f', &Ioctl_Window::fuzz_cb, 0 },
	{ menu_fuzz_pause,	0, &Ioctl_Window::pause_cb, 0 },
	{ menu_fuzz_stop,	0, &Ioctl_Window::stop_cb, 0 },
	{ 0 },
	{ 0 }
};

Ioctl_Window::Ioctl_Window()
	:Fl_Window(WIN_W, WIN_H)
{
	copy_label(label_conv("设备驱动ioctl fuzz工具（测试）"));

	set_menu_bar();
	set_tool_bar();
	set_browser();
	set_table();
	set_log();
	set_status();

	labelfont(FL_HELVETICA);
	labelsize(12);
	end();

	//resizable(this);
	//Fl::scheme("gtk+")
}

void Ioctl_Window::set_menu_bar()
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
	for (int i = 0; i < sizeof(menu_table)/sizeof(menu_table[0]); i++)
	{
		menu_table[i].labelfont(FL_HELVETICA);
		menu_table[i].labelsize(12);
		menu_table[i].user_data(this);
	}
}

#define default_device_str "device name.."

void Ioctl_Window::device_cb(Fl_Widget* w, void* v)
{
	Fl_Input* in = (Fl_Input*)w;
	if (!in->size())
		in->value(default_device_str);
	in->redraw();
}

void Ioctl_Window::set_tool_bar()
{
	Fl_Box* box = new Fl_Box(FL_UP_BOX, 0, menu_bar_height, width + 1, tool_bar_height, "");

	Fl_Box* b_dev = new Fl_Box(FL_NO_BOX, 2, menu_bar_height + 10, 95, 20, "");
	b_dev->copy_label(label_conv("设备名:"));
	b_dev->align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE);
	b_dev->labelsize(12);
	b_dev->labelfont(FL_HELVETICA);

	device = new Fl_Input(2, menu_bar_height + 30, 95, 20);
	device->value(default_device_str);
	device->textcolor(FL_BLACK);
	device->textfont(FL_HELVETICA);
	device->textsize(12);
	device->cursor_color(0x60606000);
	device->color(0xf0f0f000);
	device->box(FL_GTK_UP_BOX);
	device->callback(device_cb, this);
	//device->when(FL_WHEN_ENTER_KEY_CHANGED);

	//device->clear_visible_focus();
	//device->position(0);
	//device->mark(device->size());
	//device->copy_label(label_conv("设备:"));

	tb_scan = add_tool_button("扫描", IDR_SCAN, 0, 100);
	tb_open = add_tool_button("打开", IDR_OPEN);
	tb_save = add_tool_button("保存", IDR_SAVE);
	tb_close = add_tool_button("关闭", IDR_CLOSE);
	tb_fuzz = add_tool_button("Fuzz", IDR_FUZZ);
	tb_pause = add_tool_button("暂停", IDR_PAUSE);
	tb_stop = add_tool_button("停止", IDR_STOP);
	tb_quit = add_tool_button("退出", IDR_QUIT);
	//tb_save->deactivate();
	
#define tb_call_back(name) tb_##name->callback(name##_cb, this)
	tb_call_back(open);
	tb_call_back(save);
	tb_call_back(close);
	tb_call_back(scan);
	tb_call_back(fuzz);
	tb_call_back(pause);
	tb_call_back(stop);
	tb_call_back(quit);
}
Fl_Button* Ioctl_Window::add_tool_button(char* label, int res, int x, int skip)
{
	static int i = 0;
	static int skip_ = 0;
	Ioctl_Bmp* bmp = new Ioctl_Bmp(res);
	Fl_Image *dergb;
	dergb = bmp->copy();
	dergb->inactive();
	
	if (skip)
		skip_ += skip;

	Fl_Button* btn = 0;
	if (!x)
	{
		btn = new Fl_Button(skip_ + tool_bar_width * i, menu_bar_height, tool_bar_width, tool_bar_height - 1);
		i++;
	}
	else
		btn = new Fl_Button(x, menu_bar_height, tool_bar_width, tool_bar_height - 1);
	
	btn->image(bmp);
	btn->deimage(dergb);
	btn->copy_label(label_conv(label));
	set_label_font_12(btn);
	return btn;
}

void Ioctl_Window::set_browser()
{
	browser = new Ioctl_Browser(0, menu_bar_height + tool_bar_height, browser_width, browser_height);
	browser->set_top_window(this);
	browser->callback(&Ioctl_Browser::call_back);
}

void Ioctl_Window::set_table()
{
	int x = browser_width;
	int y = tool_bar_height + menu_bar_height;
	int w = width - browser_width;
	int h = browser_height;
	table = new Ioctl_Table(x, y, w, h);
	table->end();
	table->col_header(1);	
	table->cols(5);
	table->width = w - 2;
	table->col_width_all(table->width / 5);
	table->min_row = (browser_height / 20 - 1);
	table->rows(table->min_row);
	table->row_height_all(20);
	table->selection_color(FL_YELLOW);
	table->col_resize(0);
	table->row_resize(0);
	//table->update(0);
}

void Ioctl_Window::set_log()
{
	int i = menu_bar_height + tool_bar_height + browser_height;
	Fl_Box* box = new Fl_Box(FL_UP_BOX, 0, i, width, 20, "");
	box->copy_label(label_conv("日志"));

	log_buf = new Fl_Text_Buffer();
	log = new Fl_Text_Display(0, i + 20, width, height - 40 - i);
	set_label_font_12(box);
	log->buffer(log_buf);                 // attach text buffer to display widget
	log->wrap_mode(Fl_Text_Display::WRAP_AT_COLUMN, log->textfont());
	log->textfont(FL_HELVETICA);
	log->textsize(12);
}

void Ioctl_Window::set_status()
{
	status_box = new Fl_Box(FL_UP_BOX, 0, height - 20, width + 1, 20, "ready.");
	status_box->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
	set_label_font_12(status_box);
}

void Ioctl_Window::add_log(wchar_t* wcs)
{
	if (!wcs) return;

	char* str = wcs_to_str(wcs, -1, encoding_utf8);

	Fl::lock();

	log_buf->insert(log_buf->length(), str);
	log_buf->insert(log_buf->length(), "\n");
	log->scroll(log_buf->count_lines(0, log_buf->length()), 0);
	log->redraw();
	Fl::unlock();
	
	free(str);
}

void Ioctl_Window::step_notify(int current, int total, args_t* notify_data)
{
	Ioctl_Window* window = (Ioctl_Window*)notify_data->p0;
	wchar_t* msg = (wchar_t*)notify_data->p1;

	window->add_log(msg);
}

void Ioctl_Window::update_table(ioctl_table* t)
{
	table->update(t);
	if (t)
		device->value(t->device);
	else
		device->value(default_device_str);
}

void Ioctl_Window::open_thread(void* data)
{
	char* buf = 0;
	Ioctl_Window* win = (Ioctl_Window*)data;
	FILE* fp = 0;

	do
	{
		if (!win)
			break;

		wchar_t* str = ask_open_file(fl_window);
		if (!str)
			break;

		buf = read_file_raw(str);
		if (!buf)
			break;

		xml_document* tmp_doc;
		xml_load_document(buf, encoding_ansi, &tmp_doc);

		if (!tmp_doc)
			break;
		
		win->browser->add_(str, tmp_doc);

		xml_free_document(tmp_doc);

	} while (0);
	
	free(buf);
	if (fp)
		fclose(fp);
}

void Ioctl_Window::save_thread(void* data)
{
	char* buf = 0;
	Ioctl_Window* win = (Ioctl_Window*)data;

	/*wchar_t* str = ask_save_file(fl_window);
	if (str)
	{
		buf = wcs_to_str(str, -1, encoding_ansi);
		printf("save %s\n", buf);
	}
	free(buf);*/
}

void Ioctl_Window::close_thread(void* data)
{
	Ioctl_Window* win = (Ioctl_Window*)data;
	win->browser->close_();
}

void Ioctl_Window::scan_thread(void* data)
{
	Ioctl_Window* win = (Ioctl_Window*)data;
	const char* v = win->device->value();
	if (v && strcmp(v, default_device_str))
	{
		wchar_t* buf = str_to_wcs((char*)v, -1, encoding_utf8);
		args_t args;
		args.p0 = win;
		scan(buf, L"c:\\test.xml", step_notify, &args);
	}
}

void Ioctl_Window::fuzz_thread(void* data)
{
	Ioctl_Window* win = (Ioctl_Window*)data;
	//fuzz()
}

void Ioctl_Window::pause_thread(void* data)
{

}

void Ioctl_Window::stop_thread(void* data)
{

}

void Ioctl_Window::quit_thread(void* data)
{
	Ioctl_Window* win = (Ioctl_Window*)data;
	//win->hide();
}

int main(int argc, char* argv[])
{
	if (!is_xp() && !is_evevated())
	{
		RunAs("");
		return 0;
	}

	/* 必须在提权后面，否则没权限打开句柄 */
	if (has_been_running())
	{
		MessageBox(NULL, L"实例已运行！", L"", MB_OK);
		return 0;
	}

	Ioctl_Window window;

	window.show(1, argv);

	Fl::lock();

	return Fl::run();
}

