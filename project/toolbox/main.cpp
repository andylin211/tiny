#include "toolbox.h"
#include "FL\Fl_Window.H"
#include "tinyshell.h"

void print(char* str, void*)
{
	printf(str);
}

int main(int argc, char** argv)
{
	toolbox_t tb;

	tb.show(1, argv);

	Fl::lock();
	return Fl::run();
}