#include "FL\Fl.H"
#include "FL\Fl_Window.H"
#include "FL\Fl_Button.H"
#include "Fl\Fl_Choice.H"
#include "FL/fl_utf8.h"
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Text_Editor.H>
#include <stdio.h>

void choice_cb(Fl_Choice* o, void*)
{
	printf("%d\n", o->value());
	printf("%s\n", o->text(o->value()));
}

void btn_do_cb(Fl_Widget* o, void*)
{
	printf("Td");
}

int main(int argc, char* argv[])
{
	char* p = 0;

	char buf_title[256];
	p = "病毒一键构造工具（仅用于测试）";
	fl_utf8from_mb(buf_title, 256, p, strlen(p));
	Fl_Window window(420, 400, buf_title);

	window.label();

	char buf[256];
	p = "病毒样本(系统):";
	fl_utf8from_mb(buf, 256, p, strlen(p));
	Fl_Choice choice(120, 20, 200, 25, buf);
	choice.add("netgjo.sys (xp;win732)");
	choice.add("netgjo.sys (x1)");
	choice.add("netgjo.sys (x2)");
	choice.add("netgjo.sys (x3)");
	choice.add("netgjo.sys (x7)");
	choice.callback((Fl_Callback *)choice_cb);

	char buf2[256];
	p = "构造";
	fl_utf8from_mb(buf2, 256, p, strlen(p));
	Fl_Button btn_do(330, 20, 80, 25, buf2);
	btn_do.callback(btn_do_cb);

	Fl_Text_Buffer textbuf;

	Fl_Text_Editor editor(10, 50, 400, 340);
	editor.textfont(FL_COURIER);
	editor.textsize(14);
	editor.buffer(textbuf);

	window.end();
	window.show(1, argv);
	return Fl::run();
}