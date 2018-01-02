#pragma once
#include "tinyfltk.h"
#include "resource.h"
#include <assert.h>

class ctlcode_t
{
public:
	Fl_Input* icode;
	Fl_Button* bupdate;
	Fl_Button* bok;
	char code_buf[fltk_t::buf_len];

	Fl_Box* itype;
	Fl_Hold_Browser* btype;
	int type_last_value;
	
	Fl_Box* iaccess;
	Fl_Hold_Browser* baccess;
	int access_last_value;
	
	Fl_Box* ifunc[3];
	Fl_Slider* sfunc[3];
	int func_value;
	
	Fl_Box* imethod;
	Fl_Hold_Browser* bmethod;
	int method_last_value;

	int w() { return _w; }
	int h() { return _h; }

	int _x, _y, _w, _h;
	
public:
	ctlcode_t(int x = 0, int y = 0, int w = 350, int h = 250)
		:_x(x), _y(y), _w(w), _h(h)
	{
		fltk_t::create_outbox(x + 5, y + 5, w - 10, 40);
		y += 15;
		h -= 15;
		icode = fltk_t::create_input(x + 80, y, w - 120 - (x + 80) - 30, 20, "控制码(hex):");
		icode->value("00000000");
		icode->maximum_size(8);
		bupdate = fltk_t::create_button(w - 140, y, 60, 20, "刷新", true);
		bupdate->callback(update_callback, this);
		bok = fltk_t::create_button(w - 70, y, 60, 20, "确定");
		bok->callback(ok_callback, this);

		y += 35;
		h -= 35;

		fltk_t::create_outbox(x+5, y+5, w-10, h-10);
		fltk_t::create_imgbox(x+7, y+15, w-10, 50, IDR_CTL_CODE);
		
		x = x + 10;
		w = 142;
		y += 65;
		h -= 65;

		itype = fltk_t::create_labelbox(x, y, w, 20, device_type_unknown_hex());
		btype = fltk_t::create_browser(x, y + 20, w, h - 30);
		for (int i = 0; i < max_device_type; i++)
			btype->add(device_type_str(i));
		btype->callback(type_callback, this);
		btype->value(FILE_DEVICE_UNKNOWN);

		x += (w + 5);
		w = 40;
		iaccess = fltk_t::create_labelbox(x, y, w, 20, hex_str_0x(0));
		baccess = fltk_t::create_browser(x, y + 20, w, h - 30);
		for (int i = 0; i < 4; i++)
			baccess->add(access_str(i));
		baccess->value(1);
		baccess->callback(access_callback, this);

		x += 70;
		w = 7;
		fltk_t::create_labelbox(x - 2 * w, y, w * 2, 20, "0x");
		ifunc[0] = fltk_t::create_labelbox(x, y, w, 20, "0");
		ifunc[1] = fltk_t::create_labelbox(x + w, y, w, 20, "0");
		ifunc[2] = fltk_t::create_labelbox(x + 2 * w, y, w, 20, "0");
		x -= 25;
		w = 20;
		sfunc[0] = new Fl_Slider(x, y + 20, w, h - 30);
		sfunc[1] = new Fl_Slider(x + w, y + 20, w, h - 30);
		sfunc[2] = new Fl_Slider(x + 2 * w, y + 20, w, h - 30);
		sfunc[0]->callback(func_callback0, this);
		sfunc[1]->callback(func_callback1, this);
		sfunc[2]->callback(func_callback2, this);
		for (int i = 0; i < 3; i++)
		{
			sfunc[i]->minimum(0);
			sfunc[i]->maximum(15);
			sfunc[i]->step(1);
			sfunc[i]->value(0xf);
		}
		sfunc[0]->value(7);
		ifunc[0]->label("8");

		x += 3 * w + 5;
		w = 66;
		imethod = fltk_t::create_labelbox(x, y, w, 20, hex_str_0x(0));
		bmethod = fltk_t::create_browser(x, y + 20, w, h - 30);
		for (int i = 0; i < 4; i++)
			bmethod->add(method_str(i));
		bmethod->value(1);
		bmethod->callback(method_callback, this);

		btype->do_callback();
		baccess->do_callback();
		sfunc[0]->do_callback();
		sfunc[1]->do_callback();
		sfunc[2]->do_callback();
		bmethod->do_callback();
	}

	static void update_callback(Fl_Widget* o, void* v)
	{
		ctlcode_t* obj = (ctlcode_t*)v;

		obj->update_component();
	}

	static void ok_callback(Fl_Widget* o, void* v)
	{

	}

	static void type_callback(Fl_Widget* o, void* v)
	{
		ctlcode_t* obj = (ctlcode_t*)v;
		if (!obj->btype->value())
			obj->btype->value(obj->type_last_value);
		obj->itype->label(device_type_hex(obj->btype->value() - 1));
		obj->update_code();
		obj->type_last_value = obj->btype->value();
	}

