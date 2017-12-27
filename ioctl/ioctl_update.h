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

class Ioctl_Window;

class Ioctl_Update : public Fl_Window
{
public:
	Ioctl_Update(Ioctl_Window* win);

	~Ioctl_Update();

	Fl_Input*  input_code;

	Fl_Hold_Browser* btype;
	Fl_Hold_Browser* baccess;
	Fl_Hold_Browser* bfunc;
	Fl_Hold_Browser* bbuffer;
};

