#pragma once
#include "FL\Fl.H"
#include "FL\Fl_Input.H"
#include "FL\Fl_Box.H"
#include "FL\Fl_Hold_Browser.H"
#include "FL\Fl_Button.H"
#include "global.h"
#include <vector>

class select_device_panel
{
public:
	select_device_panel(int x, int y, int w, int h);

	~select_device_panel();

	void add_all_device();

	void free_v_buf();

	Fl_Box* box_outline;

	Fl_Hold_Browser* browser;

	std::vector<char*> v_buf;
};
