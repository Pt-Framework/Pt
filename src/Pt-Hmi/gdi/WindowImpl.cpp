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
#include "WindowImpl.h"
#include "ApplicationImpl.h"
#include <Windows.h>
#include <WindowsX.h>
#include <assert.h>
#include <Pt/Gfx/Rgb888Color.h>
#include <Pt/Gfx/Rgb888Image.h>
#include <Pt/Hmi/Application.h>
#include "PaintSurfaceImpl.h"

namespace Pt{
namespace Hmi{

WindowImpl::WindowImpl(PaintSurface* surface)
: _hwnd(0)
, _app(Pt::Hmi::Application::instance())
, _surface(surface)
, _forceTopMost(false)
{    
  _app.impl()->WindowEvent += Pt::slot(*this, &WindowImpl::onWindowEvent);
  _pointerEvent.buttons().resize(3);    

  create();
}


WindowImpl::~WindowImpl()
{
  destroy();
}


void WindowImpl::create()
{
  if( _hwnd != 0)
    throw std::logic_error("hwnd already created");

  HINSTANCE hInstance = GetModuleHandle(NULL);

  _hwnd = CreateWindow( "Pt-Hmi", "", WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 20, 20, 200, 200, GetDesktopWindow(), NULL, hInstance, NULL );
  BringWindowToTop(_hwnd);
  ShowWindow(_hwnd, SW_HIDE);    
  UpdateWindow(_hwnd);    
}


void WindowImpl::destroy()
{
  if( _hwnd == 0)
      return;

  DestroyWindow(_hwnd);
  _hwnd = 0;
}


void WindowImpl::show()
{
  ShowWindow(_hwnd, SW_SHOW);
}


void WindowImpl::hide()
{
  ShowWindow(_hwnd, SW_HIDE);
}


void WindowImpl::onKey(unsigned int msg,  WPARAM wparam, LPARAM lparam)
{
  BYTE keyboardState[256];

  GetKeyboardState(keyboardState);

  if(msg == WM_KEYDOWN)
      _keyEvent.setState(KeyEvent::KeyDown);
  else if(msg == WM_KEYUP)
      _keyEvent.setState(KeyEvent::KeyUp);            
    
  if(msg == WM_SYSCOMMAND)
  {
    _keyEvent.setState(KeyEvent::KeyUp);
    _keyEvent.setAlt(wparam == SC_KEYMENU );
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
        _keyEvent.setAlt(false);
        _keyEvent.setShift(_keyEvent.state() == KeyEvent::KeyDown);
      }
      else if(wparam == 17 )
      {//Controll key
        _keyEvent.setAlt(false);
        _keyEvent.setCtrl(_keyEvent.state() == KeyEvent::KeyDown);
      }

      Pt::uint32_t scanCode = ((lparam >> 16) & 0xFF);            
      Pt::uint32_t ucode = 0;            
        
      ToUnicode( wparam, scanCode , (BYTE*)keyboardState, (LPWSTR)&ucode, 4, 0);    
      _keyEvent.setUnicode(ucode);
  }

  _windowEvent.send(_keyEvent);
}

void WindowImpl::onWindowEvent(HWND wnd, unsigned int message, unsigned int wparam, long lparam, bool& handled)
{
    if(_hwnd != wnd)
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
            handled = true;
        }
        break;

        case WM_CLOSE:
        {
            onClosing();    
            handled = true;            
        }
        break;

        case WM_KILLFOCUS:
        {
					if( _forceTopMost )
						BringWindowToTop( _hwnd );
					else
						_activateEvent.setActive(false);
        }
        break;        

				case WM_SETFOCUS:
				{
					_activateEvent.setActive(true);
					_windowEvent.send(_activateEvent);
				}
				break;
    }
}


void WindowImpl::onClosing()
{
  CloseEvent closeEvent;
  _windowEvent.send(closeEvent);
}


