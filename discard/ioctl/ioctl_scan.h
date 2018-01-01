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

class ioctl_scan : public Fl_Window
{
public:
	ioctl_scan(Ioctl_Window* win);

	~ioctl_scan();

	void popup();

	Ioctl_Window* parent;

	int width, height;
};

