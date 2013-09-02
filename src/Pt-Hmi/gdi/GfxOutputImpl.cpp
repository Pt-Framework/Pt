#include "GfxOutputImpl.h"
#include <Pt/Hmi/GfxModel.h>
#include <Pt/Hmi/Application.h>
#include "ApplicationImpl.h"
#include <Pt/Gfx/Rgb888Color.h>
#include <Pt/Gfx/Rgb888Image.h>
#include <Windows.h>
#include <WindowsX.h>
#include <Pt/Hmi/WindowModel.h>
#include <Pt/Hmi/WindowController.h>
#include <Pt/Hmi/GfxOutput.h>

namespace Pt{
namespace Hmi{

GfxOutputImpl::GfxOutputImpl()
: _model(0)
, _ignoreEvent(false)
, _hwnd(0)
{	

	Pt::Hmi::Application* app = (Pt::Hmi::Application*) &Pt::Hmi::Application::instance();	
	app->impl()->WindowEvent += Pt::slot(*this, &GfxOutputImpl::onWindowEvent);
	_pointerEvent.buttons().resize(3);	
	create();
}

void GfxOutputImpl::create()
{
	HINSTANCE hInstance = GetModuleHandle(NULL);

    _hwnd = CreateWindow( "Pt-Hmi", "", WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 20, 20, 200, 200, GetDesktopWindow(), NULL, hInstance, NULL );
    BringWindowToTop(_hwnd);
	ShowWindow(_hwnd, SW_HIDE);    
	UpdateWindow(_hwnd);	
}

void GfxOutputImpl::onKey(unsigned int msg,  WPARAM wparam, LPARAM lparam)
{
	if(!_model->Enable.get())
		return;

	if((lparam & 0xFFF) != 1)
		return;//Repeat count

	BYTE keyboardState[256];

	GetKeyboardState(keyboardState);

	if(msg == WM_KEYDOWN)
		_keyEvent.setState(KeyEvent::KeyDown);
	else
		_keyEvent.setState(KeyEvent::KeyUp);			
			
	_keyEvent.setAlt( (lparam & 0x20000000) != 0); 
	
	keyboardState[VK_CONTROL] = 0;
	keyboardState[VK_LCONTROL] = 0;
	keyboardState[VK_RCONTROL] = 0;

	if(wparam == 16 )
	{
		_keyEvent.setShift(_keyEvent.state() == KeyEvent::KeyDown);
	}
	else if(wparam == 17 )
	{
		_keyEvent.setCtrl(_keyEvent.state() == KeyEvent::KeyDown);

	}
	//Pt::uint32_t  ucode; 
	unsigned int scanCode = ((lparam >> 16) & 0xFF);			
	Pt::uint32_t ucode;			
		
	ToUnicode( wparam, scanCode , (BYTE*)keyboardState, (LPWSTR)&ucode, 4, 0);	
	_keyEvent.setUnicode(ucode);

	Controller* ctrl = _model->controller();
	Application::instance().keyDeviceEvent().send(ctrl, _keyEvent);
}

void GfxOutputImpl::onWindowEvent(HWND wnd, unsigned int message, unsigned int wparam, long lparam, bool& handled)
{
	if(_hwnd != wnd)
		return;
	
	if(_model == 0)
		return;


	switch(message)
	{
		case WM_LBUTTONDOWN:		
		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_LBUTTONUP:		
		case WM_MBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MOUSEMOVE:
		case WM_LBUTTONDBLCLK:
		case WM_RBUTTONDBLCLK:
		case WM_MBUTTONDBLCLK:
	    {
			onMouse(message, wparam, lparam);
			handled = true;
		}
		break;

		case WM_KEYDOWN:
		case WM_KEYUP:
		{
			onKey(message, wparam, lparam);
			handled = true;
		}
		break;

		case WM_PAINT:
		{
			onPaint();
			handled = true;
		}
		break;
		
		case WM_SIZE:
		{
			onSize(wparam, lparam);
			handled = true;
		}
		break;

		case WM_MOVE:
		{
			onMove();
			handled = true;
		}
		break;

		case WM_DESTROY:
		{
			onClosed();
			handled = true;
		}
		break;

		case WM_CLOSE:
		{
			if( onClosing())
				destroy();	
				
			handled = true;			
		}
		break;
		case WM_KILLFOCUS:
		{
			if(_model->TopMost.get())
			{
				BringWindowToTop(_hwnd);
			}
			handled = true;
		}
		break;		
	}
}

GfxOutputImpl::~GfxOutputImpl()
{
    DestroyWindow(_hwnd);
}

bool GfxOutputImpl::onClosing()
{
	if(!_model->Enable.get())
		return false;

	bool canClose = false;
	WindowController* controller = (WindowController*)_model->controller();
	controller->ClosingAction.send(controller,canClose);
	return canClose;
}

void GfxOutputImpl::onClosed()
{
	WindowController* controller = (WindowController*)_model->controller();
	controller->ClosedAction.send(controller);
}

void GfxOutputImpl::onSize(WPARAM wParam, LPARAM lParam)
{			
	if(!_model->Enable.get())
	{
		setWindowSizeAndPos();
		setWindowProperties();
		return;
	}

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

	getWindowSize();
}

void GfxOutputImpl::onMouse(unsigned int msg, WPARAM wparam, LPARAM lparam)
{	
	int xPos = GET_X_LPARAM(lparam); 
	int yPos = GET_Y_LPARAM(lparam); 

	switch(msg)
	{
		case WM_LBUTTONDOWN:
			_pointerEvent.buttons()[0].setState(Pt::Hmi::DeviceButton::Pressed);
		break;
		
		case WM_LBUTTONUP:		
			_pointerEvent.buttons()[0].setState(Pt::Hmi::DeviceButton::Released);
		break;
							
		case WM_MBUTTONDOWN:
			_pointerEvent.buttons()[1].setState(Pt::Hmi::DeviceButton::Pressed);
		break;

		case WM_MBUTTONUP:
			_pointerEvent.buttons()[1].setState(Pt::Hmi::DeviceButton::Released);
		break;

		case WM_RBUTTONDOWN:		
			_pointerEvent.buttons()[2].setState(Pt::Hmi::DeviceButton::Pressed);
		break;

		case WM_RBUTTONUP:
			_pointerEvent.buttons()[2].setState(Pt::Hmi::DeviceButton::Released);
		break;		
	}
  
	 Pt::Gfx::PointF p = _model->toUnit(Pt::Gfx::Point(xPos, yPos));
	_pointerEvent.setX(p.x());
	_pointerEvent.setY(p.y());	

	Controller* ctrl = _model->controller();

	Application::instance().pointerEvent().send(ctrl, _pointerEvent);
}

void GfxOutputImpl::setWindowSizeAndPos()
{
	WindowModel* wmodel = (WindowModel*) _model;
	Pt::Gfx::Point pos = wmodel->fromUnit(wmodel->WinPos.get());
	Pt::Gfx::Size size = wmodel->fromUnit(wmodel->WinSize.get());
	SetWindowPos(_hwnd,0, pos.x(), pos.y(), size.width(), size.height(), 0);	
}

void GfxOutputImpl::getWindowSize()
{
	WINDOWINFO  info;
	GetWindowInfo(_hwnd, &info);

	//Windows external Size + Pos
	_model->WinSize = _model->toUnit(Pt::Gfx::Size(info.rcWindow.right - info.rcWindow.left, info.rcWindow.bottom - info.rcWindow.top));

	//Windows client Size + pos => Gfx Size + pos
	_model->Size	 = _model->toUnit(Pt::Gfx::Size(info.rcClient.right - info.rcClient.left, info.rcClient.bottom - info.rcClient.top));
}

void GfxOutputImpl::getWindowPos()
{
	WINDOWINFO  info;
	GetWindowInfo(_hwnd, &info);

	//Windows external Size + Pos
	{		
		Pt::Gfx::Point	pos( info.rcWindow.left, info.rcWindow.top);	
		Pt::Gfx::PointF position = _model->toUnit(pos);	
		_model->WinPos	= _model->toUnit(pos);
	}

	//Windows client Size + pos => Gfx Size + pos
	{
		Pt::Gfx::Point	pos( info.rcClient.left, info.rcClient.top);	
		Pt::Gfx::PointF position = _model->toUnit(pos);	
		_model->Position = _model->toUnit(pos);
	}
}

void GfxOutputImpl::onMove()
{
	if(!_model->Enable.get())
	{
		setWindowSizeAndPos();
		setWindowProperties();
		return;
	}

	getWindowPos();
}

void GfxOutputImpl::onPaint()
{   	
	if( _rgb88Image.width() > 0 && _rgb88Image.height() > 0)
		drawIndependentImage(0, 0, (char*)_rgb88Image.data(), _rgb88Image.width(), _rgb88Image.height());
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

void GfxOutputImpl::setWindowProperties()
{
	SetWindowText(_hwnd, _model->Caption.get().c_str());

	long style = 0;
	long exStyle = 0;
	
	//Visibility
	if(_model->Visible.get())
		style |= WS_VISIBLE;
	
	//Title	
	if( _model->ShowTitle.get())
		style |= WS_CAPTION;

	//Minimize button
	if( _model->ShowMinimizeButton.get())
		style |= WS_MINIMIZEBOX;

	//Maximize button
	if( _model->ShowMaximizeButton.get())
		style |= WS_MAXIMIZEBOX;
		
	//System menu
	if( _model->ShowSysMenu.get())
		style |= WS_SYSMENU;
		
	//Windows state
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
	
	//Window border behaviour
	switch( _model->Border.get())
	{
		case Pt::Hmi::WindowBorderType::NoBorder:			
		break;

		case Pt::Hmi::WindowBorderType::Sizeable:
			style |= WS_THICKFRAME;
		break;

		case Pt::Hmi::WindowBorderType::Dialog:
			style |= WS_DLGFRAME;			
		break;

		case Pt::Hmi::WindowBorderType::DialogSizeable:
			style |= WS_DLGFRAME;			
			style |= WS_THICKFRAME;
		break;

		case Pt::Hmi::WindowBorderType::Tool:
			style |= WS_DLGFRAME;
			exStyle |= WS_EX_TOOLWINDOW;
		break;

		case Pt::Hmi::WindowBorderType::ToolSizeable:
			style |= WS_THICKFRAME;
			exStyle |= WS_EX_TOOLWINDOW;
		break;

		default:
			style |= WS_BORDER; 
		break;
	}

	//SHow in taskbar
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
		return;
	}
	else
	{
		if(_hwnd == 0)
		{
			if(wmodel->Visible.get())
				create();
			else
				return;
		}
	}

	setWindowSizeAndPos();
	setWindowProperties();	
	output();
	InvalidateRect(_hwnd, 0, FALSE);
}

void GfxOutputImpl::output()
{
	Pt::Gfx::Size size = _model->fromUnit(_model->Size.get());

	_rgb88Image.resize(_model->PaintBuffer.width(), _model->PaintBuffer.height());

	for( size_t x = 0; x < _model->PaintBuffer.width(); ++x)
	{
		for(size_t y = 0; y < _model->PaintBuffer.height(); ++y)
		{
			const Pt::Gfx::ARgbColor& pixel = _model->PaintBuffer.pixel(x,y);

			Pt::Gfx::Rgb888Color color((Pt::uint8_t) pixel.red(), (Pt::uint8_t) pixel.green(), (Pt::uint8_t) pixel.blue());
			_rgb88Image.setColor(x,y,color);
		}
	}
}

Pt::Gfx::Painter* GfxOutputImpl::nativePainter()
{
	return _nativePainter;
}
}}
