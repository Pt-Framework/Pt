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
#include <Pt/Forms/View.h>
#include "ApplicationImpl.h"
#include <assert.h>
#include <Pt/Gfx/Rgb888Color.h>
#include <Pt/Gfx/Rgb888Image.h>
#include <Pt/Forms/Application.h>
#include <Pt/Forms/WindowModel.h>
#include <Pt/Forms/Window.h>

namespace Pt{
namespace Forms{

ViewImpl::ViewImpl()
: _model(0)
, _nativePainter(0)
, _ignoreEvent(false)
{	
	Pt::Forms::Application* app = (Pt::Forms::Application*) &Pt::Forms::Application::instance();	
	
	//app->impl()->WindowEvent += Pt::slot(*this, &ViewImpl::onWindowEvent);
	_pointerEvent.buttons().resize(3);	
	create();
}

void ViewImpl::create()
{

}

void ViewImpl::onKey()
{
	if(!_model->Enable.get())
		return;
	
	Pt::uint32_t ucode= 0;

	_keyEvent.setUnicode(ucode);

	Controller* ctrl = _model->controller();
	Application::instance().keyDeviceEvent().send(ctrl, _keyEvent);
}


ViewImpl::~ViewImpl()
{
    destroy();
}

bool ViewImpl::onClosing()
{
	if(!_model->Enable.get())
		return false;

	
	return _controller->close();
}

void ViewImpl::onClosed()
{
	Window* controller = (Window*)_model->controller();
	controller->ClosedAction.send(controller);
}

void ViewImpl::onSize()
{			

}

void ViewImpl::onMouse()
{	
	int xPos = 0;
	int yPos = 0; 
	
	/*
	
	switch(msg)
	{
		case WM_LBUTTONDOWN:
			_pointerEvent.buttons()[0].setState(Pt::Forms::DeviceButton::Pressed);
		break;
		
		case WM_LBUTTONUP:		
			_pointerEvent.buttons()[0].setState(Pt::Forms::DeviceButton::Released);
		break;
							
		case WM_MBUTTONDOWN:
			_pointerEvent.buttons()[1].setState(Pt::Forms::DeviceButton::Pressed);
		break;

		case WM_MBUTTONUP:
			_pointerEvent.buttons()[1].setState(Pt::Forms::DeviceButton::Released);
		break;

		case WM_RBUTTONDOWN:		
			_pointerEvent.buttons()[2].setState(Pt::Forms::DeviceButton::Pressed);
		break;

		case WM_RBUTTONUP:
			_pointerEvent.buttons()[2].setState(Pt::Forms::DeviceButton::Released);
		break;		
	}*/
  
	 Pt::Gfx::PointF p = _model->toUnit(Pt::Gfx::Point(xPos, yPos));
	_pointerEvent.setX(p.x());
	_pointerEvent.setY(p.y());	

	Controller* ctrl = _model->controller();

	Application::instance().pointerEvent().send(ctrl, _pointerEvent);
}

void ViewImpl::setWindowSizeAndPos(bool firstShow)
{
	WindowModel* model = _controller->windowModel();

	if(!firstShow)
	{
		Pt::Gfx::Point pos = _controller->windowModel()->fromUnit(_controller->windowModel()->WinPos.get());
		Pt::Gfx::Size size = _controller->windowModel()->fromUnit(_controller->windowModel()->WinSize.get());
		//SetWindowPos(_hwnd,0, pos.x(), pos.y(), size.width(), size.height(), 0);	
		return;
	}

	switch(_model->WindowStartPostion.get())
	{
		case WindowStartPositionType::Manual:
			setWindowSizeAndPos(false);
		break;
			
		case WindowStartPositionType::CenterParent:
		{
			Window* controller = (Window*) _model->controller();
			Window* parent = controller->windowParent();
				
			if( parent == 0)
			{
				//TODO: desktop
				//centerWindowTo(0);
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

				//centerWindowTo(parentImpl);
			}
		}
		break;

		case WindowStartPositionType::CenterScreen:
			//centerWindowTo(GetDesktopWindow());
		break;
	}
}

void ViewImpl::centerWindowTo(void* parent)
{
	/*
	RECT parentRect;   
	GetWindowRect(parent, &parentRect);
	int horizontal = parentRect.left + ((parentRect.right - parentRect.left )/2);
	int vertical = parentRect.top + (parentRect.bottom - parentRect.top)/2;
	Pt::Gfx::Size mySize  = _model->fromUnit(_model->WinSize.get());

	int posX = horizontal - (mySize.width()/2);
	int posY = vertical - (mySize.height()/2);
				
	_model->WinPos.set(_model->toUnit(Pt::Gfx::Point( posX,posY)));
	Pt::Gfx::Size size = _model->fromUnit(_model->WinSize.get());
	SetWindowPos(_hwnd,0, posX, posY, size.width(), size.height(), SWP_DRAWFRAME);
	*/
}

void ViewImpl::getWindowSize()
{	
	/*
	WINDOWINFO  info;
	//GetWindowInfo(_hwnd, &info);

	//Windows external Size + Pos
	_model->WinSize = _model->toUnit(Pt::Gfx::Size(info.rcWindow.right - info.rcWindow.left, info.rcWindow.bottom - info.rcWindow.top));

	//Windows client Size + pos => Gfx Size + pos
	_model->Size = _model->toUnit(Pt::Gfx::Size(info.rcClient.right - info.rcClient.left, info.rcClient.bottom - info.rcClient.top));
	*/
}

void ViewImpl::getWindowPos()
{
	/*
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
	}*/
}

void ViewImpl::onMove()
{
	if(!_model->Enable.get())
	{
		setWindowSizeAndPos(false);
		setWindowProperties();
		return;
	}

	getWindowPos();
}

void ViewImpl::onPaint()
{   		

	if( _rgb88Image.width() > 0 && _rgb88Image.height() > 0)
		drawIndependentImage(0, 0, (char*)_rgb88Image.data(), _rgb88Image.width(), _rgb88Image.height());
}

void ViewImpl::drawIndependentImage(size_t x, size_t y, const char* data, size_t width, size_t height)
{
	/*
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
	EndPaint(_hwnd, &ps);*/
}


void ViewImpl::setWindowIcon()
{
	/*
	if( _model->Icon.get().width() == 0 ||  _model->Icon.get().height() == 0)
		return;

	HINSTANCE hInstance = GetModuleHandle(NULL);
	const size_t planes = 4;
	std::vector<Pt::uint8_t> bitmapBuffer(_model->Icon.get().width() * _model->Icon.get().height() *planes);
		
	for(size_t y = 0; y <_model->Icon.get().height(); ++y)
	{
		const size_t offsetLine = y * (_model->Icon.get().width()*planes);

		for(size_t x = 0; x <_model->Icon.get().width(); ++x)
		{
			const size_t index  = offsetLine + (x*planes);

			const Pt::Gfx::ARgbColor& pix =  _model->Icon.get().pixel(x,y);
				
			bitmapBuffer[index]     = pix.blue();	
			bitmapBuffer[index + 1] = pix.green();
			bitmapBuffer[index + 2] = pix.red();

			bitmapBuffer[index + 3] = pix.alpha();
		}		
	}
	HICON icon = ::CreateIcon(GetModuleHandle(NULL), _model->Icon.get().width(), _model->Icon.get().height(), 4, 8, 0, (BYTE*)&bitmapBuffer[0]);
	SetClassLong(_hwnd, GCL_HICON, (LONG)icon);*/ 	
}

void ViewImpl::setWindowProperties()
{
/*
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
		case Pt::Forms::WindowStateType::Normal:			
		break;

		case Pt::Forms::WindowStateType::Maximazed:
			style |= WS_MAXIMIZE;
		break;

		case Pt::Forms::WindowStateType::Minimized:
			style |= WS_MINIMIZE;
		break;
	}
	
	//Window border behaviour
	switch( _model->Border.get())
	{
		case Pt::Forms::WindowBorderType::NoBorder:			
		break;

		case Pt::Forms::WindowBorderType::Sizeable:
			style |= WS_THICKFRAME;
		break;

		case Pt::Forms::WindowBorderType::Dialog:
			style |= WS_DLGFRAME;			
		break;

		case Pt::Forms::WindowBorderType::DialogSizeable:
			style |= WS_DLGFRAME;			
			style |= WS_THICKFRAME;
		break;

		case Pt::Forms::WindowBorderType::Tool:
			style |= WS_DLGFRAME;
			exStyle |= WS_EX_TOOLWINDOW;
		break;

		case Pt::Forms::WindowBorderType::ToolSizeable:
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

	bool visible = ((styleVisible & WS_VISIBLE) == WS_VISIBLE);

	if(!_model->Visible.get())
		ShowWindow(_hwnd, SW_HIDE);
	else if( !visible)
		ShowWindow(_hwnd, SW_SHOW);		

	SetWindowLong(_hwnd, GWL_STYLE, style); */ 
}

void ViewImpl::destroy()
{
/*	if( _hwnd == 0)
		return;

	DestroyWindow(_hwnd);
	_hwnd = 0;*/
}

void ViewImpl::output(Pt::Forms::Model* model)
{
	bool firstShow =  (_model == 0);
	_model = dynamic_cast<WindowModel*>(model);

	assert(_model != 0);
	/*
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
	}*/

	setWindowSizeAndPos(firstShow);
	setWindowProperties();	
	setWindowIcon();
	output();
//	InvalidateRect(_hwnd, 0, FALSE);
}

void ViewImpl::output()
{
	Pt::Gfx::Size size = _model->fromUnit(_model->Size.get());

	_rgb88Image.resize(_model->PaintSurface.width(), _model->PaintSurface.height());

	for( size_t x = 0; x < _model->PaintSurface.width(); ++x)
	{
		for(size_t y = 0; y < _model->PaintSurface.height(); ++y)
		{
			const Pt::Gfx::ARgbColor& pixel = _model->PaintSurface.pixel(x,y);

			Pt::Gfx::Rgb888Color color((Pt::uint8_t) pixel.red(), (Pt::uint8_t) pixel.green(), (Pt::uint8_t) pixel.blue());
			_rgb88Image.setColor(x,y,color);
		}
	}
}

Pt::Gfx::Painter* ViewImpl::nativePainter()
{
	return _nativePainter;
}
}}
