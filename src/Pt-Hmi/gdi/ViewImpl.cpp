/* Copyright (C) 2013 Laurentiu-Gheorghe Crisan
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA*/
#include "ViewImpl.h"
#include <Pt/Hmi/View.h>
#include <Pt/Hmi/NativePaintSurface.h>
#include <Pt/Hmi/ImagePaintSurface.h>
#include "ApplicationImpl.h"
#include <Windows.h>
#include <WindowsX.h>
#include <assert.h>
#include <Pt/Gfx/Rgb888Color.h>
#include <Pt/Gfx/Rgb888Image.h>
#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/WindowModel.h>
#include <Pt/Hmi/WindowController.h>
#include "PaintSurfaceImpl.h"

namespace Pt{
namespace Hmi{

ViewImpl::ViewImpl()
: _hwnd(0)
, _model(0)
, _nativePainter(0)
, _ignoreSizePositionEvent(false)
{	
	Pt::Hmi::Application* app = (Pt::Hmi::Application*) &Pt::Hmi::Application::instance();	
	app->impl()->WindowEvent += Pt::slot(*this, &ViewImpl::onWindowEvent);
	_pointerEvent.buttons().resize(3);	
	create();
}

void ViewImpl::create()
{
	HINSTANCE hInstance = GetModuleHandle(NULL);

    _hwnd = CreateWindow( "Pt-Hmi", "", WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 20, 20, 200, 200, GetDesktopWindow(), NULL, hInstance, NULL );
    BringWindowToTop(_hwnd);
	ShowWindow(_hwnd, SW_HIDE);    
	UpdateWindow(_hwnd);	
}

void ViewImpl::onKey(unsigned int msg,  WPARAM wparam, LPARAM lparam)
{
	if(!_model->Enabled.get())
		return;

	BYTE keyboardState[256];

	GetKeyboardState(keyboardState);

	if(msg == WM_KEYDOWN)
		_keyEvent.setState(KeyEvent::KeyDown);
	else if(msg == WM_KEYUP)
		_keyEvent.setState(KeyEvent::KeyUp);			
	
	if(msg == WM_SYSCOMMAND)
	{
		_keyEvent.setState(KeyEvent::KeyUp);
		_keyEvent.setAlt(wparam == SC_KEYMENU);
		_keyEvent.setShift(false);
		_keyEvent.setCtrl(false);
		_keyEvent.setUnicode(lparam);
	}
	else
	{
		if((lparam & 0xFFF) != 1)
			return;//Repeat count

		keyboardState[VK_CONTROL] = 0;
		keyboardState[VK_LCONTROL] = 0;
		keyboardState[VK_RCONTROL] = 0;

		if(wparam == 16 )
		{//Shift key
			_keyEvent.setShift(_keyEvent.state() == KeyEvent::KeyDown);
		}
		else if(wparam == 17 )
		{//Controll key
			_keyEvent.setCtrl(_keyEvent.state() == KeyEvent::KeyDown);
		}

		Pt::uint32_t scanCode = ((lparam >> 16) & 0xFF);			
		Pt::uint32_t ucode = 0;			
		
		ToUnicode( wparam, scanCode , (BYTE*)keyboardState, (LPWSTR)&ucode, 4, 0);	
		_keyEvent.setUnicode(ucode);
	}

	_keyEvent.setController(_controller);

	Application::instance().systemEvent().send(_keyEvent);
}

void ViewImpl::onWindowEvent(HWND wnd, unsigned int message, unsigned int wparam, long lparam, bool& handled)
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

		case WM_SYSCOMMAND:
			onKey(message, wparam, lparam);
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
				handled = true;
			}
		}
		break;		
	}
}

ViewImpl::~ViewImpl()
{
    DestroyWindow(_hwnd);
}

bool ViewImpl::onClosing()
{
	if(!_model->Enabled.get())
		return false;

	bool canClose = false;
	_controller->ClosingAction.send(_controller,canClose);
	return canClose;
}

void ViewImpl::onClosed()
{
	_controller->ClosedAction.send(_controller);
}

