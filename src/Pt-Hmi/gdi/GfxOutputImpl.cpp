#include "GfxOutputImpl.h"
#include <Pt/Hmi/GfxModel.h>
#include <Pt/Hmi/Application.h>
#include "ApplicationImpl.h"
#include <Pt/Gfx/Rgb888Color.h>
#include <Pt/Gfx/Rgb888Image.h>
#include <Windows.h>

namespace Pt{
namespace Hmi{

GfxOutputImpl::GfxOutputImpl()
: _model(0)
{
	HINSTANCE hInstance = GetModuleHandle(NULL);

    _hwnd = CreateWindow( "Pt-Hmi", "", WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 20, 20, 200, 200, NULL, NULL, hInstance, NULL );
	SetWindowLong(_hwnd, GWL_STYLE, 0); 
    BringWindowToTop(_hwnd);
	ShowWindow(_hwnd, SW_HIDE);    
	UpdateWindow(_hwnd);	
	Pt::Hmi::Application* app = (Pt::Hmi::Application*) &Pt::Hmi::Application::instance();
	
	app->impl()->PaintEvent += Pt::slot(*this,&GfxOutputImpl::OnPaint);
}

GfxOutputImpl::~GfxOutputImpl()
{
    DestroyWindow(_hwnd);
}


void GfxOutputImpl::OnPaint(HWND hwnd)
{
	if( _hwnd != hwnd)
		return;

	if(_model == 0)
		return;

	GfxModel* gfxModel = dynamic_cast<GfxModel*>(_model);
    
	Pt::Gfx::Size size = gfxModel->fromUnit(gfxModel->Size.get());

	Pt::Gfx::Rgb888Image rgb88Image(gfxModel->PaintBuffer.width(), gfxModel->PaintBuffer.height());

	for( size_t x = 0; x < gfxModel->PaintBuffer.width(); ++x)
	{
		for(size_t y = 0; y < gfxModel->PaintBuffer.height(); ++y)
		{
			const Pt::Gfx::ARgbColor& pixel = gfxModel->PaintBuffer.pixel(x,y);

			Pt::Gfx::Rgb888Color color(pixel.red(), pixel.green(), pixel.blue());
			rgb88Image.setColor(x,y,color);
		}
	}
		

    drawIndependentImage(0, 0, (char*)rgb88Image.data(), size.width(), size.height());
}

void GfxOutputImpl::drawIndependentImage(size_t x, size_t y, const char* data, size_t width, size_t height)
{
   PAINTSTRUCT ps;

    HDC hdc = BeginPaint(_hwnd, &ps);

    BITMAPINFO bitmapInfo;
    ZeroMemory(&bitmapInfo.bmiHeader, sizeof(BITMAPINFOHEADER));

    bitmapInfo.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER); // Size of this struct.
    bitmapInfo.bmiHeader.biWidth       = width;             // Bitmap width.
    bitmapInfo.bmiHeader.biHeight      = -(ssize_t)height;  // Bitmap height. Negative value = top-down image.
    bitmapInfo.bmiHeader.biPlanes      = 1;                 // Always 1.
    bitmapInfo.bmiHeader.biBitCount    = 32;                // We internally use a 32-bit bitmap.
    bitmapInfo.bmiHeader.biCompression = BI_RGB;            // Uncompressed (top-down) RGB bitmap.
    bitmapInfo.bmiHeader.biSizeImage   = 0;                 // 0 = automatic for BI_RGB-images.
    bitmapInfo.bmiHeader.biClrUsed     = 0;                 // 0 = No color table.
    bitmapInfo.bmiHeader.biClrImportant= 0;                 // 0 = No color table.

    VOID* imageBits;
    HBITMAP bitmap = CreateDIBSection(hdc, &bitmapInfo, DIB_RGB_COLORS, &imageBits, NULL, 0);

    memcpy(imageBits, data, width * height * 4);

    HDC bitmapDeviceConText = CreateCompatibleDC(NULL);
    SelectObject(bitmapDeviceConText, bitmap);	
    BOOL ret = BitBlt(hdc, x, y, width, height, bitmapDeviceConText, 0, 0, SRCCOPY);

    DeleteDC(bitmapDeviceConText);
    DeleteObject(bitmap);
	EndPaint(_hwnd, &ps);
}

void GfxOutputImpl::output(Pt::Hmi::Model* model)
{
	GfxModel* gfxModel = dynamic_cast<GfxModel*>(model);

	_model = model;

	if( gfxModel == 0)
		throw std::logic_error("GFX Model expected");

	if(!gfxModel->Visible.get())
		ShowWindow(_hwnd, SW_HIDE);
	else
		ShowWindow(_hwnd, SW_SHOW);

	Application& app = *((Application*) &Application::instance());

	Pt::Gfx::Point pos = app.fromUnit(gfxModel->Position.get());
	Pt::Gfx::Size size = app.fromUnit(gfxModel->Size.get());

	SetWindowPos(_hwnd, 0, pos.x(), pos.y(), size.width(),size.height(), 0);
	InvalidateRect(_hwnd, 0, FALSE);
}


Pt::Gfx::Painter* GfxOutputImpl::nativePainter()
{
	return _nativePainter;
}
}}