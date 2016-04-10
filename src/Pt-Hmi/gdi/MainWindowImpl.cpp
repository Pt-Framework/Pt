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
#include "PaintSurfaceImpl.h"
#include <Pt/Gfx/Color.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/Window.h>
#include <Pt/Hmi/ResizeEvent.h>
#include <Pt/Hmi/MoveEvent.h>
#include <Pt/Hmi/CloseEvent.h>
#include <Pt/Hmi/ActivateEvent.h>
#include <Pt/Hmi/PaintEvent.h>
#include <Pt/Hmi/Screen.h>
#include <cassert>
#include <Windows.h>
#include <WindowsX.h>

namespace Pt {

namespace Hmi {

MainWindowImpl::MainWindowImpl(Window* w)
: WindowImpl(w)
, _app( Pt::Hmi::Application::instance() )
, _screen( _app.screen() )
, _hwnd(0)
, _mouseEvent(0)
, _hasPointer(false)
{    
    create();
}


MainWindowImpl::~MainWindowImpl()
{
    Application::instance().screen().unregisterWindow(*_apiWindow);
    destroy();
}


void MainWindowImpl::create()
{
  if( _hwnd != 0 )
    throw std::logic_error("hwnd already created");

  HINSTANCE hInstance = GetModuleHandle(NULL);

  _hwnd = CreateWindow( "Pt-Hmi", "", WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 20, 20, 200, 200, GetDesktopWindow(), NULL, hInstance, NULL );
}


void MainWindowImpl::destroy()
{
  if( _hwnd == 0)
      return;

  DestroyWindow(_hwnd);
  _hwnd = 0;
}


void MainWindowImpl::show( bool v)
{    
    _apiWindow->onShowEvent( ShowEvent(_apiWindow->vid(), v ) );

   if( v )
       ShowWindow(_hwnd, SW_SHOW);
   else
       ShowWindow(_hwnd, SW_HIDE);
    
}

void MainWindowImpl::update(const Gfx::RectF& rect)
{
   Application::instance().screen().onUpdate(*_apiWindow, rect);
}

void MainWindowImpl::activate()
{
    SetActiveWindow( _hwnd );
}


void MainWindowImpl::enable(bool e)
{
    EnableWindow(_hwnd, e);
}


void MainWindowImpl::move(const Gfx::PointF& pos)
{
    Gfx::Point p = _screen.fromUnit(pos);
    SetWindowPos(_hwnd, 0, p.x(), p.y(), 0, 0, 
                 SWP_DRAWFRAME|SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOZORDER);
}


void MainWindowImpl::resize(const Gfx::SizeF& s)
{
    if( _hwnd == 0)
        return;

    Gfx::Size size = _screen.fromUnit(s);

    RECT clientRect;
    SetRect(&clientRect, 0, 0, size.width() - 1, size.height() - 1);

    LONG style = GetWindowLong(_hwnd, GWL_STYLE);
    LONG exStyle = GetWindowLong(_hwnd, GWL_EXSTYLE);

    AdjustWindowRectEx(&clientRect, style, FALSE, exStyle);
    
    LONG clientWidth  = clientRect.right  - clientRect.left + 1;
    LONG clientHeight = clientRect.bottom - clientRect.top  + 1;
    SetWindowPos(_hwnd, NULL, 0, 0, clientWidth, clientHeight, 
                 SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE);
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


void MainWindowImpl::paint(const Gfx::RectF& rect)
{
    InvalidateRect(_hwnd, NULL, FALSE);
}


void MainWindowImpl::close()
{
    destroy();
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

        case WM_MOUSEWHEEL:
        {
            int delta = GET_WHEEL_DELTA_WPARAM(wparam);
            _scrollEvent.set(0, delta/120.0);
            _app.sendEvent(*_apiWindow, _scrollEvent);
            handled = true;
        }
        break;

        case WM_KEYDOWN:
        case WM_KEYUP:    
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:    
        {
            UINT vkey = wparam;
            UINT scanCode = ((lparam >> 16) & 0xFF);
            bool isPress = (message == WM_KEYDOWN || message == WM_SYSKEYDOWN);
            //UINT repeatCount = lparam & 0xFFFF;

            onKey(vkey, scanCode, isPress);
            handled = true;
            break;
        }


        case WM_PAINT:
        {
            onPaint();
            handled = true;
        }
        break;
        
        case WM_SIZE:
        {
            onResize(wparam, lparam);
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
                
        case WM_ENABLE:
              onEnable(true);
        break;

        case WS_DISABLED:
            onEnable(false);
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
            _mouseEvent.clear();
            _app.screen().setCursor(0);
            _app.screen().setPointerWindow(0);
        break;
        
    }

    return handled;
}


void MainWindowImpl::onClose()
{
    CloseEvent ev;
    _apiWindow->processEvent(ev);
}


void MainWindowImpl::onActivate(bool a)
{
    _apiWindow->processEvent( ActivateEvent(_apiWindow->vid(), a) );
}


void MainWindowImpl::onEnable(bool e)
{
    _apiWindow->processEvent( EnableEvent(_apiWindow->vid(), e) );
}


void MainWindowImpl::onKey(UINT vkey, UINT scanCode, bool isPress)
{
    BYTE keyboardState[256];
    GetKeyboardState(keyboardState);

    wchar_t wc = 0;
    ToUnicode(vkey, scanCode, (BYTE*)keyboardState, &wc, 1, 0);    

    bool shift = (keyboardState[VK_SHIFT] & 0x80) == 0x80;
    bool control = (keyboardState[VK_CONTROL] & 0x80) == 0x80;
    bool alt = (keyboardState[VK_MENU] & 0x80) == 0x80;
    bool rwin = (keyboardState[VK_RWIN] & 0x80) == 0x80;
    bool lwin = (keyboardState[VK_LWIN] & 0x80) == 0x80;

    Key::Modifiers modifiers;
    if(shift)
        modifiers |= Key::Shift;

    if(control)
        modifiers |= Key::Control;

    if(alt)
        modifiers |= Key::Alt;

    if(rwin || lwin)
        modifiers |= Key::Meta;

    Key::Code keyCode = static_cast<Key::Code>(vkey);
    Key key(modifiers, keyCode);

    if(isPress)
        _keyEvent.setPress(key, wc);
    else
        _keyEvent.setRelease(key, wc); 

    _app.loop().commitEvent(_keyEvent);
}


void MainWindowImpl::onMouse(unsigned int msg, WPARAM wparam, LPARAM lparam)
{    
    int xPos = GET_X_LPARAM(lparam); 
    int yPos = GET_Y_LPARAM(lparam); 

    switch(msg)
    {
        case WM_LBUTTONDOWN:
            _mouseEvent.setPress(MouseEvent::Left);
        break;
        
        case WM_LBUTTONUP:        
            _mouseEvent.setRelease(MouseEvent::Left);
        break;
                            
        case WM_MBUTTONDOWN:
            _mouseEvent.setPress(MouseEvent::Middle);
        break;
        
        case WM_MBUTTONUP:
            _mouseEvent.setRelease(MouseEvent::Middle);
        break;   
        
        case WM_RBUTTONDOWN:        
            _mouseEvent.setPress(MouseEvent::Right);
        break;
        
        case WM_RBUTTONUP:
            _mouseEvent.setRelease(MouseEvent::Right);
        break; 

        case WM_MOUSEMOVE:
            _mouseEvent.setMove();
        break;
    }
  
    Gfx::PointF p = _screen.toUnit( Gfx::Point(xPos, yPos) );
    _mouseEvent.setX( p.x() );
    _mouseEvent.setY( p.y() );            

    if( ! _hasPointer )
    {
        _hasPointer = true;
        _app.screen().setPointerWindow( _apiWindow );
    }

    _mouseEvent.setId( _apiWindow->vid() );
    _app.loop().commitEvent(_mouseEvent);
}


void MainWindowImpl::onMove(LPARAM lParam)
{ 
    RECT  info;
    GetWindowRect(_hwnd, &info);
    int xPos = info.left;
    int yPos = info.top;

    MoveEvent ev(_apiWindow->vid(), Gfx::PointF(xPos, yPos) );
   _apiWindow->processEvent( ev );          
}


void MainWindowImpl::onResize(WPARAM wParam, LPARAM lParam)
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
 
        default:
        case SIZE_RESTORED:
            state = WindowState::Normal;
            break;
    }   

    int width  = LOWORD(lParam);
    int height = HIWORD(lParam); 

    Gfx::SizeF to(width, height);
          
    ResizeEvent rev(_apiWindow->vid(), to);
    _app.loop().commitEvent(rev);
           
    Gfx::RectF updateRect(Gfx::PointF(0,0), to);
    _apiWindow->update(updateRect);
            
    // windows starts a nested message loop during resizing, so the events
    // required for painting need to be processed manually
    _app.impl()->processEvents();
}


void MainWindowImpl::onPaint()
{
    RECT info;
    GetClientRect(_hwnd, &info);      
      
    PAINTSTRUCT ps;
    HDC windowContext = BeginPaint(_hwnd, &ps);

    // TODO: rect optimization
    Gfx::RectF updateRect(Gfx::PointF(0,0), _apiWindow->size());

    HDC bitmapContext = _apiWindow->surface().pixmapImpl()->deviceContext();
    BitBlt(windowContext, 0, 0, info.right, info.bottom, bitmapContext, 0, 0, SRCCOPY);    
    
    EndPaint(_hwnd, &ps);    
}

} // namespace

} // namespace
