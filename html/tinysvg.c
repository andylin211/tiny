#include "tinysvg.h"
#include <Windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "tinykv.h"
#include <assert.h>
#include "graphic.h"


void svg_reset_color(grph_color_t* color)
{
	if (!color)
		return;

	color->r = color->g = color->b = 0;
}

static char* svg_style_color_value(char* buffer, int* value)
{
	char* start = 0;
	int len = 0;

	if (!buffer || !value)
		return buffer;

	start = buffer;
	while (1)
	{
		assert(*buffer != 0);
		if (*buffer == ',' || *buffer == ')')
			break;
		len++;
		buffer++;
	}
	*buffer = 0;
	*value = atoi(start);

	buffer++;
	return buffer;
}

/* L"rgb(0,0,255)" */
static void svg_style_color(wchar_t* buffer, grph_color_t* color)
{
	char* start = 0;
	char* str = 0;
	if (!buffer || !color)
		return;

	start = str = wcs_to_str(buffer, -1, encoding_ansi);

	str += 4;

	str = svg_style_color_value(str, &color->r);
	str = svg_style_color_value(str, &color->g);
	str = svg_style_color_value(str, &color->b);

	free(start);
}


/* L"244" */
static void svg_style_int_value(wchar_t* buffer, int* value)
{
	if (!buffer || !value)
		return;

	*value = wcstol(buffer, 0, 10);
}

/* must no other char */
static kv_list_t* svg_load_style(char* buffer)
{
	kv_t* kv = 0;
	kv_list_t* list = 0;
	char* key = 0;
	int key_len = 0;
	char* value = 0;
	int value_len = 0;

	if (!buffer)
		return 0;

	list = (kv_list_t*)safe_malloc(sizeof(kv_list_t));
	initialize_list_head(&list->kv_list);

	while (1)
	{
		key_len = 0;
		value_len = 0;

		kv = (kv_t*)safe_malloc(sizeof(kv_t));

		key = buffer;
		while (1)
		{
			assert(*buffer != 0);
			if (*buffer == ':')
				break;

			key_len++;
			buffer++;
		}

		kv->key = (char*)safe_malloc(key_len + 1);
		memcpy(kv->key, key, key_len);

		/* skip : */
		buffer++;

		value = buffer;
		while (1)
		{
			if (*buffer == 0 || *buffer == ';')
				break;

			value_len++;
			buffer++;
		}

		kv->value = str_to_wcs(value, value_len, encoding_ansi);

		insert_into_list(&list->kv_list, &kv->list_entry);

		if (*buffer == 0)
			break;

		/* skip ; */
		buffer++;
	}

	return list;
}

int svg_query_int_attribute(xml_element* element, char* name)
{
	wchar_t* value = 0;

	value = xml_query_attribute(element, name);

	return value ? wcstol(value, 0, 10) : 0;
}

void svg_set_fill(kv_list_t* style, grph_fill_t* fill)
{
	list_entry_t* list_entry = 0;
	kv_t* kv = 0;

	if (!fill)
		return;

	fill->is_set = 0;
	fill->opacity = 255;
	svg_reset_color(&fill->color);

	if (!style)
		return;

	for (list_entry = style->kv_list.flink; list_entry != &style->kv_list; list_entry = list_entry->flink)
	{
		kv = container_of(list_entry, kv_t, list_entry);
		if (kv->key && 0 == strcmp(kv->key, "fill"))
		{
			if (kv->value)
			{
				if (0 == wcscmp(kv->value, L"none"))
					fill->is_set = 0;
				else
				{
					fill->is_set = 1;
					svg_style_color(kv->value, &fill->color);
				}
			}
			break;
		}
		else if (0 == strcmp(kv->key, "fill-opacity"))
			svg_style_int_value(kv->value, &fill->opacity);
	}
}

void svg_set_stroke(kv_list_t* style, grph_stroke_t* stroke)
{
	list_entry_t* list_entry = 0;
	kv_t* kv = 0;

	if (!stroke)
		return;

	stroke->width = 1;
	stroke->opacity = 255;
	svg_reset_color(&stroke->color);

	if (!style)
		return;

	for (list_entry = style->kv_list.flink; list_entry != &style->kv_list; list_entry = list_entry->flink)
	{
		kv = container_of(list_entry, kv_t, list_entry);
		if (kv->key)
		{
			if (0 == strcmp(kv->key, "stroke-width"))
				svg_style_int_value(kv->value, &stroke->width);
			else if (0 == strcmp(kv->key, "stroke"))
				svg_style_color(kv->value, &stroke->color);
			else if (0 == strcmp(kv->key, "stroke-opacity"))
				svg_style_int_value(kv->value, &stroke->opacity);
		}
	}
}

/************************************************************************************************************************/

/* line */
/* load or free */

