#include "control_code_panel.h"
#include "main_frame.h"
#include <assert.h>
#include "resource.h"
#include "bmp_image.h"
#include "global.h"

control_code_panel::control_code_panel(int x, int y, int w, int h)
{
	box_outline = new Fl_Box(x, y+20, w-5, h);
	set_label_font(box_outline);
	box_outline->align(FL_ALIGN_TOP);
	box_outline->box(FL_GTK_UP_BOX);
	box_outline->copy_label(label_conv("±à¼­·ÖÁ¿"));

	image = new bmp_image(IDR_CONTROL_CODE);
	box_image = new Fl_Box(x, y+30, 340, 50);
	box_image->image(image);

	x = x + 10;
	y = y + 80;
	w = 142;
	h = h - 80 - 5;

	itype = new Fl_Box(x, y, w, 20);
	set_label_font(itype);
	itype->label(device_type_unknown_hex());
	btype = new Fl_Hold_Browser(x, y + 20, w, h - 20);
	set_text_font(btype);
	for (int i = 0; i < max_device_type; i++)
		btype->add(device_type_str(i));
	btype->callback(type_callback, this);
	btype->value(FILE_DEVICE_UNKNOWN);

	x += (w + 5);
	w = 40;
	iaccess = new Fl_Box(x, y, w, 20);
	set_label_font(iaccess);
	iaccess->label(hex_str_0x(0));
	baccess = new Fl_Hold_Browser(x, y + 20, w, h - 20);
	set_text_font(baccess);
	for (int i = 0; i < 4; i++)
		baccess->add(access_str(i));
	baccess->value(1);
	baccess->callback(access_callback, this);

	x += 70;
	w = 7;
	Fl_Box* b = new Fl_Box(x - 2 * w, y, w * 2, 20);
	set_label_font(b);
	b->label("0x");
	ifunc[0] = new Fl_Box(x, y, w, 20);
	ifunc[1] = new Fl_Box(x + w, y, w, 20);
	ifunc[2] = new Fl_Box(x + 2 * w, y, w, 20);
	x -= 25;
	w = 20;
	sfunc[0] = new Fl_Slider(x, y + 20, w, h - 20);
	sfunc[1] = new Fl_Slider(x + w, y + 20, w, h - 20);
	sfunc[2] = new Fl_Slider(x + 2 * w, y + 20, w, h - 20);
	sfunc[0]->callback(func_callback0, this);
	sfunc[1]->callback(func_callback1, this);
	sfunc[2]->callback(func_callback2, this);
	for (int i = 0; i < 3; i++)
	{
		sfunc[i]->minimum(0);
		sfunc[i]->maximum(15);
		sfunc[i]->step(1);
		sfunc[i]->value(0xf);
		set_label_font(ifunc[i]);
		ifunc[i]->label("0");
	}
	sfunc[0]->value(7);
	ifunc[0]->label("8");

	x += 3 * w + 5;
	w = 66;
	imethod = new Fl_Box(x, y, w, 20);
	set_label_font(imethod);
	imethod->label(hex_str_0x(0));
	bmethod = new Fl_Hold_Browser(x, y + 20, w, h - 20);
	set_text_font(bmethod);
	for (int i = 0; i < 4; i++)
		bmethod->add(method_str(i));
	bmethod->value(1);
	bmethod->callback(method_callback, this);
}

void control_code_panel::update_code()
{

}

void control_code_panel::type_callback(Fl_Widget* o, void* v)
{
	control_code_panel* obj = (control_code_panel*)v;
	if (!obj->btype->value())
		obj->btype->value(obj->type_last_value);
	obj->itype->label(device_type_hex(obj->btype->value() - 1));
	obj->update_code();
	obj->type_last_value = obj->btype->value();
}

void control_code_panel::access_callback(Fl_Widget* o, void* v)
{
	control_code_panel* obj = (control_code_panel*)v;
	if (!obj->baccess->value())
		obj->baccess->value(obj->access_last_value);
	obj->iaccess->label(hex_str_0x(obj->baccess->value() - 1));
	obj->update_code();
	obj->access_last_value = obj->baccess->value();
}
void control_code_panel::method_callback(Fl_Widget* o, void* v)
{
	control_code_panel* obj = (control_code_panel*)v;
	if (!obj->bmethod->value())
		obj->bmethod->value(obj->method_last_value);
	obj->imethod->label(hex_str_0x(obj->bmethod->value() - 1));
	obj->update_code();
	obj->method_last_value = obj->bmethod->value();
}

void control_code_panel::func_callback(int i)
{
	ifunc[i]->label(hex_digit_str((int)(15 - sfunc[i]->value())));
	update_func_value();
	update_code();
}

void control_code_panel::func_callback0(Fl_Widget* o, void* v)
{
	control_code_panel* up = (control_code_panel*)v;
	up->func_callback(0);
}

void control_code_panel::func_callback1(Fl_Widget* o, void* v)
{
	control_code_panel* up = (control_code_panel*)v;
	up->func_callback(1);
}

void control_code_panel::func_callback2(Fl_Widget* o, void* v)
{
	control_code_panel* up = (control_code_panel*)v;
	up->func_callback(2);
}

void control_code_panel::update_func_value()
{
	func_value = 0xf - (int)sfunc[0]->value();
	func_value = (func_value << 4) + 0xf - (int)sfunc[1]->value();
	func_value = (func_value << 4) + 0xf - (int)sfunc[2]->value();
}

static void select_line(Fl_Hold_Browser* b, char* v)
{
	assert(b);
	if (v)
	{
		for (int i = 1; i <= b->size(); i++)
		{
			if (!strcmp(b->text(i), v))
			{
				b->value(i);
				break;
			}
		}
	}
}

static void update_browser(Fl_Hold_Browser* b, int default_line = 0, char* v = 0)
{
	assert(b);
	b->value(default_line);
	select_line(b, v);
	b->do_callback();
}

static void update_func(char* func, Fl_Slider** s)
{
	s[0]->value(7);
	s[0]->value(0xf);
	s[0]->value(0xf);
	assert(s);
	if (func && strlen(func) == 3)
	{
		for (int i = 0; i < 3; i++)
			s[i]->value(0xf - ctoi(func[i]));
	}
	for (int i = 0; i < 3; i++)
		s[i]->do_callback();
}

void control_code_panel::update_all(char* code_value)
{
	if (!code_value)
		return;

	unsigned long code = strtol(code_value, 0, 16);
	unsigned type = code >> 16;
	unsigned access = code >> 14 & 0x3;
	unsigned func = code >> 2 & 0xfff;
	unsigned method = code & 0x3;
	if (type > max_device_type)
		btype->value(FILE_DEVICE_UNKNOWN - 1);
	else
		btype->value(type);
	btype->do_callback();

	baccess->value(access + 1);
	baccess->do_callback();

	bmethod->value(method + 1);
	bmethod->do_callback();

	sfunc[0]->value(0xf - ((func >> 8) & 0xf));
	sfunc[1]->value(0xf - ((func >> 4) & 0xf));
	sfunc[2]->value(0xf - (func & 0xf));
	for (int i = 0; i < 3; i++)
		ifunc[i]->do_callback();
}
