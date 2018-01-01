#include "graphic.h"
#include <Windows.h>
#include <d2d1.h>
#include "tinystr.h"

template<class Interface>
inline void safe_release(Interface **pp)
{
	if (!*pp)
		return;

	(*pp)->Release();
	(*pp) = 0;
}

typedef struct
{
	HWND hwnd;
	ID2D1Factory* factory;
	ID2D1HwndRenderTarget* target;
	float dpi_x;
	float dpi_y;
	D2D1_SIZE_U size;
}grph_context_t;

void* grph_create_context(void* hwnd)
{
	HRESULT hr = S_OK;
	RECT rc;
	
	grph_context_t* context = 0;
	
	if (!hwnd)
		return 0;

	context = (grph_context_t*)safe_malloc(sizeof(grph_context_t));
	context->hwnd = (HWND)hwnd;

	do
	{
		hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &context->factory);
		if (FAILED(hr))
			break;

		context->factory->GetDesktopDpi(&context->dpi_x, &context->dpi_y);		

		GetClientRect(context->hwnd, &rc);

		context->size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

		hr = context->factory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(context->hwnd, context->size),
			&context->target
		);

		if (FAILED(hr))
			break;

		hr = S_OK;
	} while (0);
	
	if (FAILED(hr))
	{
		grph_destroy_context(context);
		context = 0;
	}

	return context;
}

void grph_destroy_context(void* gr_context)
{
	grph_context_t* p = (grph_context_t*)gr_context;
	if (!p)
		return;

	safe_release(&p->factory);
	safe_release(&p->target);
	free(p);
}

void grph_begin_draw(void* gr_context)
{
	grph_context_t* context = (grph_context_t*)gr_context;

	if (!context)
		return;

	context->target->BeginDraw();

	context->target->SetTransform(D2D1::Matrix3x2F::Identity());

	context->target->Clear(D2D1::ColorF(D2D1::ColorF::White));
}

void grph_end_draw(void* gr_context)
{
	grph_context_t* context = (grph_context_t*)gr_context;

	if (!context)
		return;

	/* ignore hr */
	context->target->EndDraw();
}

template <class Element>
void grph_uninit_element(Element* element);

template <class Element>
void grph_init_element(void* gr_context, Element* element)
{
	grph_context_t* context = (grph_context_t*)gr_context;
	ID2D1SolidColorBrush* brush = 0;
	HRESULT hr = S_OK;
	D2D1_COLOR_F color;

	if (!gr_context || !element)
		return;

	do
	{
		color.r = float(element->style.stroke.color.r / 255.0);
		color.g = float(element->style.stroke.color.g / 255.0);
		color.b = float(element->style.stroke.color.b / 255.0);
		color.a = float(element->style.stroke.opacity / 255.0);
		hr = context->target->CreateSolidColorBrush(color, &brush);

		if (FAILED(hr))
			break;

		element->style.stroke.brush = (void*)brush;

		color.r = float(element->style.fill.color.r / 255.0);
		color.g = float(element->style.fill.color.g / 255.0);
		color.b = float(element->style.fill.color.b / 255.0);
		color.a = float(element->style.fill.opacity / 255.0);
		hr = context->target->CreateSolidColorBrush(color, &brush);

		if (FAILED(hr))
			break;

		element->style.fill.brush = (void*)brush;

		hr = S_OK;

	} while (0);

	if (FAILED(hr))
		grph_uninit_element(element);
}

template <class Element>
void grph_uninit_element(Element* element)
{
	ID2D1SolidColorBrush* brush = 0;

	if (!element)
		return;

	if (element->style.stroke.brush)
	{
		brush = (ID2D1SolidColorBrush*)element->style.stroke.brush;
		safe_release(&brush);
		element->style.stroke.brush = 0;
	}
	
	if (element->style.fill.brush)
	{
		brush = (ID2D1SolidColorBrush*)element->style.fill.brush;
		safe_release(&brush);
		element->style.fill.brush = 0;
	}
}

/**********************************************************************************************/

/* line */
void grph_init_line(void* gr_context, grph_line_t* line)
{
	grph_init_element<grph_line_t>(gr_context, line);
}

void grph_uninit_line(grph_line_t* line)
{
	grph_uninit_element<grph_line_t>(line);
}

void grph_draw_line(void* gr_context, grph_line_t* line)
{
	grph_context_t* context = (grph_context_t*)gr_context;
	ID2D1SolidColorBrush* brush = 0;
	HRESULT hr = S_OK;

	if (!gr_context || !line)
		return;

	brush = (ID2D1SolidColorBrush*)line->style.stroke.brush;

	context->target->DrawLine(
		D2D1::Point2F((float)line->x1, (float)line->y1),
		D2D1::Point2F((float)line->x2, (float)line->y2),
		brush,
		(float)line->style.stroke.width
	);
}

/* rect */
void grph_init_rect(void* gr_context, grph_rect_t* rect)
{
	grph_init_element<grph_rect_t>(gr_context, rect);
}

void grph_uninit_rect(grph_rect_t* rect)
{
	grph_uninit_element<grph_rect_t>(rect);
}

void grph_draw_rect(void* gr_context, grph_rect_t* rect)
{
	grph_context_t* context = (grph_context_t*)gr_context;
	ID2D1SolidColorBrush* brush = 0;
	HRESULT hr = S_OK;
	D2D1_RECT_F rectangle;

	if (!gr_context || !rect)
		return;

	rectangle.left = (float)rect->x;
	rectangle.top = (float)rect->y;
	rectangle.right = (float)(rect->x + rect->w - 1);
	rectangle.bottom = (float)(rect->y + rect->h - 1);

	if (rect->style.fill.is_set)
	{
		brush = (ID2D1SolidColorBrush*)rect->style.fill.brush;

		context->target->FillRectangle(rectangle, brush);
	}

	brush = (ID2D1SolidColorBrush*)rect->style.stroke.brush;

	context->target->DrawRectangle(rectangle, brush, (float)rect->style.stroke.width);
}


/* circle */
void grph_init_circle(void* gr_context, grph_circle_t* circle)
{
	grph_init_element<grph_circle_t>(gr_context, circle);
}

void grph_uninit_circle(grph_circle_t* circle)
{
	grph_uninit_element<grph_circle_t>(circle);
}

void grph_draw_circle(void* gr_context, grph_circle_t* circle)
{
	grph_context_t* context = (grph_context_t*)gr_context;
	ID2D1SolidColorBrush* brush = 0;
	HRESULT hr = S_OK;

	if (!gr_context || !circle)
		return;

	D2D1_ELLIPSE ellipse;

	ellipse.point = D2D1::Point2F((float)circle->x, (float)circle->y);
	ellipse.radiusX = (float)circle->r;
	ellipse.radiusY = (float)circle->r;

	if (circle->style.fill.is_set)
	{
		brush = (ID2D1SolidColorBrush*)circle->style.fill.brush;
		context->target->FillEllipse(ellipse, brush);
	}

	brush = (ID2D1SolidColorBrush*)circle->style.stroke.brush;

	context->target->DrawEllipse(ellipse, brush, (float)circle->style.stroke.width);
}
