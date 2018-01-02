#include "toolbox.h"
#include "FL\Fl_Window.H"
#include "tinyshell.h"

void print(char* str, void*)
{
	printf(str);
}

int main(int argc, char** argv)
{
	exec_script("c:\\tiny\\shell\\bash.exe", "c:\\tiny\\script\\newproj.sh", "make", print, 0);
	return 0;
	/*Fl_Window win(400, 400, "Select I/O Control Code");

	toolbox_t tb;

	win.size(tb.w(), tb.h());

	fltk_t::make_screen_center(&win);

	win.end();

	win.show(1, argv);

	Fl::lock();
	return Fl::run();*/
}