#ifndef _tinysvg_h_
#define _tinysvg_h_

#include "tinyxml.h"
#include <Windows.h>

#ifdef __cplusplus
extern "C" {
#endif

	typedef enum
	{
		type_unknown = 0,
		type_circle,
		type_line,
		type_ellipse,
		type_path,
		type_polygon,
		type_polyline,
		type_rect,
		type_image,
	}svg_type;

	typedef struct
	{
		svg_type type;
		void* object;
		list_entry_t list_entry;
	}svg_element;

	typedef struct
	{
		void* hwnd;
		int x;
		int y;
		int w;
		int h;
		void* gr_context;
		list_entry_t element_list;
	}svg_canvas;


	/* load or free */
	svg_canvas* svg_load_canvas(xml_element* element);

	void svg_free_canvas(svg_canvas* canvas);

	void svg_init_canvas(svg_canvas* canvas, HWND hwnd, int x, int y, int w, int h);

	LRESULT CALLBACK canvas_window_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);



#ifdef __cplusplus
}
#endif

#endif
