#include "ioctl_update.h"
#include "ioctl_win.h"
#include <assert.h>
#include "resource.h"
#include "ioctl_bmp.h"
#include "common.h"

char* label_hex[] = { "0", "1", "2","3","4","5","6","7","8","9","a","b","c","d","e","f", };
char* type_str_def[] = {
	"beep",//0x1
	"cd rom",
	"cd rom file system",
	"controller",
	"datalink",
	"dfs",
	"disk",
	"disk file system",
	"file system",
	"inport port",
	"keyboard",
	"mailslot",
	"midi in",
	"midi out",
	"mouse",
	"multi unc provider",
	"named pipe",
	"network",
	"network browser",
	"network file system",
	"null",
	"parallel port",
	"physical netcard",
	"printer",
	"scanner",
	"serial mouse port",
	"serial port",
	"screen",
	"sound",
	"streams",
	"tape",
	"tape file system",
	"transport",
	"unknown",
	"video",
	"virtual disk",
	"wave in",
	"wave out",
	"8042 port",
	"network redirector",
	"battery",
	"bus extender",
	"modem",
	"vdm",
	"mass storage",
	"smb",
	"ks",
	"changer",
	"smartcard",
	"acpi",
	"dvd",
	"fullscreen video",
	"dfs file system",
	"dfs volume",
	"serenum",
	"termsrv",
	"ksec",
	"fips",
	"infiniband",
	"vmbus",
	"crypt provider",
	"wpd",
	"bluetooth",
	"mt composite",
	"mt transport",
	"biometric	",
	"pmi",
};

char* type_hex_def[] = {
	"0x0001",
	"0x0002",
	"0x0003",
	"0x0004",
	"0x0005",
	"0x0006",
	"0x0007",
	"0x0008",
	"0x0009",
	"0x000a",
	"0x000b",
	"0x000c",
	"0x000d",
	"0x000e",
	"0x000f",
	"0x0010",
	"0x0011",
	"0x0012",
	"0x0013",
	"0x0014",
	"0x0015",
	"0x0016",
	"0x0017",
	"0x0018",
	"0x0019",
	"0x001a",
	"0x001b",
	"0x001c",
	"0x001d",
	"0x001e",
	"0x001f",
	"0x0020",
	"0x0021",
	"0x0022",
	"0x0023",
	"0x0024",
	"0x0025",
	"0x0026",
	"0x0027",
	"0x0028",
	"0x0029",
	"0x002a",
	"0x002b",
	"0x002c",
	"0x002d",
	"0x002e",
	"0x002f",
	"0x0030",
	"0x0031",
	"0x0032",
	"0x0033",
	"0x0034",
	"0x0035",
	"0x0036",
	"0x0037",
	"0x0038",
	"0x0039",
	"0x003A",
	"0x003B",
	"0x003E",
	"0x003F",
	"0x0040",
	"0x0041",
	"0x0042",
	"0x0043",
	"0x0044",
	"0x0045",
};

static char read[buf_len];
static char write[buf_len];
static char any[buf_len];
static char both[buf_len];

static void buf_gbk_to_utf8(char* buf, char* src)
{
	fl_utf8from_mb(buf, buf_len, src, strlen(src));
}

char* access_str_def[] = {
	any,
	read,
	write,
	both
};

char* method_str_def[] = {
	"buffered",
	"in direct",
	"out direct",
	"neither",
};

char* hex_str_0x[] = {
	"0x0",
	"0x1",
	"0x2",
	"0x3",
};

unsigned long make_ioctl_code(unsigned long device_type, unsigned long function, unsigned long method, unsigned long access)
{
	return (((device_type) << 16) | ((access) << 14) | ((function) << 2) | (method));
}

static int ctoi(char ch)
{
	if (ch >= '0' && ch <= '9')
		return ch - '0';
	if (ch >= 'a' && ch <= 'f')
		return ch - 'a' + 10;
	if (ch >= 'A' && ch <= 'F')
		return ch - 'A' + 10;
	return 0;
}

static int get_func(Fl_Slider** s)
{
	assert(s);
	unsigned i = 0xf - (int)s[0]->value();
	i = (i << 4) + 0xf - (int)s[1]->value();
	i = (i << 4) + 0xf - (int)s[2]->value();
	return i;
}

void Ioctl_Update::update_code()
{
	unsigned long code = make_ioctl_code(btype->value(), get_func(ifunc), bmethod->value() - 1, baccess->value() - 1);
	sprintf(code_buf, "%x", code);
	icode->value(code_buf);
}

