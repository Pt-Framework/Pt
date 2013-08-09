#include "GfxOutputImpl.h"
#include <Pt/Hmi/GfxModel.h>
#include <Pt/Hmi/Application.h>
#include "ApplicationImpl.h"
#include <Pt/Gfx/Rgb888Color.h>
#include <Pt/Gfx/Rgb888Image.h>
#include <Windows.h>
#include <Pt/Hmi/WindowModel.h>
#include <Pt/Hmi/WindowController.h>

namespace Pt{
namespace Hmi{

GfxOutputImpl::GfxOutputImpl()
: _model(0)
, _ignoreSizeEvent(false)
{
	create();
}


void GfxOutputImpl::create()
{
	HINSTANCE hInstance = GetModuleHandle(NULL);

    _hwnd = CreateWindow( "Pt-Hmi", "", WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 20, 20, 200, 200, GetDesktopWindow(), NULL, hInstance, NULL );
    BringWindowToTop(_hwnd);
	ShowWindow(_hwnd, SW_HIDE);    
	UpdateWindow(_hwnd);	
	Pt::Hmi::Application* app = (Pt::Hmi::Application*) &Pt::Hmi::Application::instance();
	
	app->impl()->PaintEvent += Pt::slot(*this,&GfxOutputImpl::onPaint);
	app->impl()->SizeEvent += Pt::slot(*this,&GfxOutputImpl::onSize);
	app->impl()->MoveEvent += Pt::slot(*this,&GfxOutputImpl::onMove);
	app->impl()->ClosingEvent += Pt::slot(*this,&GfxOutputImpl::onClosing);
	app->impl()->ClosedEvent += Pt::slot(*this,&GfxOutputImpl::onClosed);
}

GfxOutputImpl::~GfxOutputImpl()
{
    DestroyWindow(_hwnd);
}

void GfxOutputImpl::onClosing(HWND hwnd, WPARAM wparam, LPARAM lparam, bool& canClose)
{
	if( _hwnd != hwnd)
		return;


	if(_model == 0)
		return;

	WindowController* controller = (WindowController*)_model->Controller.get();
	controller->Closing.send(canClose);
}

void GfxOutputImpl::onClosed(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
	if( _hwnd != hwnd)
		return;

	if(_model == 0)
		return;

	WindowController* controller = (WindowController*)_model->Controller.get();
	controller->Closed.send();
			
	 _model->Closed = true;
}

void GfxOutputImpl::onSize(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	if( _hwnd != hwnd)
		return;

	if(_model == 0)
		return;

	if( _ignoreSizeEvent)
		return;

			
	switch(wParam)
	{
		case SIZE_MAXHIDE:
		case SIZE_MAXSHOW:
			return;
		break;

		case SIZE_MAXIMIZED:
			_model->WindowState = WindowStateType::Maximazed;
		break;

		case SIZE_MINIMIZED:
			_model->WindowState = WindowStateType::Minimized;
		break;
 
		case SIZE_RESTORED:
			_model->WindowState = WindowStateType::Normal;
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
	WINDOWINFO  info;
	GetWindowInfo(_hwnd, &info);

	//Windows external Size + Pos
	{		
		Pt::Gfx::Point	pos( info.rcWindow.left, info.rcWindow.top);	
		Pt::Gfx::PointF position = _model->toUnit(pos);
	
		_model->WinSize = _model->toUnit(Pt::Gfx::Size(info.rcWindow.right - info.rcWindow.left, info.rcWindow.bottom - info.rcWindow.top));
		_model->WinPos	= _model->toUnit(pos);
	}

	//Windows client Size + pos => Gfx Size + pos
	{
		Pt::Gfx::Point	pos( info.rcClient.left, info.rcClient.top);	
		Pt::Gfx::PointF position = _model->toUnit(pos);
	
		_model->Size	 = _model->toUnit(Pt::Gfx::Size(info.rcClient.right - info.rcClient.left, info.rcClient.bottom - info.rcClient.top));
		_model->Position = _model->toUnit(pos);
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

   
	Pt::Gfx::Size size = _model->fromUnit(_model->Size.get());

	Pt::Gfx::Rgb888Image rgb88Image(_model->PaintBuffer.width(), _model->PaintBuffer.height());

	for( size_t x = 0; x < _model->PaintBuffer.width(); ++x)
	{
		for(size_t y = 0; y < _model->PaintBuffer.height(); ++y)
		{
			const Pt::Gfx::ARgbColor& pixel = _model->PaintBuffer.pixel(x,y);

			Pt::Gfx::Rgb888Color color(pixel.red(), pixel.green(), pixel.blue());
			rgb88Image.setColor(x,y,color);
		}
	}
		

	if( size.width() > 0 && size.height() > 0)
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
	SetWindowText(_hwnd, _model->Caption.get().c_str());

	long style = 0;
	long exStyle = 0;

	if(_model->Visible.get())
		style |= WS_VISIBLE;
		
	if( _model->ShowTitle.get())
		style |= WS_CAPTION;

	if( _model->ShowMinimizeBt.get())
		style |= WS_MINIMIZEBOX;

	if( _model->ShowMaximizeBt.get())
		style |= WS_MAXIMIZEBOX;

	if( _model->ShowSysMenu.get())
		style |= WS_SYSMENU;

	switch(_model->WindowState.get())
	{
		case Pt::Hmi::WindowStateType::Normal:
		break;

		case Pt::Hmi::WindowStateType::Maximazed:
			style |= WS_MAXIMIZE;
		break;

		case Pt::Hmi::WindowStateType::Minimized:
			style |= WS_MINIMIZE;
		break;
	}

	switch( _model->Border.get())
	{
		case Pt::Hmi::BorderStyle::None:
			
		break;

		case Pt::Hmi::BorderStyle::Single:
			style |= WS_BORDER; 
		break;

		case Pt::Hmi::BorderStyle::Sizebale:
			style |= WS_THICKFRAME;
		break;

		case Pt::Hmi::BorderStyle::Dialog:
			style |= WS_DLGFRAME;			
			exStyle |= WS_EX_DLGMODALFRAME;
		break;

		case Pt::Hmi::BorderStyle::DialogSizeable:
			style |= WS_DLGFRAME;			
			exStyle |= WS_EX_DLGMODALFRAME;
			style |= WS_THICKFRAME;
		break;

		case Pt::Hmi::BorderStyle::Tool:
			style |= WS_DLGFRAME;
			exStyle |= WS_EX_TOOLWINDOW;
		break;

		case Pt::Hmi::BorderStyle::ToolSizeable:
			style |= WS_THICKFRAME;
			exStyle |= WS_EX_TOOLWINDOW;
		break;

	}

	if(_model->ShowInTaskbar.get())
	{
		exStyle |= WS_EX_APPWINDOW;  
		SetWindowLong(_hwnd, GWL_EXSTYLE, exStyle); 
	}
	else
	{
		SetWindowLong(_hwnd, GWL_STYLE, style);  
	}

	long styleVisible = GetWindowLong(_hwnd, GWL_STYLE);  

	bool visible = ((styleVisible & WS_VISIBLE) == WS_VISIBLE);

	if(!_model->Visible.get())
		ShowWindow(_hwnd, SW_HIDE);
	else if( !visible)
		ShowWindow(_hwnd, SW_SHOW);		

	SetWindowLong(_hwnd, GWL_STYLE, style);  
}

void GfxOutputImpl::destroy()
{
	if( _hwnd == 0)
		return;

	DestroyWindow(_hwnd);
	_hwnd = 0;
}

void GfxOutputImpl::output(Pt::Hmi::Model* model)
{
	WindowModel* wmodel = dynamic_cast<WindowModel*>(model);

	_model = wmodel;

	if( wmodel == 0)
		throw std::logic_error("ERROR: WindowModel model expected!");

	if(wmodel->Closed.get())
	{
		if(_hwnd != 0)
			destroy();
	}
	else
	{
		if(_hwnd == 0)
			create();
	}

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
