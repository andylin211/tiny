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

class Ioctl_Window;

class Ioctl_Update : public Fl_Window
{
public:
	Ioctl_Update(Ioctl_Window* win, char* type=0, char* access=0, char* func=0, char* method=0, int inlen=0, int oulen=0);

	~Ioctl_Update();

	int width;
	int height;

	Fl_Input*  icode;
	
	Fl_Input* iinlen;
	Fl_Input* ioulen;

	Fl_Return_Button* bok;
	Fl_Button* bcancel;

	Fl_Box* itype;
	Fl_Hold_Browser* btype;
	static void type_callback(Fl_Widget* o, void* v);

	Fl_Box* iaccess;
	Fl_Hold_Browser* baccess;
	static void access_callback(Fl_Widget* o, void* v);
	
	Fl_Box* iifunc[3];
	Fl_Slider* ifunc[3];
	static void ifunc_callback0(Fl_Widget* o, void* v);
	static void ifunc_callback1(Fl_Widget* o, void* v);
	static void ifunc_callback2(Fl_Widget* o, void* v);
	void ifunc_callback(int i);

	Fl_Box* imethod;
	Fl_Hold_Browser* bmethod;
	static void method_callback(Fl_Widget* o, void* v);
};