void Ioctl_Update::update_component()
{
	const char* buf = icode->value();
	unsigned long code = strtol(buf, 0, 16);
	unsigned type = code >> 16;
	unsigned access = code >> 14 & 0x3;
	unsigned func = code >> 2 & 0xfff;
	unsigned method = code & 0x3;
	if (type > sizeof(type_str_def) / sizeof(type_str_def[0]))
		type = FILE_DEVICE_UNKNOWN;
	btype->value(type);
	btype->do_callback();
	baccess->value(access + 1);
	baccess->do_callback();
	bmethod->value(method + 1);
	bmethod->do_callback();
	ifunc[0]->value(0xf - ((func >> 8) & 0xf));
	ifunc[1]->value(0xf - ((func >> 4) & 0xf));
	ifunc[2]->value(0xf - (func & 0xf));
	for (int i = 0; i < 3; i++)
		ifunc[i]->do_callback();

	update_code();
}

void Ioctl_Update::code_callback(Fl_Widget* o, void* v)
{
	Ioctl_Update* up = (Ioctl_Update*)v;
	up->update_component();
}

void Ioctl_Update::type_callback(Fl_Widget* o, void* v)
{
	Ioctl_Update* up = (Ioctl_Update*)v;
	if (!up->btype->value())
		up->btype->value(up->type_last_value);
	up->itype->label(type_hex_def[up->btype->value() - 1]);
	up->update_code();
	up->type_last_value = up->btype->value();
}
void Ioctl_Update::access_callback(Fl_Widget* o, void* v)
{
	Ioctl_Update* up = (Ioctl_Update*)v;
	if (!up->baccess->value())
		up->baccess->value(up->access_last_value);
	up->iaccess->label(hex_str_0x[up->baccess->value() - 1]);
	up->update_code();
	up->access_last_value = up->baccess->value();
}
void Ioctl_Update::method_callback(Fl_Widget* o, void* v)
{
	Ioctl_Update* up = (Ioctl_Update*)v;
	if (!up->bmethod->value())
		up->bmethod->value(up->method_last_value);
	up->imethod->label(hex_str_0x[up->bmethod->value() - 1]);
	up->update_code();
	up->method_last_value = up->bmethod->value();
}

void Ioctl_Update::ifunc_callback(int i)
{
	iifunc[i]->label(label_hex[(int)(15 - ifunc[i]->value())]);
}

