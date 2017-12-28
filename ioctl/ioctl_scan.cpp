#include "ioctl_scan.h"
#include "ioctl_win.h"

ioctl_scan::ioctl_scan(Ioctl_Window* win)
	:Fl_Window(400,400,400,400)
{
	width = 360;
	height = 340;
	resize(win->x() + win->w() / 2 - width / 2, win->y() + win->h() / 2 - height / 2, width, height);
	set_modal();
	hide();

	end();

}


ioctl_scan::~ioctl_scan()
{
}


void ioctl_scan::popup()
{
	show();
}