/* Copyright (C) 2015 Marc Boris Duerner 
   Copyright (C) 2015 Laurentiu-Gheorghe Crisan
   
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.
   
   As a special exception, you may use this file as part of a free
   software library without restriction. Specifically, if other files
   instantiate templates or use macros or inline functions from this
   file, or you compile this file and link it with other files to
   produce an executable, this file does not by itself cause the
   resulting executable to be covered by the GNU General Public
   License. This exception does not however invalidate any other
   reasons why the executable file might be covered by the GNU Library
   General Public License.
   
   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.
   
   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
   MA 02110-1301 USA
*/

#include "MainWindowImpl.h"
#include "ApplicationImpl.h"
#include <Windows.h>
#include <WindowsX.h>
#include <assert.h>
#include <Pt/Gfx/Color.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/Window.h>
#include <Pt/Hmi/ResizeEvent.h>
#include <Pt/Hmi/MoveEvent.h>
#include <Pt/Hmi/CloseEvent.h>
#include <Pt/Hmi/ActivateEvent.h>
#include <Pt/Hmi/Screen.h>
#include "PaintSurfaceImpl.h"

namespace Pt{
namespace Hmi{

MainWindowImpl::MainWindowImpl(Window* w)
: WindowImpl(w)
, _app( Pt::Hmi::Application::instance() )
, _screen( _app.mainScreen() )
, _hwnd( 0 )
, _hasPointer(false)
{    
    create();
}


MainWindowImpl::~MainWindowImpl()
{
  destroy();
}


void MainWindowImpl::create()
{
  if( _hwnd != 0 )
    throw std::logic_error("hwnd already created");

  HINSTANCE hInstance = GetModuleHandle(NULL);

  HWND hwnd = CreateWindow( "Pt-Hmi", "", WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 20, 20, 200, 200, GetDesktopWindow(), NULL, hInstance, NULL );
  BringWindowToTop( hwnd );
  ShowWindow( hwnd, SW_HIDE );    
  UpdateWindow( hwnd );
  _hwnd = hwnd;
}


void MainWindowImpl::destroy()
{
  if( _hwnd == 0)
      return;

  DestroyWindow(_hwnd);
  _hwnd = 0;
}


void MainWindowImpl::show()
{
    if( _hwnd == 0)
        create();

   ShowWindow(_hwnd, SW_SHOW);
}


void MainWindowImpl::hide()
{
  ShowWindow(_hwnd, SW_HIDE);
}


void MainWindowImpl::onKey(unsigned int msg,  WPARAM wparam, LPARAM lparam)
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
    _keyEvent.key().setAlt(wparam == SC_KEYMENU );
    _keyEvent.key().setShift(false);
    _keyEvent.key().setCtrl(false);
    _keyEvent.key().setUnicode(lparam);
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
        _keyEvent.key().setAlt(false);
        _keyEvent.key().setShift(_keyEvent.state() == KeyEvent::KeyDown);
      }
      else if(wparam == 17 )
      {//Controll key
        _keyEvent.key().setAlt(false);
        _keyEvent.key().setCtrl(_keyEvent.state() == KeyEvent::KeyDown);
      }

      Pt::uint32_t scanCode = ((lparam >> 16) & 0xFF);            
      Pt::uint32_t ucode = 0;            
        
      ToUnicode( wparam, scanCode , (BYTE*)keyboardState, (LPWSTR)&ucode, 4, 0);    
      _keyEvent.key().setUnicode(ucode);
  }

  _app.sendEvent(*_apiWindow, _keyEvent);
}


