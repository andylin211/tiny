#include "main_frame.h"

main_frame::main_frame()
	:Fl_Window(400,400,800,600),
	device(0, 0, 200, 300),
	fuzz(200,0,300,300),
	code(455,0,345,300)
{
	end();
}


int main(int argc, char* argv[])
{
	if (!is_xp() && !is_evevated())
	{
		RunAs("");
		return 0;
	}

	/* 必须在提权后面，否则没权限打开句柄 */
	if (has_been_running())
	{
		MessageBox(NULL, L"实例已运行！", L"", MB_OK);
		return 0;
	}

	main_frame win;

	win.show(1, argv);

	Fl::lock();

	return Fl::run();
}