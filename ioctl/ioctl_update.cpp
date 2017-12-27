#include "ioctl_update.h"
#include "ioctl_win.h"
#include <assert.h>
#include "resource.h"
#include "ioctl_bmp.h"
#include "common.h"

Ioctl_Update::Ioctl_Update(Ioctl_Window* win)
	:Fl_Window(400,400,400,300)
{
	assert(win);
	resize(win->x() + 100, win->y() + 200, 800, 300);
	set_modal();

	Ioctl_Bmp* bmp = new Ioctl_Bmp(IDR_IOCTL_CODE);
	Fl_Image *dergb;
	dergb = bmp->copy();
	dergb->inactive();
	Fl_Box* box = new Fl_Box(20, 20, 340, 50);
	box->image(bmp);
	box->deimage(dergb);

	btype = new Fl_Hold_Browser(20, 80, 200, 200);
	btype->textsize(12);
	btype->textfont(FL_HELVETICA);
	for (int i = 0; i < type_def_size; i++)
		btype->add(type_def[i].v3);
	
	baccess = new Fl_Hold_Browser(220, 80, 200, 200);
	baccess->textsize(12);
	baccess->textfont(FL_HELVETICA);
	baccess->add("FILE_READ_ACCESS");
	baccess->add("FILE_WRITE_ACCESS");
	baccess->add("FILE_READ_ACCESS|FILE_WRITE_ACCESS");



	end();
}


Ioctl_Update::~Ioctl_Update()
{
}
