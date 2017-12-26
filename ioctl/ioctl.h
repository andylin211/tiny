#pragma once
#include <process.h>
#include "FL\Fl.H"
#include "FL\Fl_Button.H"
#include "FL\Fl_Box.H"
#include "FL\fl_utf8.h"
#include "FL\Fl_Text_Buffer.H"
#include <FL/Fl_Sys_Menu_Bar.H>
#include "FL\Fl_Text_Display.H"
#include <FL/fl_draw.H>
#include "tinyargs.h"
#include "tinyxml.h"
#include "ioctl_table.h"
#include "ioctl_browser.h"
#include "ioctl_bmp.h"
#include "common.h"
//
//class Scan_Window : public Fl_Window
//{
//public: 
//	Scan_Window();
//	char* device;
//};

#define WIN_W 640
#define WIN_H 480

#define declare_cb_and_thread(name) \
	static void name##_cb(Fl_Widget* o, void* v) { _beginthread(&Ioctl_Window::name##_thread, 0, v); }\
	static void name##_thread(void* data)
	

class Ioctl_Window : public Fl_Window
{
public:
	Ioctl_Window();

	void add_log(wchar_t* wcs);

	static void step_notify(int current, int total, args_t* notify_data);

	void update_table(std::vector<ioctl_line> *line);

	//void resize(int x, int y, int w, int h);

	declare_cb_and_thread(open);
	declare_cb_and_thread(save);
	declare_cb_and_thread(close);
	declare_cb_and_thread(scan);
	declare_cb_and_thread(fuzz);
	declare_cb_and_thread(pause);
	declare_cb_and_thread(stop);
	declare_cb_and_thread(quit);
	
private:
	const int width = WIN_W;
	const int height = WIN_H;
	
	/* menu bar */
	void set_menu_bar();
	const int menu_bar_height = 30;

	/* tool bar */
	Fl_Button* tb_open;
	Fl_Button* tb_save;
	Fl_Button* tb_close;
	Fl_Button* tb_scan;
	Fl_Button* tb_fuzz;
	Fl_Button* tb_pause;
	Fl_Button* tb_stop;
	Fl_Button* tb_quit;
	const int tool_bar_height = 60;
	const int tool_bar_width = 50;
	void set_tool_bar();
	Fl_Button* add_tool_button(char* label, int res, int x=0, int skip=0);

	/* browser */
	Ioctl_Browser* browser;
	const int browser_width = 140;
	const int browser_height = 240;
	void set_browser();

	/* table */
	Ioctl_Table* table;
	void set_table();
	
	/* log */
	Fl_Text_Display* log;
	Fl_Text_Buffer* log_buf;
	void set_log();

	/* status */
	Fl_Box* status_box;
	const int status_height = 20;
	void set_status();

	/* device */
	Fl_Input* device;
	static void device_cb(Fl_Widget* w, void* v);
};