static grph_line_t* svg_load_line(xml_element* element)
{
	char* style_str = 0;
	kv_list_t *style = 0;
	grph_line_t* line = 0;

	if (!element || strcmp(element->name, "line"))
		return 0;

	line = (grph_line_t*)safe_malloc(sizeof(grph_line_t));
	line->x1 = svg_query_int_attribute(element, "x1");
	line->y1 = svg_query_int_attribute(element, "y1");
	line->x2 = svg_query_int_attribute(element, "x2");
	line->y2 = svg_query_int_attribute(element, "y2");
	style_str = wcs_to_str(xml_query_attribute(element, "style"), -1, encoding_ansi);
	style = svg_load_style(style_str);
	svg_set_fill(style, &line->style.fill);
	svg_set_stroke(style, &line->style.stroke);

	free(style);
	free(style_str);
	return line;
}

/* rect */
/* load or free */

static grph_rect_t* svg_load_rect(xml_element* element)
{
	char* style_str = 0;
	kv_list_t *style = 0;
	grph_rect_t* rect = 0;

	if (!element || strcmp(element->name, "rect"))
		return 0;

	rect = (grph_rect_t*)safe_malloc(sizeof(grph_rect_t));
	rect->x = svg_query_int_attribute(element, "x");
	rect->y = svg_query_int_attribute(element, "y");
	rect->w = svg_query_int_attribute(element, "width");
	rect->h = svg_query_int_attribute(element, "height");
	style_str = wcs_to_str(xml_query_attribute(element, "style"), -1, encoding_ansi);
	style = svg_load_style(style_str);
	svg_set_fill(style, &rect->style.fill);
	svg_set_stroke(style, &rect->style.stroke);

	free(style);
	free(style_str);
	return rect;
}


/* circle */
/* load or free */

static grph_circle_t* svg_load_circle(xml_element* element)
{
	char* style_str = 0;
	kv_list_t *style = 0;
	grph_circle_t* circle = 0;

	if (!element || strcmp(element->name, "circle"))
		return 0;

	circle = (grph_circle_t*)safe_malloc(sizeof(grph_circle_t));
	circle->x = svg_query_int_attribute(element, "cx");
	circle->y = svg_query_int_attribute(element, "cy");
	circle->r = svg_query_int_attribute(element, "r");
	style_str = wcs_to_str(xml_query_attribute(element, "style"), -1, encoding_ansi);
	style = svg_load_style(style_str);
	svg_set_fill(style, &circle->style.fill);
	svg_set_stroke(style, &circle->style.stroke);

	free(style);
	free(style_str);
	return circle;
}


/* element */
/* add or free */

static void svg_add_element(svg_canvas* canvas, void* object, svg_type type)
{
	svg_element* ele = 0;

	if (!canvas || !object)
		return;

	ele = (svg_element*)safe_malloc(sizeof(svg_element));
	ele->type = type;
	ele->object = object;
	insert_into_list(&canvas->element_list, &ele->list_entry);
}

static void svg_free_element(svg_element* element)
{
	if (!element || !element->object)
		return;

	switch (element->type)
	{
	case type_rect:
		free((grph_rect_t*)element->object);
		break;
	case type_line:
		free((grph_line_t*)element->object);
		break;
	case type_circle:
		free((grph_circle_t*)element->object);
		break;
	default:
		break;
	}
}

static void svg_draw_element(svg_element* element, void* gr_context)
{
	if (!element)
		return;

	switch (element->type)
	{
	case type_rect:
		grph_draw_rect(gr_context, (grph_rect_t*)element->object);
		break;
	case type_line:
		grph_draw_line(gr_context, (grph_line_t*)element->object);
		break;
	case type_circle:
		grph_draw_circle(gr_context, (grph_circle_t*)element->object);
		break;
	default:
		break;
	}
}


static void svg_init_element(svg_element* element, void* gr_context)
{
	if (!element || !element->object || !gr_context)
		return;

	switch (element->type)
	{
	case type_rect:
		grph_init_rect(gr_context, (grph_rect_t*)element->object);
		break;
	case type_line:
		grph_init_line(gr_context, (grph_line_t*)element->object);
		break;
	case type_circle:
		grph_init_circle(gr_context, (grph_circle_t*)element->object);
		break;
	default:
		break;
	}
}

static void svg_uninit_element(svg_element* element)
{
	if (!element)
		return;

	switch (element->type)
	{
	case type_rect:
		grph_uninit_rect((grph_rect_t*)element->object);
		break;
	case type_line:
		grph_uninit_line((grph_line_t*)element->object);
		break;
	case type_circle:
		grph_uninit_circle((grph_circle_t*)element->object);
		break;
	default:
		break;
	}
}

