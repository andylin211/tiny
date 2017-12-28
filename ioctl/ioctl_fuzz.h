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
#include "tinyxml.h"

class Ioctl_Window;

class Ioctl_Fuzz : public Fl_Window
{
public:
	Ioctl_Fuzz(Ioctl_Window* win);

	~Ioctl_Fuzz();

	void popup();

	Ioctl_Window* parent;

	int width, height;

	Fl_Input* idevice;
	Fl_Button* bdevice;
	Fl_Input* icode;
	Fl_Button* bcode;
	Fl_Input* iinlen;
	Fl_Input* ioulen;

	Fl_Return_Button* bfuzz;
};

