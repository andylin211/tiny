#include "ctlcode.h"
#include "FL\Fl_Window.H"

int main(int argc, char** argv)
{
	int num = 0;
	wchar_t* str = GetCommandLine();
	wchar_t** a = CommandLineToArgvW(str, &num);
	for (int i = 0; i < num; i++)
		printf("%S\n", a[i]);

	Fl_Window win(400, 400, "Select I/O Control Code");

	ctlcode_t ctlcode;

	win.size(ctlcode.w(), ctlcode.h());

	misc::make_screen_center(&win);

	win.end();

	win.show(1, argv);

	Fl::lock();
	return Fl::run();
}