/* canvas */
/* load or free */
svg_canvas* svg_load_canvas(xml_element* element)
{
	list_entry_t* list_entry = 0;
	xml_element* ele = 0;
	svg_canvas* canvas = 0;
	grph_rect_t* rect = 0;
	grph_line_t* line = 0;
	grph_circle_t* circle = 0;

	if (!element)
		return 0;

	canvas = (svg_canvas*)safe_malloc(sizeof(svg_canvas));
	canvas->w = canvas->h = 0;

	initialize_list_head(&canvas->element_list);

	for (list_entry = element->element_list.flink; list_entry != &element->element_list; list_entry = list_entry->flink)
	{
		ele = container_of(list_entry, xml_element, list_entry);
		if (0 == strcmp(ele->name, "rect"))
		{
			rect = svg_load_rect(ele);
			svg_add_element(canvas, rect, type_rect);
		}
		else if (0 == strcmp(ele->name, "line"))
		{
			line = svg_load_line(ele);
			svg_add_element(canvas, line, type_line);
		}
		else if (0 == strcmp(ele->name, "circle"))
		{
			circle = svg_load_circle(ele);
			svg_add_element(canvas, circle, type_circle);
		}
	}

	return canvas;
}

void svg_free_canvas(svg_canvas* canvas)
{
	svg_element* ele = 0;

	if (!canvas)
		return;

	while (!is_list_empty(&canvas->element_list))
	{
		ele = container_of(canvas->element_list.flink, svg_element, list_entry);

		svg_free_element(ele);

		remove_from_list(canvas->element_list.flink);

		free(ele);
	}
}

void svg_paint_canvas(svg_canvas* canvas);

void svg_destroy_canvas(svg_canvas* canvas);

LRESULT CALLBACK canvas_window_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	svg_canvas* canvas = (svg_canvas*)(LONG_PTR)GetWindowLongPtr(hwnd, GWLP_USERDATA);	

	switch (message)
	{
	case WM_PAINT:
		svg_paint_canvas(canvas);		
		ValidateRect(hwnd, NULL);
		break;

	case WM_DESTROY:
		svg_destroy_canvas(canvas);
		PostQuitMessage(0);
		break;
		
	default:
		break;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

void svg_init_canvas(svg_canvas* canvas, HWND hwnd, int x, int y, int w, int h)
{
	list_entry_t* list_entry = 0;
	svg_element* ele = 0;

	if (!canvas || !hwnd)
		return;

	canvas->x = x;
	canvas->y = y;
	canvas->w = w;
	canvas->h = h;
	canvas->hwnd = hwnd;
	canvas->gr_context = grph_create_context(canvas->hwnd);

	SetWindowLongPtr(canvas->hwnd, GWLP_USERDATA, (LONG)(LONG_PTR)canvas);

	for (list_entry = canvas->element_list.flink; list_entry != &canvas->element_list; list_entry = list_entry->flink)
	{
		ele = container_of(list_entry, svg_element, list_entry);
		svg_init_element(ele, canvas->gr_context);
	}
}

void svg_destroy_canvas(svg_canvas* canvas)
{
	list_entry_t* list_entry = 0;
	svg_element* ele = 0;

	if (!canvas)
		return;

	for (list_entry = canvas->element_list.flink; list_entry != &canvas->element_list; list_entry = list_entry->flink)
	{
		ele = container_of(list_entry, svg_element, list_entry);
		svg_uninit_element(ele);
	}

	grph_destroy_context(canvas->gr_context);
}


/* paint */
void svg_paint_canvas(svg_canvas* canvas)
{
	list_entry_t* list_entry = 0;
	svg_element* ele = 0;
	FILETIME ft1, ft2;

	if (!canvas)
		return;

	GetSystemTimeAsFileTime(&ft1);

	grph_begin_draw(canvas->gr_context);

	for (list_entry = canvas->element_list.flink; list_entry != &canvas->element_list; list_entry = list_entry->flink)
	{
		ele = container_of(list_entry, svg_element, list_entry);
		svg_draw_element(ele, canvas->gr_context);
	}

	grph_end_draw(canvas->gr_context);

	GetSystemTimeAsFileTime(&ft2);

	printf("%ld - %ld = %ld\n", ft2.dwLowDateTime, ft1.dwLowDateTime, ft2.dwLowDateTime - ft1.dwLowDateTime);
}

#ifdef _tinysvg_test_

int main()
{
	char* buffer = 0;
	int len = 0;
	FILE* file = 0;
	xml_document* doc = 0;
	svg_canvas* canvas = 0;

	file = fopen("C:\\Users\\andycylin\\tiny\\tiny\\Debug\\tinysvg.xml", "rb");
	if (!file)
		return;

	fseek(file, 0, SEEK_END);
	len = ftell(file);
	fseek(file, 0, SEEK_SET);

	buffer = (char*)safe_malloc(len + 1);
	fread(buffer, len, 1, file);
	fclose(file);

	xml_load_document(buffer, encoding_utf8, &doc);
	xml_write_document(stdout, encoding_ansi, doc);

	if (doc)
		canvas = svg_load_canvas(doc->root_element);

	free(buffer);
	xml_free_document(doc);
	svg_free_canvas(canvas);

	ui();

	return 0;
}

#endif