void WindowImpl::onSize(WPARAM wParam, LPARAM lParam)
{
  WindowState::Type state = WindowState::Normal;

  switch(wParam)
  {
      case SIZE_MAXHIDE:
      case SIZE_MAXSHOW:
      break;

      case SIZE_MAXIMIZED:
          state = WindowState::Maximazed;                            
      break;

      case SIZE_MINIMIZED:
          state = WindowState::Minimized;            
      break;
 
      case SIZE_RESTORED:
          state = WindowState::Normal;
      break;
  }
 
  int width  = LOWORD(lParam);
  int height = HIWORD(lParam);  
    
  Pt::Gfx::SizeF winSize(width, height);
  _resizeEvent.setSize(winSize);
  _resizeEvent.setState(state);
  _windowEvent.send(_resizeEvent);
}


void WindowImpl::onMouse(unsigned int msg, WPARAM wparam, LPARAM lparam)
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
  
     Pt::Gfx::PointF p = _app.toUnit(Pt::Gfx::Point(xPos, yPos));
    _pointerEvent.setX(p.x());
    _pointerEvent.setY(p.y());            

    _windowEvent.send(_pointerEvent);
}


void WindowImpl::onMove()
{
    RECT  info;
    GetWindowRect(_hwnd, &info);
    Pt::Gfx::PointF winPos(info.left, info.right);
    _positionEvent.setPosition(winPos);
		_windowEvent.send( _positionEvent );
}


void WindowImpl::onPaint()
{           
    RECT  info;
    GetWindowRect(_hwnd, &info);

    PAINTSTRUCT ps;
    HDC windowContext = BeginPaint(_hwnd, &ps);
    
    HDC bitmapDeviceConText = _surface->impl()->deviceContext();
    BitBlt(windowContext, 0, 0, info.right - info.left,  info.bottom - info.top, bitmapDeviceConText, 0, 0, SRCCOPY);    
    
    EndPaint(_hwnd, &ps);    
}


void WindowImpl::setPosition(const Gfx::PointF& pf)
{
  Gfx::Point p = _app.fromUnit(pf);
  SetWindowPos(_hwnd, 0, p.x(), p.y(), 0, 0, SWP_DRAWFRAME|SWP_NOSIZE);
}


