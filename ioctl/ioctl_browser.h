#pragma once

#include <FL/Fl.H>
#include <FL/Fl_Hold_Browser.H>
#include <FL/Fl_Box.H>
#include <map>
#include <vector>
#include "tinyxml.h"
#include "common.h"

class Ioctl_Window;

class  ioctl_line 
{
public:
	char* code;
	char* type;
	char* access;
	char* func;
	char* method;
	char* inlen;
	char* oulen;
	ioctl_line(wchar_t* c, wchar_t* t, wchar_t* f, wchar_t* m, wchar_t* a, wchar_t* in, wchar_t* ou)
	{
		code = wcs_to_str(c, -1, encoding_utf8);
		type = wcs_to_str(t, -1, encoding_utf8);
		func = wcs_to_str(f, -1, encoding_utf8);
		method = wcs_to_str(m, -1, encoding_utf8);
		access = wcs_to_str(a, -1, encoding_utf8);
		inlen = wcs_to_str(in, -1, encoding_utf8);
		oulen = wcs_to_str(ou, -1, encoding_utf8);
	}
	~ioctl_line() 
	{
		free(code);
		free(type);
		free(func);
		free(method);
		free(access);
		free(inlen);
		free(oulen);
	}
};

class ioctl_table 
{
public:
	char* device;
	std::vector<ioctl_line*> v_line;
	ioctl_table(xml_document* doc)
	{
		device = 0;
		v_line.clear();

		if (!doc->root_element)
			return;

		if (strcmp(doc->root_element->name, "device"))
			return;

		device = wcs_to_str(xml_query_attribute(doc->root_element, "name"), -1, encoding_utf8);
		if (!is_list_empty(&doc->root_element->element_list))
		{
			for (list_entry_t* i = doc->root_element->element_list.flink; i != &doc->root_element->element_list; i = i->flink)
			{
				xml_element* ele = container_of(i, xml_element, list_entry);
				wchar_t* code = xml_query_attribute(ele, "code");
				wchar_t* type = xml_query_attribute(ele, "type");
				wchar_t* function = xml_query_attribute(ele, "function");
				wchar_t* method = xml_query_attribute(ele, "method");
				wchar_t* access = xml_query_attribute(ele, "access");
				wchar_t* inlen = xml_query_attribute(ele, "inlen");
				wchar_t* oulen = xml_query_attribute(ele, "oulen");

				ioctl_line* line = new ioctl_line(code, type_str(type), function, method_str(method), access_str(access), inlen, oulen);
				v_line.push_back(line);
			}
		}
	}
	~ioctl_table()
	{
		free(device);
		for (std::vector<ioctl_line*>::iterator i = v_line.begin(); i != v_line.end(); i++)
		{
			ioctl_line* p = *i;
			if (p)
				delete p;
		}
		v_line.clear();
	}
};

class Ioctl_Browser : public Fl_Hold_Browser
{
public:

	Ioctl_Browser(int x, int y, int w, int h, const char *l = 0);

	void set_top_window(Ioctl_Window* w) { win = w; }

	static void call_back(Fl_Widget* o, long);

	/* thread safe */
	void add_(wchar_t* file, xml_document* doc);

	/* thread safe */
	void close_();

private:
	/* thread safe */
	void update_(char* str);

	std::map<char*, ioctl_table*> map_config;

	Ioctl_Window* win;
};