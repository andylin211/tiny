#pragma once

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Choice.H>
#include <FL/fl_draw.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Table_Row.H>
#include "tinyxml.h"
#include <vector>
#include "ioctl_browser.h"

class Ioctl_Window;

class Ioctl_Table : public Fl_Table_Row
{
public:
	Ioctl_Table(Ioctl_Window* parent, int x, int y, int w, int h, const char *l = 0);

	void update(ioctl_table* t);

private:
	static void event_callback(Fl_Widget*, void*);

	void event_callback2();				// callback for table events

	void draw_cell(TableContext context, int r, int c, int x, int y, int w, int h);

public:
	int width;

	int min_row;

	int selected_row;

	ioctl_table* itable;

	Ioctl_Window* parent;

	Fl_Button* btn;

	Fl_Input* inp;
};