void ViewImpl::onSize(WPARAM wParam, LPARAM lParam)
{
	if(_ignoreSizePositionEvent)
		return;

	switch(wParam)
	{
		case SIZE_MAXHIDE:
		case SIZE_MAXSHOW:

		break;

		case SIZE_MAXIMIZED:
			_model->WindowState = WindowStateType::Maximazed;							
		break;

		case SIZE_MINIMIZED:
			_model->WindowState = WindowStateType::Minimized;			
		break;
 
		case SIZE_RESTORED:
			_model->WindowState = WindowStateType::Normal;
			updateModelSizeAndPos();
		break;
	}

}

void ViewImpl::onMouse(unsigned int msg, WPARAM wparam, LPARAM lparam)
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

	_pointerEvent.setController(_controller);	

	Application::instance().systemEvent().send(_pointerEvent);
}

void ViewImpl::setWindowSizeAndPos(bool firstShow)
{
	_ignoreSizePositionEvent = true;
	
	RECT  info;
	GetWindowRect(_hwnd, &info);

	Pt::Gfx::Size winSize = Pt::Gfx::Size(info.right - info.left, info.bottom - info.top);
	Pt::Gfx::Point winPos = Pt::Gfx::Point(info.left, info.right);
	
	Pt::Gfx::Point pos = _model->fromUnit(_model->Position.get());
	Pt::Gfx::Size size = _model->fromUnit(_model->Size.get());

	if( winSize != size ||  pos != winPos) 
		SetWindowPos(_hwnd,0, pos.x(), pos.y(), size.width(), size.height(), 0);	
	
	if(firstShow)
	{
		switch(_model->WindowStartPostion.get())
		{
			case WindowStartPositionType::Manual:
			{
				Pt::Gfx::Point pos = _model->fromUnit(_model->Position.get());
				Pt::Gfx::Size size = _model->fromUnit(_model->Size.get());
				SetWindowPos(_hwnd,0, pos.x(), pos.y(), size.width(), size.height(), 0);
			}
			break;
			
			case WindowStartPositionType::CenterParent:
			{				
				WindowController* parent = _controller->windowParent();
				
				if( parent == 0)
				{
					centerWindowTo(GetDesktopWindow());
				}
				else
				{
					ViewImpl* parentImpl = 0;

					for(size_t i = 0; i < parent->outputDevices().size(); ++i)
					{	
						View* parentDevice = dynamic_cast<View*>(parent->outputDevices()[i]);

						if( parentDevice == 0)
							continue;

						parentImpl = dynamic_cast<ViewImpl*>(parentDevice->impl());
						
						if( parentImpl != 0)
							break;
					}

					centerWindowTo(parentImpl->hwnd());
				}
			}
			break;

			case WindowStartPositionType::CenterScreen:
				centerWindowTo(GetDesktopWindow());
			break;
		}
	}

	_ignoreSizePositionEvent = false;
}

void ViewImpl::centerWindowTo(HWND parent)
{
	RECT parentRect;   
	GetWindowRect(parent, &parentRect);
	int horizontal = parentRect.left + ((parentRect.right - parentRect.left )/2);
	int vertical = parentRect.top + (parentRect.bottom - parentRect.top)/2;
	Pt::Gfx::Size mySize  = _model->fromUnit(_model->Size.get());

	int posX = horizontal - (mySize.width()/2);
	int posY = vertical - (mySize.height()/2);
				
	_model->Position.set(_model->toUnit(Pt::Gfx::Point( posX,posY)));
	Pt::Gfx::Size size = _model->fromUnit(_model->Size.get());
	SetWindowPos(_hwnd,0, posX, posY, size.width(), size.height(), SWP_DRAWFRAME);
}

void ViewImpl::updateModelSizeAndPos()
{
	RECT  info;
	GetWindowRect(_hwnd, &info);
	Pt::Gfx::Size winSize(info.right - info.left, info.bottom - info.top);
	Pt::Gfx::Point winPos(info.left, info.right);

	Pt::Gfx::Size curSize = _model->fromUnit(_model->Size.get());
	Pt::Gfx::Point curPos = _model->fromUnit(_model->Position.get());

	if( winPos != curPos) 
		_model->Position = _model->toUnit(winPos);

	if( winSize != curSize) 
		_model->Size = _model->toUnit(winSize);
}

