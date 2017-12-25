#ifndef _graphic_h_
#define _graphic_h_

#ifdef __cplusplus
extern "C" {
#endif

	/* 0~255 */
	typedef struct
	{
		int r;
		int g;
		int b;
	}grph_color_t;

	typedef struct
	{
		int is_set;
		grph_color_t color;
		int opacity;
		void* brush;
	}grph_fill_t;

	typedef struct
	{
		int width;
		grph_color_t color;
		int opacity;
		void* brush;
	}grph_stroke_t;

	typedef struct
	{
		grph_fill_t fill;
		grph_stroke_t stroke;
	}grph_style_t;

	/* element */
	typedef struct
	{
		int x;
		int y;
		int w;
		int h;
		grph_style_t style;	
	}grph_rect_t;

	typedef struct
	{
		int x;
		int y;
		int r;
		grph_style_t style;
	}grph_circle_t;

	typedef struct
	{
		int x1;
		int y1;
		int x2;
		int y2;
		grph_style_t style;
	}grph_line_t;

	/* graphic */
	void* grph_create_context(void* hwnd);

	void grph_destroy_context(void* gr_context);

	void grph_begin_draw(void* gr_context);

	void grph_end_draw(void* gr_context);

	/* line */
	void grph_init_line(void* gr_context, grph_line_t* line);

	void grph_uninit_line(grph_line_t* line);

	void grph_draw_line(void* gr_context, grph_line_t* line);

	/* rect */
	void grph_init_rect(void* gr_context, grph_rect_t* rect);

	void grph_uninit_rect(grph_rect_t* rect);

	void grph_draw_rect(void* gr_context, grph_rect_t* rect);


	/* circle */
	void grph_init_circle(void* gr_context, grph_circle_t* circle);

	void grph_uninit_circle(grph_circle_t* circle);

	void grph_draw_circle(void* gr_context, grph_circle_t* circle);

#ifdef __cplusplus
}
#endif

#endif
