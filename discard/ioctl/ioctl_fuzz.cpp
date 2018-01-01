#include "ioctl_fuzz.h"
#include "ioctl_win.h"

Ioctl_Fuzz::Ioctl_Fuzz(Ioctl_Window* win)
	:Fl_Window(400, 400, 400, 400)
{
	width = 230;
	height = 200;
	resize(win->x() + win->w() / 2 - width / 2, win->y() + win->h() / 2 - height / 2, width, height);
	set_modal();
	hide();

	int x = 100;
	int y = 10;
	int w = 100;
	int h = 20;
	idevice = new Fl_Input(x, y, w, h);
	idevice->copy_label(label_conv("设备名:"));
	set_label_font_12(idevice);

	bdevice = new Fl_Button(x + w + 10, y, 20, 20, "..");

	y += 30;
	icode = new Fl_Input(x, y, w, h);
	icode->copy_label(label_conv("控制码(hex):"));
	set_label_font_12(idevice);

	bcode = new Fl_Button(x + w + 10, y, 20, 20, "..");

	y += 30;
	iinlen = new Fl_Input(x, y, w, h);
	iinlen ->copy_label(label_conv("字节数(in):"));
	set_label_font_12(iinlen);

	y += 30;
	ioulen = new Fl_Input(x, y, w, h);
	ioulen->copy_label(label_conv("字节数(out):"));
	set_label_font_12(ioulen);

	y += 30;
	bfuzz = new Fl_Return_Button(x, y, w, h, "Fuzz");
	set_label_font_12(bfuzz);

	end();

}


Ioctl_Fuzz::~Ioctl_Fuzz()
{
}


void Ioctl_Fuzz::popup()
{
	show();
}