bool MainWindowImpl::processEvent(unsigned int message, WPARAM wparam, LPARAM lparam )
{
    bool handled = false;

    switch( message )
    {
        case WM_MOUSEMOVE:
        {
          TRACKMOUSEEVENT tme ={ sizeof(TRACKMOUSEEVENT), TME_LEAVE, _hwnd, 0 };
          TrackMouseEvent(&tme);              
          onMouse(message, wparam, lparam);
          handled = true;
        }
        break;

        case WM_LBUTTONDOWN:        
        case WM_MBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_LBUTTONUP:        
        case WM_MBUTTONUP:
        case WM_RBUTTONUP:        
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
            onMove(lparam);
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
            onClose();
        }
        break;

        case WM_ACTIVATE:
        {
            onActivate( wparam != 0 );
        }
        break;
                
        case WM_KILLFOCUS:
        {
            onActivate( false );
        }
        break;        

        case WM_GETMINMAXINFO:
        {
            MINMAXINFO* mmi = (MINMAXINFO*)lparam;
            POINT min = { _apiWindow->minimumSize().width(), _apiWindow->minimumSize().height() };
            POINT max = { _apiWindow->maximumSize().width(), _apiWindow->maximumSize().height() };
            mmi->ptMaxTrackSize = max; 
            mmi->ptMinTrackSize = min;
            handled = true;  
        }
        break;

        case WM_MOUSELEAVE:
            handled = true;  
            _hasPointer = false;
            _app.mainScreen().setCursor(0);
            _app.mainScreen().setPointerWindow(0);
        break;
        
    }

    return handled;
}

void MainWindowImpl::onClose()
{
    CloseEvent ev;
    _apiWindow->processEvent(ev);
}


void MainWindowImpl::onActivate(bool f)
{
    _apiWindow->processEvent( ActivateEvent(f) );
}


void MainWindowImpl::onSize(WPARAM wParam, LPARAM lParam)
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
    
    Pt::Hmi::ResizeEvent ev(Gfx::SizeF(width, height), state);
    _apiWindow->processEvent(ev);
}


void MainWindowImpl::onMouse(unsigned int msg, WPARAM wparam, LPARAM lparam)
{    
    int xPos = GET_X_LPARAM(lparam); 
    int yPos = GET_Y_LPARAM(lparam); 

    switch(msg)
    {
        case WM_LBUTTONDOWN:
            _pointerEvent.setPress(0);
        break;
        
        case WM_LBUTTONUP:        
            _pointerEvent.setRelease(0);
        break;
                            
        case WM_MBUTTONDOWN:
            _pointerEvent.setPress(1);
        break;
        
        case WM_MBUTTONUP:
            _pointerEvent.setRelease(1);
        break;   
        
        case WM_RBUTTONDOWN:        
            _pointerEvent.setPress(2);
        break;
        
        case WM_RBUTTONUP:
            _pointerEvent.setRelease(2);
        break; 

        case WM_MOUSEMOVE:
            _pointerEvent.setMove();
        break;
    }
  
    Gfx::PointF p = _screen.toUnit( Gfx::Point(xPos, yPos) );
    _pointerEvent.setX( p.x() );
    _pointerEvent.setY( p.y() );            

    if( ! _hasPointer )
    {
        _hasPointer = true;
        _app.mainScreen().setPointerWindow( _apiWindow );
    }

    _app.sendEvent(*_apiWindow, _pointerEvent);
}


void MainWindowImpl::onMove(LPARAM lParam)
{ 
    RECT  info;
  GetWindowRect(_hwnd, &info);
    int xPos = info.left;
    int yPos = info.top;

    MoveEvent ev(Gfx::PointF(xPos, yPos) );
  _apiWindow->processEvent( ev );    
}


void MainWindowImpl::onPaint()
{           
      RECT  info;
      GetClientRect(_hwnd, &info);

      PAINTSTRUCT ps;
      HDC windowContext = BeginPaint(_hwnd, &ps);
    
      HDC bitmapDeviceConText = _apiWindow->surface().impl()->deviceContext();
      BitBlt(windowContext, 0, 0, info.right,  info.bottom, bitmapDeviceConText, 0, 0, SRCCOPY);    
    
      EndPaint(_hwnd, &ps);    
}


void MainWindowImpl::setPosition(const Gfx::PointF& pf)
{
  Gfx::Point p = _screen.fromUnit(pf);
  SetWindowPos(_hwnd, 0, p.x(), p.y(), 0, 0, SWP_DRAWFRAME|SWP_NOSIZE);
}

void MainWindowImpl::activate()
{
  SetActiveWindow( _hwnd );
  BringWindowToTop( _hwnd );
}


