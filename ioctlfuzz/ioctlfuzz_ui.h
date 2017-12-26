#ifndef _ioctlfuzz_ui_h_
#define _ioctlfuzz_ui_h_

#include "FL\Fl.H"
#include "FL\Fl_Double_Window.H"
#include "FL\Fl_Button.H"
#include "FL\Fl_Return_Button.H"
#include "Fl\Fl_Choice.H"
#include "FL\fl_utf8.h"
#include "FL\Fl_Text_Buffer.H"
#include "FL\Fl_Text_Editor.H"
#include "FL\Fl_Text_Display.H"

class Virus_Window : public Fl_Double_Window
{
public:
	Virus_Window();

	void add_log(wchar_t* wcs);

	~Virus_Window();

	void init_choice();

	static void choice_cb(Fl_Choice* o, void*);

	static void btn_do_thread(void* data);
	static void btn_do_cb(Fl_Widget* o, void* v)
	{
		_beginthread(&Virus_Window::btn_do_thread, 0, v);
	}

	static void btn_doc_thread(void* data);
	static void btn_doc_cb(Fl_Widget* o, void* v)
	{
		_beginthread(&Virus_Window::btn_doc_thread, 0, v);
	}

	static void btn_sample_thread(void* data);
	static void btn_sample_cb(Fl_Widget* o, void* v)
	{
		_beginthread(&Virus_Window::btn_sample_thread, 0, v);
	}

	static void btn_pchunter_thread(void* data);
	static void btn_pchunter_cb(Fl_Widget* o, void* v)
	{
		_beginthread(&Virus_Window::btn_pchunter_thread, 0, v);
	}

	static void btn_reboot_thread(void* data);
	static void btn_reboot_cb(Fl_Widget* o, void* v)
	{
		_beginthread(&Virus_Window::btn_reboot_thread, 0, v);
	}

	static void btn_uac_thread(void* data);
	static void btn_uac_cb(Fl_Widget* o, void* v)
	{
		_beginthread(&Virus_Window::btn_uac_thread, 0, v);
	}

	static void step_notify(args_t* notify_data);

private:


	Fl_Choice* choice;
	Fl_Return_Button* btn_do;
	Fl_Button* btn_doc;
	Fl_Button* btn_sample;
	Fl_Button* btn_pchunter;
	Fl_Button* btn_reboot;
	Fl_Button* btn_uac;
	Fl_Text_Buffer* log_buf;
	Fl_Text_Display* log;

	char title_buf[256];
	char choice_buf[256];
	char btn_do_buf[256];
	char btn_doc_buf[256];
	char btn_sample_buf[256];
	char btn_pchunter_buf[256];
	char btn_reboot_buf[256];
	char btn_uac_buf[256];
};

#endif // !_ioctlfuzz_ui_h_