	static void access_callback(Fl_Widget* o, void* v)
	{
		ctlcode_t* obj = (ctlcode_t*)v;
		if (!obj->baccess->value())
			obj->baccess->value(obj->access_last_value);
		obj->iaccess->label(hex_str_0x(obj->baccess->value() - 1));
		obj->update_code();
		obj->access_last_value = obj->baccess->value();
	}

	static void method_callback(Fl_Widget* o, void* v)
	{
		ctlcode_t* obj = (ctlcode_t*)v;
		if (!obj->bmethod->value())
			obj->bmethod->value(obj->method_last_value);
		obj->imethod->label(hex_str_0x(obj->bmethod->value() - 1));
		obj->update_code();
		obj->method_last_value = obj->bmethod->value();
	}

	void func_callback(int i)
	{
		ifunc[i]->label(hex_digit_str((int)(15 - sfunc[i]->value())));
		update_func_value();
		update_code();
	}

	static void func_callback0(Fl_Widget* o, void* v) 
	{
		ctlcode_t* up = (ctlcode_t*)v;
		up->func_callback(0);
	}

	static void func_callback1(Fl_Widget* o, void* v)
	{
		ctlcode_t* up = (ctlcode_t*)v;
		up->func_callback(1);
	}

	static void func_callback2(Fl_Widget* o, void* v)
	{
		ctlcode_t* up = (ctlcode_t*)v;
		up->func_callback(2);
	}

	void update_func_value()
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

	void update_code()
	{
		unsigned long code = make_ioctl_code(btype->value(), func_value, bmethod->value() - 1, baccess->value() - 1);
		sprintf_s(code_buf, fltk_t::buf_len, "%08X", code);
		icode->value(code_buf);
	}

	void update_component()
	{
		memcpy(code_buf, icode->value(), strlen(icode->value()) + 1);
		unsigned long code = strtol(code_buf, 0, 16);
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
			sfunc[i]->do_callback();
	}
	
	static char* hex_digit_str(int i)
	{
		static char* _hex_digit_str[] = { "0", "1", "2","3","4","5","6","7","8","9","A","B","C","D","E","F", };
		assert(i >= 0 && i <= 15);
		return _hex_digit_str[i];
	}

	static const int max_device_type = 45;

	static char* device_type_str(int i)
	{
		static char* _device_type_str[] = {
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
		assert(i >= 0 && i < 45);
		return _device_type_str[i];
	}

	static char* device_type_unknown_str()
	{
		return "unknown";
	}

	static char* device_type_unknown_hex()
	{
		return "0x0022";
	}

	static char* device_type_hex(int i)
	{
		static char* _device_type_hex[] = {
			"0x0001",
			"0x0002",
			"0x0003",
			"0x0004",
			"0x0005",
			"0x0006",
			"0x0007",
			"0x0008",
			"0x0009",
			"0x000A",
			"0x000B",
			"0x000C",
			"0x000D",
			"0x000E",
			"0x000F",
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
			"0x001A",
			"0x001B",
			"0x001C",
			"0x001D",
			"0x001E",
			"0x001F",
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
			"0x002A",
			"0x002B",
			"0x002C",
			"0x002D",
			"0x002E",
			"0x002F",
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
		assert(i >= 0 && i < 45);
		return _device_type_hex[i];
	}

	#define _gbk_to_utf8(buf, src)	fl_utf8from_mb(buf, buf_len, src, strlen(src))

	static char* access_str(int i)
	{
		const int buf_len = 16;
		static char _access_read[buf_len];
		static char _access_write[buf_len];
		static char _access_any[buf_len];
		static char _access_both[buf_len];
		static char* _access_str_def[] = {
			_access_any,
			_access_read,
			_access_write,
			_access_both
		};
		static int first = 1;
		if (first)
		{
			first = 0;
			_gbk_to_utf8(_access_any, "任意");
			_gbk_to_utf8(_access_read, "只读");
			_gbk_to_utf8(_access_write, "只写");
			_gbk_to_utf8(_access_both, "读写");
		}
		assert(i >= 0 && i <= 3);
		return _access_str_def[i];
	}

	static char* method_str(int i)
	{
		static char* _method_str_def[] = {
			"buffered",
			"in direct",
			"out direct",
			"neither",
		};
		assert(i >= 0 && i <= 3);
		return _method_str_def[i];
	}

	static char* hex_str_0x(int i)
	{
		static char* _hex_str_0x[] = {
			"0x0",
			"0x1",
			"0x2",
			"0x3",
		};
		assert(i >= 0 && i <= 3);
		return _hex_str_0x[i];
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

	static unsigned long make_ioctl_code(unsigned long device_type, unsigned long function, unsigned long method, unsigned long access)
	{
		return (((device_type) << 16) | ((access) << 14) | ((function) << 2) | (method));
	}

};



