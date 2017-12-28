#include "fuzz_panel.h"

fuzz_panel::fuzz_panel(int x, int y, int w, int h)
{
	box_device = new Fl_Box(x + 20, y + 100, 150, 60);
	box_device->labelfont(FL_COURIER_BOLD);
	box_device->labelsize(40);
	box_device->labelcolor(FL_RED);
	box_device->label("tssk");

	box_code = new Fl_Box(x + 20, y + 300, 150, 60);
	box_code->labelfont(FL_COURIER_BOLD);
	box_code->labelsize(40);
	box_code->labelcolor(FL_RED);
	box_code->label("0x222ee3");
}