void Ioctl_Update::ifunc_callback0(Fl_Widget* o, void* v)
{
	Ioctl_Update* up = (Ioctl_Update*)v;
	up->ifunc_callback(0);
	up->update_code();
}
void Ioctl_Update::ifunc_callback1(Fl_Widget* o, void* v)
{
	Ioctl_Update* up = (Ioctl_Update*)v;
	up->ifunc_callback(1);
	up->update_code();
}
void Ioctl_Update::ifunc_callback2(Fl_Widget* o, void* v)
{
	Ioctl_Update* up = (Ioctl_Update*)v;
	up->ifunc_callback(2);
	up->update_code();
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


void Ioctl_Update::popup(ioctl_line* line)
{
	if (!line)
		return;

	btype->value(FILE_DEVICE_UNKNOWN);
	if (line->type && strcmp(type_str_def[FILE_DEVICE_UNKNOWN - 1], line->type))// not unknown, rare
		update_browser(btype, FILE_DEVICE_UNKNOWN, line->type);

	update_browser(baccess, 0, line->access);
	update_browser(bmethod, 0, line->method);

	update_func(line->func, ifunc);

	if (line->code)
		icode->value(line->code);

	if (line->inlen)
		iinlen->value(line->inlen);
	else
		iinlen->value("0");

	if (line->oulen)
		ioulen->value(line->oulen);
	else
		ioulen->value("0");

	show();
}

Ioctl_Update::Ioctl_Update(Ioctl_Window* win)
	:Fl_Window(400,400,400,300)
{
	assert(win);
	buf_gbk_to_utf8(read, "只读");
	buf_gbk_to_utf8(write, "只写");
	buf_gbk_to_utf8(any, "任意");
	buf_gbk_to_utf8(both, "读写");
	
	width = 360;
	height = 340;
	resize(win->x() + win->w()/2 - width/2, win->y() + win->h()/2 - height/2, width, height);
	set_modal();
	hide();

	Ioctl_Bmp* bmp = new Ioctl_Bmp(IDR_IOCTL_CODE);
	Fl_Image *dergb;
	dergb = bmp->copy();
	dergb->inactive();
	Fl_Box* box = new Fl_Box(10, 10, 340, 50);
	box->image(bmp);
	box->deimage(dergb);

	int x = 10;
	int y = 60;
	int w = 142;
	int h = 160;

	itype = new Fl_Box(x, y, w, 20);
	set_label_font_12(itype);
	itype->label(type_hex_def[FILE_DEVICE_UNKNOWN-1]);
	btype = new Fl_Hold_Browser(x, y+20, w, h-20);
	btype->textsize(12);
	btype->textfont(FL_HELVETICA);
	for (int i = 0; i < sizeof(type_str_def) / sizeof(type_str_def[0]); i++)
		btype->add(type_str_def[i]);
	btype->callback(type_callback, this);
	btype->value(FILE_DEVICE_UNKNOWN);

	x += (w+5);
	w = 40;
	iaccess = new Fl_Box(x, y, w, 20);
	set_label_font_12(iaccess);
	iaccess->label(hex_str_0x[0]);
	baccess = new Fl_Hold_Browser(x, y+20, w, h-20);
	baccess->textsize(12);
	baccess->textfont(FL_HELVETICA);
	for (int i = 0; i < 4; i++)
		baccess->add(access_str_def[i]);
	baccess->value(1);
	baccess->callback(access_callback, this);
	
	x = 235;
	w = 7;
	Fl_Box* b = new Fl_Box(x - 2 * w, y, w * 2, 20);
	set_label_font_12(b);
	b->label("0x");
	iifunc[0] = new Fl_Box(x, y, w, 20);
	iifunc[1] = new Fl_Box(x+w, y, w, 20);
	iifunc[2] = new Fl_Box(x+2*w, y, w, 20);
	x = 210;
	w = 20;
	ifunc[0] = new Fl_Slider(x, y+20, w, h-20);
	ifunc[1] = new Fl_Slider(x+w, y+20, w, h-20);
	ifunc[2] = new Fl_Slider(x+2*w, y+20, w, h-20);
	ifunc[0]->callback(ifunc_callback0, this);
	ifunc[1]->callback(ifunc_callback1, this);
	ifunc[2]->callback(ifunc_callback2, this);
	for (int i = 0; i < 3; i++)
	{
		ifunc[i]->minimum(0);
		ifunc[i]->maximum(15);
		ifunc[i]->step(1);
		ifunc[i]->value(0xf);
		set_label_font_12(iifunc[i]);
		iifunc[i]->label("0");
	}
	ifunc[0]->value(7);
	iifunc[0]->label("8");
	
	x += 3 * w + 10;
	w = 66;
	imethod = new Fl_Box(x, y, w, 20);
	set_label_font_12(imethod);
	imethod->label(hex_str_0x[0]);
	bmethod = new Fl_Hold_Browser(x, y+20, w, h-20);
	bmethod->textsize(12);
	bmethod->textfont(FL_HELVETICA);
	for (int i = 0; i < 4; i++)
		bmethod->add(method_str_def[i]);
	bmethod->value(1);
	bmethod->callback(method_callback, this);

	x = 10;
	y = y + h + 20;
	w = (width - 10) / 3 - 10;
	h = 20;
	icode = new Fl_Input(x, y, w, h);
	icode->copy_label(label_conv("控制码(hex)"));
	icode->align(FL_ALIGN_TOP);
	set_label_font_12(icode);
	set_text_font_12(icode);
	
	x += w + 10;
	iinlen = new Fl_Input(x, y, w, h);
	iinlen->copy_label(label_conv("字节数(In)"));
	iinlen->align(FL_ALIGN_TOP);
	iinlen->type(FL_INT_INPUT);
	set_label_font_12(iinlen);
	set_text_font_12(iinlen);

	x += w + 10;
	ioulen = new Fl_Input(x, y, w, h);
	ioulen->copy_label(label_conv("字节数(Out)"));
	ioulen->type(FL_INT_INPUT);
	ioulen->align(FL_ALIGN_TOP);
	set_label_font_12(ioulen);
	set_text_font_12(ioulen);

	/*bok = new Fl_Button(width - 90, height - 40, 80, 20);
	bok->copy_label(label_conv("确定"));
	set_label_font_12(bok);
	bcancel = new Fl_Button(width - 90, height - 70, 80, 20);
	bcancel->copy_label(label_conv("取消"));
	set_label_font_12(bcancel);

	btncode = new Fl_Return_Button(width - 90, height - 100, 80, 20);
	btncode->copy_label(label_conv("刷新"));
	set_label_font_12(btncode);
	btncode->callback(code_callback, this);
*/
	copy_label(label_conv("更新行"));
	end();
}


Ioctl_Update::~Ioctl_Update()
{
	
}
