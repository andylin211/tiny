#pragma once

#include <FL/Fl.H>
#include <FL/Fl_Hold_Browser.H>
#include <FL/Fl_Box.H>
#include <map>
#include <vector>
#include "tinyxml.h"

class Ioctl_Window;

typedef struct {
	char* device;
	char* code;
	char* type;
	char* func;
	char* method;
	char* access;
}ioctl_line;

class Ioctl_Browser : public Fl_Hold_Browser
{
public:

	Ioctl_Browser(int x, int y, int w, int h, const char *l = 0);

	void set_top_window(Ioctl_Window* w) { win = w; }

	static void call_back(Fl_Widget* o, long);

	/* thread safe */
	void add_(wchar_t* file, xml_document* doc);

	/* thread safe */
	void close_();

private:
	/* thread safe */
	void update_(char* str);

	std::map<char*, std::vector<ioctl_line>*> map_line;

	void free_ioctl_line(ioctl_line* li);

	void free_map_line(std::vector<ioctl_line>* line);

	void add_map_line(wchar_t* file, std::vector<ioctl_line>* line);

	Ioctl_Window* win;
};