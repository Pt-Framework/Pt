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
#include <Pt/Hmi/Application.h>
#include <cassert>

namespace Pt {

namespace Hmi {

MainWindowImpl::MainWindowImpl()
: _hwnd(0)
, _screen( Application::instance().screen() )
{
  HINSTANCE hInstance = GetModuleHandle(NULL);
  _hwnd = CreateWindow( "Pt-Hmi", "", WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 20, 20, 200, 200, GetDesktopWindow(), NULL, hInstance, NULL );
}


MainWindowImpl::~MainWindowImpl()
{
    DestroyWindow( _hwnd);
}


void MainWindowImpl::close()
{
    if( _hwnd == 0)
        return;

    PostMessage(_hwnd, WM_CLOSE, 0, 0);    
}


void MainWindowImpl::show( bool v)
{    
   if( v )
       ShowWindow(_hwnd, SW_SHOW);
   else
       ShowWindow(_hwnd, SW_HIDE);   
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

        case Pt::Hmi::WindowState::Maximized:
            style |= WS_MAXIMIZE;
        break;

        case Pt::Hmi::WindowState::Minimized:
            style |= WS_MINIMIZE;
        break;
    }

  SetWindowLong(_hwnd, GWL_STYLE, style); 
}


void MainWindowImpl::setBorder(bool s)
{
    LONG style = GetWindowLong(_hwnd, GWL_STYLE);
    LONG exStyle = GetWindowLong(_hwnd, GWL_EXSTYLE);
    
    RECT wrect;
    GetWindowRect(_hwnd, &wrect);

    RECT rect;
    GetClientRect(_hwnd, &rect);
    
    if(s)
    {
        style |= WS_BORDER; 
        style |= WS_DLGFRAME;
        style |= WS_THICKFRAME;
    }
    else
    {
        style &= ~WS_BORDER;
        style &= ~WS_DLGFRAME;
        style &= ~WS_THICKFRAME; 
    }
    
    // changing the border also changes the window area
    AdjustWindowRectEx(&rect, style, FALSE, exStyle);

    LONG w = rect.right - rect.left;
    LONG h = rect.bottom - rect.top;
    LONG x = wrect.left;
    LONG y = wrect.top;

    SetWindowLong(_hwnd, GWL_STYLE, style);
    SetWindowPos(_hwnd, 0, x, y, w, h,
                 SWP_FRAMECHANGED|/*SWP_NOMOVE|SWP_NOSIZE|*/SWP_NOZORDER|SWP_NOOWNERZORDER);
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

} // namespace

} // namespace
