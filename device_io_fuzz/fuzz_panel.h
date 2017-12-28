#pragma once
#include "FL\Fl.H"
#include "FL\Fl_Input.H"
#include "FL\Fl_Box.H"
#include "FL\Fl_Hold_Browser.H"
#include "FL\Fl_Button.H"
#include "FL\Fl_Return_Button.H"
#include "global.h"

class fuzz_panel
{
public:
	fuzz_panel(int x, int y, int w, int h);

	Fl_Box* box_device;

	Fl_Box* box_code;

	Fl_Input* inp_inlen;

	Fl_Input* inp_oulen;

	Fl_Return_Button* btn_fuzz;
};