void WindowImpl::setSize(const Gfx::SizeF& sizef)
{
  Gfx::Size size = _app.fromUnit(sizef);

  RECT clientRect;
  SetRect(&clientRect, 0, 0, size.width() - 1, size.height() - 1);

  LONG style = GetWindowLong(_hwnd, GWL_STYLE);
	LONG exStyle = GetWindowLong(_hwnd, GWL_EXSTYLE);

  AdjustWindowRectEx(&clientRect, style, false, exStyle);
    
  LONG clientWidth  = clientRect.right  - clientRect.left + 1;
  LONG clientHeight = clientRect.bottom - clientRect.top  + 1;
  SetWindowPos(_hwnd, NULL, 0, 0, clientWidth, clientHeight, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
}


void WindowImpl::showTitle(bool p)
{
  LONG style = GetWindowLong(_hwnd, GWL_STYLE);
    
    if( p)
        style |= WS_CAPTION;
  else
    style &= ~WS_CAPTION;

  SetWindowLong(_hwnd, GWL_STYLE, style); 
}


void WindowImpl::setCaption(const std::string& text)
{
  SetWindowText(_hwnd, text.c_str());
}


void WindowImpl::showMinimizedButton(bool p)
{
  LONG style = GetWindowLong(_hwnd, GWL_STYLE);

    if(p)
        style |= WS_MINIMIZEBOX;
  else
    style &= ~WS_MINIMIZEBOX;

  SetWindowLong(_hwnd, GWL_STYLE, style); 
}


void WindowImpl::showMaximizeButton(bool p)
{
  LONG style = GetWindowLong(_hwnd, GWL_STYLE);

    if(p)
        style |= WS_MAXIMIZEBOX;
  else
    style &= ~WS_MAXIMIZEBOX;

  SetWindowLong(_hwnd, GWL_STYLE, style); 
}


void WindowImpl::showSysMenu(bool p)
{
  LONG style = GetWindowLong(_hwnd, GWL_STYLE);

    if(p)
        style |= WS_SYSMENU;
  else
    style &= ~WS_SYSMENU;

  SetWindowLong(_hwnd, GWL_STYLE, style); 
}


void WindowImpl::setForceTopMost(bool force)
{
	_forceTopMost = force;

	if( _forceTopMost )
	{
		BringWindowToTop(_hwnd);		
	}
}


void WindowImpl::setWindowState(WindowState::Type p)
{
    LONG style = GetWindowLong(_hwnd, GWL_STYLE);

    switch(p)
    {
        case Pt::Hmi::WindowState::Normal:            
        break;

        case Pt::Hmi::WindowState::Maximazed:
            style |= WS_MAXIMIZE;
        break;

        case Pt::Hmi::WindowState::Minimized:
            style |= WS_MINIMIZE;
        break;
    }

  SetWindowLong(_hwnd, GWL_STYLE, style); 
}


void WindowImpl::setBorder(WindowBorder::Type p)
{
    LONG style = GetWindowLong(_hwnd, GWL_STYLE);
		LONG exStyle = GetWindowLong(_hwnd, GWL_EXSTYLE);

    switch( p)
    {
        case Pt::Hmi::WindowBorder::NoBorder:            
						style &= ~WS_DLGFRAME;
						style &= ~WS_THICKFRAME; 
						exStyle &= ~WS_EX_TOOLWINDOW;						
        break;

        case Pt::Hmi::WindowBorder::Sizeable:
            style |= WS_THICKFRAME;
						style &= ~WS_DLGFRAME;
        break;

        case Pt::Hmi::WindowBorder::Dialog:
            style |= WS_DLGFRAME;   
						style &= ~WS_THICKFRAME;         
        break;

        case Pt::Hmi::WindowBorder::DialogSizeable:
            style |= WS_DLGFRAME;            
            style |= WS_THICKFRAME;
        break;

        case Pt::Hmi::WindowBorder::Tool:
            style |= WS_DLGFRAME;
						style &= ~WS_THICKFRAME; 
            exStyle |= WS_EX_TOOLWINDOW;
        break;

        case Pt::Hmi::WindowBorder::ToolSizeable:
						style &= ~WS_DLGFRAME;
            style |= WS_THICKFRAME;
            exStyle |= WS_EX_TOOLWINDOW;
        break;

        default:
            style |= WS_BORDER; 
						style &= ~WS_THICKFRAME;
        break;
    }

  SetWindowLong(_hwnd, GWL_STYLE, style);
  SetWindowLong(_hwnd, GWL_EXSTYLE, exStyle);
}


void WindowImpl::showInTaskbar(bool p)
{	
  LONG style = GetWindowLong(_hwnd, GWL_EXSTYLE);

  if(p)
		style |= WS_EX_APPWINDOW;  
  else
    style &= ~WS_EX_APPWINDOW; 
  	
  SetWindowLong(_hwnd, GWL_EXSTYLE, style);	
}

void WindowImpl::setEnable(bool e)
{
	EnableWindow(_hwnd, e);
}

void WindowImpl::setIcon(const Pt::Gfx::ARgbImage& icon)
{
    if(icon.width() == 0 || icon.height() == 0)
        return;

    HINSTANCE hInstance = GetModuleHandle(NULL);
    const size_t planes = 4;
    std::vector<Pt::uint8_t> bitmapBuffer(icon.width() * icon.height() * planes);
        
    for(size_t y = 0; y < icon.height(); ++y)
    {
        const size_t offsetLine = y * (icon.width()*planes);

        for(size_t x = 0; x < icon.width(); ++x)
        {
          const size_t index  = offsetLine + (x*planes);

          const Pt::Gfx::ARgbColor& pix =  icon.pixel(x,y);
                
          bitmapBuffer[index]     = static_cast<unsigned char>(pix.blue());    
          bitmapBuffer[index + 1] = static_cast<unsigned char>(pix.green());
          bitmapBuffer[index + 2] = static_cast<unsigned char>(pix.red());
          bitmapBuffer[index + 3] = static_cast<unsigned char>(pix.alpha());
        }        
    }

    HICON hIcon = ::CreateIcon(hInstance, icon.width(), icon.height(), 4, 8, 0, (BYTE*)&bitmapBuffer[0]);
    SetClassLong(_hwnd, GCL_HICON, (LONG)hIcon);     
}


void WindowImpl::render()
{
	InvalidateRect(_hwnd, NULL, FALSE);
}

}}
