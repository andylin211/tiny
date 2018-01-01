#pragma once
#include "FL\Fl_Window.H"
#include "FL\Fl.H"
#include "FL\Fl_Box.H"
#include "FL\Fl_Button.H"
#include "FL\Fl_Return_Button.H"
#include "FL\Fl_Input.H"
#include "fuzz_panel.h"
#include "control_code_panel.h"
#include "select_device_panel.h"
#include "global.h"


class main_frame :
	public Fl_Window
{
public:
	main_frame();
	
	select_device_panel device;
	fuzz_panel fuzz;
	control_code_panel code;
	
};
