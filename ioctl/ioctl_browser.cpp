#include "ioctl_browser.h"
#include "ioctl.h"
#include "common.h"

Ioctl_Browser::Ioctl_Browser(int x, int y, int w, int h, const char *l )
	:Fl_Hold_Browser(x, y + 20, w, h-20, l)
{
	Fl_Box* box = new Fl_Box(FL_UP_BOX, x, y, w, 20, "");
	box->copy_label(label_conv("ÅäÖÃÎÄ¼þ"));
	set_label_font_12(box);
	textfont(FL_COURIER);
	textsize(12);
	callback(&Ioctl_Browser::call_back, 0);
}

void Ioctl_Browser::call_back(Fl_Widget* o, long i)
{
	Ioctl_Browser* b = (Ioctl_Browser*)o;
	if (b->value())
	{
		const char* str = b->text(b->value());
		for (std::map<char*, ioctl_table*>::iterator i = b->map_config.begin(); i != b->map_config.end(); i++)
		{
			if (!strcmp(i->first, str))
			{
				b->win->update_table(i->second);
				break;
			}
		}
	}
}

void Ioctl_Browser::add_(wchar_t* file, xml_document* doc)
{
	if (!file || !doc)
		return;

	if (!doc)
		return;

	char* buf = wcs_to_str(file, -1, encoding_utf8);
	ioctl_table* t = new ioctl_table(doc);
	for (std::map<char*, ioctl_table*>::iterator i = map_config.begin(); i != map_config.end(); i++)
	{
		if (!strcmp(i->first, buf))
		{
			delete i->second;
			free(i->first);
			map_config.erase(i);
			break;
		}
	}
	map_config.insert(std::pair<char*, ioctl_table*>(buf, t));

	win->update_table(t);
	update_(buf);
}

void Ioctl_Browser::update_(char* str)
{
	Fl::lock();
	clear();
	int index = 1;
	for (std::map<char*, ioctl_table*>::iterator i = map_config.begin(); i != map_config.end(); i++)
	{
		add(i->first);
		if (!strcmp(str, i->first))
			select(index);
		index++;
	}
	Fl::unlock();
}

void Ioctl_Browser::close_()
{
	Fl::lock();
	if (size() && value())
	{
		const char* buf = text(value());
		for (std::map<char*, ioctl_table*>::iterator i = map_config.begin(); i != map_config.end(); i++)
		{
			if (!strcmp(i->first, buf))
			{
				delete i->second;
				free(i->first);
				map_config.erase(i);
				break;
			}
		}
		remove(value());
		if (size())
		{
			select(1);
			win->update_table(map_config.begin()->second);
		}
		else
			win->update_table(0);
	}
	Fl::unlock();
}