void MainWindowImpl::setSize(const Gfx::SizeF& sizef)
{
    if( _hwnd == 0)
        return;

  Gfx::Size size = _screen.fromUnit(sizef);

  RECT clientRect;
  SetRect(&clientRect, 0, 0, size.width() - 1, size.height() - 1);

  LONG style = GetWindowLong(_hwnd, GWL_STYLE);
    LONG exStyle = GetWindowLong(_hwnd, GWL_EXSTYLE);

  AdjustWindowRectEx(&clientRect, style, false, exStyle);
    
  LONG clientWidth  = clientRect.right  - clientRect.left + 1;
  LONG clientHeight = clientRect.bottom - clientRect.top  + 1;
  SetWindowPos(_hwnd, NULL, 0, 0, clientWidth, clientHeight, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
}


void MainWindowImpl::setDecoration( WindowDecoration::Flags deco )
{
    setShowTitle( (deco & WindowDecoration::Flags::ShowTitleBar) != 0);
    setShowMinimizeButton( (deco & WindowDecoration::Flags::ShowMinimizeButton) != 0);
    setShowMaximizeButton( (deco & WindowDecoration::Flags::ShowMaximizeButton) != 0);
    setShowSystemMenu( (deco & WindowDecoration::Flags::ShowIcon) != 0);
}

void MainWindowImpl::setShowTitle(bool p)
{
  LONG style = GetWindowLong(_hwnd, GWL_STYLE);
    
    if( p)
        style |= WS_CAPTION;
  else
    style &= ~WS_CAPTION;

  SetWindowLong(_hwnd, GWL_STYLE, style); 
}


void MainWindowImpl::setTitle(const std::string& text)
{
  SetWindowText(_hwnd, text.c_str());
}


void MainWindowImpl::setShowMinimizeButton(bool p)
{
  LONG style = GetWindowLong(_hwnd, GWL_STYLE);

    if(p)
        style |= WS_MINIMIZEBOX;
  else
    style &= ~WS_MINIMIZEBOX;

  SetWindowLong(_hwnd, GWL_STYLE, style); 
}


void MainWindowImpl::setShowMaximizeButton(bool p)
{
  LONG style = GetWindowLong(_hwnd, GWL_STYLE);

    if(p)
        style |= WS_MAXIMIZEBOX;
  else
    style &= ~WS_MAXIMIZEBOX;

  SetWindowLong(_hwnd, GWL_STYLE, style); 
}


void MainWindowImpl::setShowSystemMenu(bool p)
{
  LONG style = GetWindowLong(_hwnd, GWL_STYLE);

    if(p)
        style |= WS_SYSMENU;
  else
    style &= ~WS_SYSMENU;

  SetWindowLong(_hwnd, GWL_STYLE, style); 
}


void MainWindowImpl::setState(WindowState::Type p)
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


void MainWindowImpl::setBorder(WindowBorder::Type p)
{
    LONG style = GetWindowLong(_hwnd, GWL_STYLE);
        LONG exStyle = GetWindowLong(_hwnd, GWL_EXSTYLE);

    switch( p )
    {
        case Pt::Hmi::WindowBorder::NoBorder:            
                        style &= ~WS_DLGFRAME;
                        style &= ~WS_THICKFRAME; 
                        exStyle &= ~WS_EX_TOOLWINDOW;                        
        break;

        case Pt::Hmi::WindowBorder::Sizeable:
            style |= WS_THICKFRAME;
                        style |= WS_DLGFRAME;
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


void MainWindowImpl::setMinimumSize(const Gfx::SizeF& s)
{
    
}
    
void MainWindowImpl::setMaximumSize(const Gfx::SizeF& s)
{
    
}

void MainWindowImpl::setEnabled(bool e)
{
    EnableWindow(_hwnd, e);
}


void MainWindowImpl::setIcon(const Gfx::Image& icon)
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

          const Gfx::Color& pix =  icon.color(x,y);
                
          bitmapBuffer[index]     = static_cast<unsigned char>(pix.blue());    
          bitmapBuffer[index + 1] = static_cast<unsigned char>(pix.green());
          bitmapBuffer[index + 2] = static_cast<unsigned char>(pix.red());
          bitmapBuffer[index + 3] = static_cast<unsigned char>(pix.alpha());
        }        
    }

    HICON hIcon = ::CreateIcon(hInstance, icon.width(), icon.height(), 4, 8, 0, (BYTE*)&bitmapBuffer[0]);
    SetClassLong(_hwnd, GCL_HICON, (LONG)hIcon);     
}


void MainWindowImpl::invalidate()
{
   _apiWindow->render();
    InvalidateRect(_hwnd, NULL, FALSE);
}

void MainWindowImpl::close()
{
    destroy();
}

}} // namespace
