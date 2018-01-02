#include "image.h"
#include <Windows.h>
#include <d2d1.h>
#include <wincodec.h>  
#include "tinystr.h"

template <class Interface>
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
	IWICImagingFactory* image_factory;
	float dpi_x;
	float dpi_y;
	D2D1_SIZE_U size;
}img_context_t;


void* img_create_context(void* hwnd)
{
	HRESULT hr = S_OK;
	RECT rc;

	img_context_t* context = 0;

	if (!hwnd)
		return 0;

	context = (img_context_t*)safe_malloc(sizeof(img_context_t));
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

		CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, __uuidof(IWICImagingFactory), (LPVOID*)&context->image_factory);

		hr = S_OK;
	} while (0);

	if (FAILED(hr))
	{
		img_destroy_context(context);
		context = 0;
	}

	return context;
}

void img_destroy_context(void* im_context)
{
	img_context_t* p = (img_context_t*)im_context;
	if (!p)
		return;

	safe_release(&p->factory);
	safe_release(&p->target);
	free(p);
}

void img_begin_draw(void* im_context)
{
	img_context_t* context = (img_context_t*)im_context;

	if (!context)
		return;

	context->target->BeginDraw();

	context->target->SetTransform(D2D1::Matrix3x2F::Identity());

	context->target->Clear(D2D1::ColorF(D2D1::ColorF::White));
}

void img_end_draw(void* im_context)
{
	img_context_t* context = (img_context_t*)im_context;

	if (!context)
		return;

	/* ignore hr */
	context->target->EndDraw();
}

/* bitmap */
img_bitmap_t* img_create_bitmap(void* im_context, wchar_t* file)
{
	img_bitmap_t* p = 0;
	IWICBitmapDecoder *bitmapdecoder = NULL;
	IWICBitmapFrameDecode  *pframe = NULL;
	IWICFormatConverter * fmtcovter = NULL;
	ID2D1Bitmap * bitmap = NULL;
	HRESULT hr = S_OK;
	img_context_t* context = (img_context_t*)im_context;

	if (!context)
		return 0;

	do
	{
		hr = context->image_factory->CreateDecoderFromFilename(file, NULL, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &bitmapdecoder);
		if (FAILED(hr))
			break;
		
		hr = bitmapdecoder->GetFrame(0, &pframe);
		if (FAILED(hr))
			break;

		hr = context->image_factory->CreateFormatConverter(&fmtcovter);
		if (FAILED(hr))
			break;

		hr = fmtcovter->Initialize(pframe, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeCustom);
		if (FAILED(hr))
			break;

		hr = context->target->CreateBitmapFromWicBitmap(fmtcovter, NULL, &bitmap);
		if (FAILED(hr))
			break;

		hr = S_OK;
	} while (0);
	
	safe_release(&bitmapdecoder);
	safe_release(&fmtcovter);
	safe_release(&pframe);

	if (FAILED(hr))
	{
		safe_release(&bitmap);
		return 0;
	}

	p = (img_bitmap_t*)safe_malloc(sizeof(img_bitmap_t));
	p->bitmap = (void*)bitmap;
	p->w = bitmap->GetPixelSize().width;
	p->h = bitmap->GetPixelSize().height;

	return p;
}

void img_destroy_bitmap(img_bitmap_t* img)
{
	ID2D1Bitmap * bitmap = NULL;
	if (!img)
		return;

	bitmap = (ID2D1Bitmap*)img->bitmap;
	safe_release(&bitmap);
	if (img->file)
	{
		free(img->file);
		img->file = 0;
	}
		
	free(img);
}

void img_draw_bitmap(void* im_context, img_bitmap_t* img)
{
	img_context_t* context = (img_context_t*)im_context;

	if (!context || !img)
		return;

	D2D1_RECT_F rect = { 0, 0, float(img->w), float(img->h) };

	context->target->DrawBitmap((ID2D1Bitmap*)img->bitmap, rect);
}