void ViewImpl::onMove()
{
	if(_ignoreSizePositionEvent)
		return;

	if(!_model->Enabled.get())
	{
		setWindowSizeAndPos(false);
		setWindowProperties();
		return;
	}

	updateModelSizeAndPos();
}

void ViewImpl::onPaint()
{   		
	if(_model == 0)
		return;

	PAINTSTRUCT ps;
    HDC windowContext = BeginPaint(_hwnd, &ps);
	Pt::Gfx::Size size = _model->fromUnit(_model->Size.get());
	Pt::Hmi::PaintSurface* surface = _model->paintSurface();
	Pt::Hmi::NativePaintSurface* nativePaintSurface = dynamic_cast<Pt::Hmi::NativePaintSurface*>(surface);
	Pt::Hmi::ImagePaintSurface* imagePaintSurface = dynamic_cast<Pt::Hmi::ImagePaintSurface*>(surface);

	if(nativePaintSurface != 0)
	{
		HDC bitmapDeviceConText = nativePaintSurface->impl()->deviceContext();
		BitBlt(windowContext, 0, 0, size.width(), size.height(), bitmapDeviceConText, 0, 0, SRCCOPY);	
		EndPaint(_hwnd, &ps);	
	}
	else if(imagePaintSurface != 0)
	{
		//TODO: draw image
	}
}

void ViewImpl::setWindowIcon()
{
	if( _model->Icon.get().width() == 0 ||  _model->Icon.get().height() == 0)
		return;

	HINSTANCE hInstance = GetModuleHandle(NULL);
	const size_t planes = 4;
	std::vector<Pt::uint8_t> bitmapBuffer(_model->Icon.get().width() * _model->Icon.get().height() *planes);
		
	for(size_t y = 0; y <_model->Icon.get().height(); ++y)
	{
		const size_t offsetLine = y * (_model->Icon.get().width()*planes);

		for(size_t x = 0; x < _model->Icon.get().width(); ++x)
		{
			const size_t index  = offsetLine + (x*planes);

			const Pt::Gfx::ARgbColor& pix =  _model->Icon.get().pixel(x,y);
				
			bitmapBuffer[index]     = static_cast<unsigned char>(pix.blue());	
			bitmapBuffer[index + 1] = static_cast<unsigned char>(pix.green());
			bitmapBuffer[index + 2] = static_cast<unsigned char>(pix.red());
			bitmapBuffer[index + 3] = static_cast<unsigned char>(pix.alpha());
		}		
	}

	HICON icon = ::CreateIcon(GetModuleHandle(NULL), _model->Icon.get().width(), _model->Icon.get().height(), 4, 8, 0, (BYTE*)&bitmapBuffer[0]);
	SetClassLong(_hwnd, GCL_HICON, (LONG)icon); 	
}

void ViewImpl::setWindowProperties()
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

	//Show in taskbar
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


	SetWindowLong(_hwnd, GWL_STYLE, style);  

	bool visible = ((styleVisible & WS_VISIBLE) == WS_VISIBLE);

	if(!_model->Visible.get() && visible)
		ShowWindow(_hwnd, SW_HIDE);
	
	if( !visible && _model->Visible.get())
		ShowWindow(_hwnd, SW_SHOW);		
}

void ViewImpl::destroy()
{
	if( _hwnd == 0)
		return;

	DestroyWindow(_hwnd);
	_hwnd = 0;
}

void ViewImpl::output(Pt::Hmi::Controller* controller, Pt::Hmi::Model* model)
{
	bool firstShow =  (_model == 0);
	_model = dynamic_cast<WindowModel*>(model);
	_controller = dynamic_cast<WindowController*>(controller);

	assert(_model != 0);
	assert(_controller != 0);	

	//Check create/destroy
	if(_model->Closed.get())
	{
		
		if(_hwnd != 0)
			destroy();

		_model = 0;
		return;
	}
	else
	{
		if(_hwnd == 0)
		{
			if(_model->Visible.get())
			{
				create();
			}
			else
			{
				_model = 0;
				return;
			}
		}
	}

	setWindowSizeAndPos(firstShow);
	setWindowProperties();	
	setWindowIcon();	
	InvalidateRect(_hwnd, NULL, FALSE);
}

}}
