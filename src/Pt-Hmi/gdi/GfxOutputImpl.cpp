#include "GfxOutputImpl.h"
#include <Pt/Hmi/GfxModel.h>
#include <Pt/Hmi/Application.h>
#include "ApplicationImpl.h"
#include <Pt/Gfx/Rgb888Color.h>
#include <Pt/Gfx/Rgb888Image.h>
#include <Windows.h>
#include <Pt/Hmi/WindowModel.h>

namespace Pt{
namespace Hmi{

GfxOutputImpl::GfxOutputImpl()
: _model(0)
, _ignoreSizeEvent(false)
{
	HINSTANCE hInstance = GetModuleHandle(NULL);

    _hwnd = CreateWindow( "Pt-Hmi", "", WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 20, 20, 200, 200, NULL, NULL, hInstance, NULL );
    BringWindowToTop(_hwnd);
	ShowWindow(_hwnd, SW_HIDE);    
	UpdateWindow(_hwnd);	
	Pt::Hmi::Application* app = (Pt::Hmi::Application*) &Pt::Hmi::Application::instance();
	
	app->impl()->PaintEvent += Pt::slot(*this,&GfxOutputImpl::onPaint);
	app->impl()->SizeEvent += Pt::slot(*this,&GfxOutputImpl::onSize);
	app->impl()->MoveEvent += Pt::slot(*this,&GfxOutputImpl::onMove);
}

GfxOutputImpl::~GfxOutputImpl()
{
    DestroyWindow(_hwnd);
}

void GfxOutputImpl::onSize(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	if( _hwnd != hwnd)
		return;

	if(_model == 0)
		return;

	if( _ignoreSizeEvent)
		return;

	WindowModel* winMod = (WindowModel*) _model;
			
	switch(wParam)
	{
		case SIZE_MAXHIDE:
		case SIZE_MAXSHOW:
			return;
		break;

		case SIZE_MAXIMIZED:
			winMod->WindowState = WindowStateType::Maximazed;
		break;

		case SIZE_MINIMIZED:
			winMod->WindowState = WindowStateType::Minimized;
		break;
 
		case SIZE_RESTORED:
			winMod->WindowState = WindowStateType::Normal;
		break;
	}

	readWindowSizeAndPos();
}


void GfxOutputImpl::writeWindowSizeAndPos()
{
	WindowModel* wmodel = (WindowModel*) _model;
	Pt::Gfx::Point pos = wmodel->fromUnit(wmodel->WinPos.get());
	Pt::Gfx::Size size = wmodel->fromUnit(wmodel->WinSize.get());
	SetWindowPos(_hwnd,0, pos.x(), pos.y(), size.width(), size.height(), 0);
	 
	WINDOWINFO  info;
	GetWindowInfo(_hwnd, &info);
	
	wmodel->Size = wmodel->toUnit(Pt::Gfx::Size( info.rcClient.right - info.rcClient.left, info.rcClient.bottom- info.rcClient.top));
	wmodel->Position = wmodel->toUnit(Pt::Gfx::Point( info.rcClient.left, info.rcClient.top));
}

void GfxOutputImpl::readWindowSizeAndPos()
{
	WindowModel* winMod = (WindowModel*) _model;

	WINDOWINFO  info;
	GetWindowInfo(_hwnd, &info);

	//Windows external Size + Pos
	{		
		Pt::Gfx::Point	pos( info.rcWindow.left, info.rcWindow.top);	
		Pt::Gfx::PointF position = winMod->toUnit(pos);
	
		winMod->WinSize = winMod->toUnit(Pt::Gfx::Size(info.rcWindow.right - info.rcWindow.left, info.rcWindow.bottom - info.rcWindow.top));
		winMod->WinPos	= winMod->toUnit(pos);
	}

	//Windows client Size + pos => Gfx Size + pos
	{
		Pt::Gfx::Point	pos( info.rcClient.left, info.rcClient.top);	
		Pt::Gfx::PointF position = winMod->toUnit(pos);
	
		winMod->Size	 = winMod->toUnit(Pt::Gfx::Size(info.rcClient.right - info.rcClient.left, info.rcClient.bottom - info.rcClient.top));
		winMod->Position = winMod->toUnit(pos);
	}
}

void GfxOutputImpl::onMove(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	if( _hwnd != hwnd)
		return;

	if(_model == 0)
		return;

	if( _ignoreSizeEvent)
		return;

	readWindowSizeAndPos();
}

void GfxOutputImpl::onPaint(HWND hwnd)
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

void GfxOutputImpl::writeWindowProperties()
{
	WindowModel* wmodel = (WindowModel*) _model;

	SetWindowText(_hwnd, wmodel->Caption.get().c_str());

	long style= GetWindowLong(_hwnd, GWL_STYLE);

	if( wmodel->ShowInTaskbar.get())
	{
		style |= WS_EX_APPWINDOW;   // flags don't work - windows remains in taskbar
	}
	else
	{
		style &= ~(WS_EX_APPWINDOW); 
	}

	SetWindowLong(_hwnd, GWL_STYLE, style);  

	if(!wmodel->Visible.get())
		ShowWindow(_hwnd, SW_HIDE);
	else
		ShowWindow(_hwnd, SW_SHOW);
	
}

void GfxOutputImpl::output(Pt::Hmi::Model* model)
{
	WindowModel* wmodel = dynamic_cast<WindowModel*>(model);

	_model = model;

	if( wmodel == 0)
		throw std::logic_error("ERROR: WindowModel model expected!");

	_ignoreSizeEvent = true;	

	writeWindowSizeAndPos();
	writeWindowProperties();
	
	_ignoreSizeEvent = false;

	InvalidateRect(_hwnd, 0, FALSE);
}


Pt::Gfx::Painter* GfxOutputImpl::nativePainter()
{
	return _nativePainter;
}
}}