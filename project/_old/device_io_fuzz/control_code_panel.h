#pragma once
#include "FL\Fl.H"
#include "FL\Fl_Input.H"
#include "FL\Fl_Box.H"
#include "FL\Fl_Hold_Browser.H"
#include "FL\Fl_Button.H"
#include "global.h"
#include <vector>

class bmp_image;

class control_code_panel
{
public:
	control_code_panel(int x, int y, int w, int h);

	void update_code();

	void update_all(char* code);

	Fl_Box* box_outline;

	bmp_image* image;
	Fl_Box* box_image;

	Fl_Box* itype;
	Fl_Hold_Browser* btype;
	int type_last_value;
	static void type_callback(Fl_Widget* o, void* v);

	Fl_Box* iaccess;
	Fl_Hold_Browser* baccess;
	int access_last_value;
	static void access_callback(Fl_Widget* o, void* v);

	Fl_Box* ifunc[3];
	Fl_Slider* sfunc[3];
	int func_value;
	void update_func_value();
	static void func_callback0(Fl_Widget* o, void* v);
	static void func_callback1(Fl_Widget* o, void* v);
	static void func_callback2(Fl_Widget* o, void* v);
	void func_callback(int i);

	Fl_Box* imethod;
	Fl_Hold_Browser* bmethod;
	int method_last_value;
	static void method_callback(Fl_Widget* o, void* v);

};


