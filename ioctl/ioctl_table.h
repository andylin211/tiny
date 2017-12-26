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

class Ioctl_Table : public Fl_Table_Row
{
public:
	Ioctl_Table(int x, int y, int w, int h, const char *l = 0);

	void update(std::vector<ioctl_line> *v_line);

	void min_row(int i) { min_row_ = i; }
private:
	static void event_callback(Fl_Widget*, void*);

	void event_callback2();				// callback for table events

	void draw_cell(TableContext context, int r, int c, int x, int y, int w, int h);

private:
	int min_row_;

	std::vector<ioctl_line> *v_line;
};
