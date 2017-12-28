#pragma once
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Choice.H>
#include <FL/fl_draw.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Table_Row.H>
#include "FL\Fl_Hold_Browser.H"
#include "FL\Fl_Input.H"
#include "FL\Fl_Slider.H"
#include "FL\Fl_Return_Button.H"
#include "common.h"

class Ioctl_Window;

class ioctl_line;

class Ioctl_Update : public Fl_Window
{
public:
	Ioctl_Update(Ioctl_Window* win);

	~Ioctl_Update();

	void popup(ioctl_line* line);

	int width;
	int height;

	char code_buf[buf_len];
	Fl_Input*  icode;
	static void code_callback(Fl_Widget* o, void* v);
	
	Fl_Input* iinlen;
	Fl_Input* ioulen;

	Fl_Button* bok;
	Fl_Button* bcancel;
	Fl_Return_Button* btncode;

	Fl_Box* itype;
	Fl_Hold_Browser* btype;
	int type_last_value;
	static void type_callback(Fl_Widget* o, void* v);

	Fl_Box* iaccess;
	Fl_Hold_Browser* baccess;
	int access_last_value;
	static void access_callback(Fl_Widget* o, void* v);
	
	Fl_Box* iifunc[3];
	Fl_Slider* ifunc[3];
	static void ifunc_callback0(Fl_Widget* o, void* v);
	static void ifunc_callback1(Fl_Widget* o, void* v);
	static void ifunc_callback2(Fl_Widget* o, void* v);
	void ifunc_callback(int i);

	Fl_Box* imethod;
	Fl_Hold_Browser* bmethod;
	int method_last_value;
	static void method_callback(Fl_Widget* o, void* v);

	void update_code();
	void update_component();
};

