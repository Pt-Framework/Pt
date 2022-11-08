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
#include <Pt/Math.h>
#include <cassert>

namespace Pt {

namespace Hmi {

MainWindowImpl::MainWindowImpl(Window::Type type)
: _hwnd(0)
, _scalingFactor(1.0)
{
    HINSTANCE hInstance = GetModuleHandle(NULL);
  
    LONG style = 0;
    LONG exStyle = 0;
    
    switch(type)
    {
        case WindowType::Popup:
            style = WS_POPUP;
            exStyle = WS_EX_NOACTIVATE;
            break;

        default:
        case WindowType::Default:
            style = WS_OVERLAPPEDWINDOW;
            exStyle = WS_EX_APPWINDOW;
            break;
    }

    _hwnd = CreateWindowEx(exStyle, "Pt-Hmi", "", style,
                           0, 0, 10, 10, GetDesktopWindow(), 
                           NULL, hInstance, NULL);

    HDC screen = GetDC(_hwnd);

    int dpix = GetDeviceCaps(screen, LOGPIXELSX);

    //std::clog << "SCALING DPI: " << dpix << std::endl;
    //std::clog << "SCALING: " << dpix / 96.0 << std::endl;

    _scalingFactor = dpix / 96.0;
    ReleaseDC(_hwnd, screen);
}


MainWindowImpl::~MainWindowImpl()
{
    DestroyWindow( _hwnd);
}


void MainWindowImpl::setType(WindowType type)
{
    LONG style = GetWindowLong(_hwnd, GWL_STYLE);
    LONG exStyle = GetWindowLong(_hwnd, GWL_EXSTYLE);
    
    RECT wrect;
    GetWindowRect(_hwnd, &wrect);

    RECT rect;
    GetClientRect(_hwnd, &rect);

    switch(type)
    {
        case WindowType::Popup:
            style = WS_POPUP;
            exStyle = WS_EX_NOACTIVATE;
            break;

        default:
        case WindowType::Default:
            style = WS_OVERLAPPEDWINDOW;
            exStyle = WS_EX_APPWINDOW;
            break;
    }

    // changing the border also changes the window area
    AdjustWindowRectEx(&rect, style, FALSE, exStyle);

    LONG w = rect.right - rect.left;
    LONG h = rect.bottom - rect.top;
    LONG x = wrect.left;
    LONG y = wrect.top;

    SetWindowLong(_hwnd, GWL_STYLE, style);
    SetWindowLong(_hwnd, GWL_EXSTYLE, exStyle);
    
    SetWindowPos(_hwnd, 0, x, y, w, h,
                 SWP_FRAMECHANGED|
                 SWP_NOZORDER|SWP_NOOWNERZORDER);
}


double MainWindowImpl::scaleFactor() const
{
    return _scalingFactor;
}


Gfx::PointF MainWindowImpl::toScreen(const Gfx::PointF& windowPos) const
{
    POINT p = { lround(windowPos.x()), 
                lround(windowPos.y()) };

    ClientToScreen(_hwnd, &p);

    return Gfx::PointF( p.x, p.y );
}


Gfx::PointF MainWindowImpl::fromScreen(const Gfx::PointF& screenPos) const
{
    POINT p = { lround(screenPos.x()), 
                lround(screenPos.y()) };

    ScreenToClient(_hwnd, &p);

    return Gfx::PointF( p.x, p.y );
}


void MainWindowImpl::paint(const Gfx::RectF& rect)
{
    RECT wRect;
    wRect.bottom = lround( rect.bottom() );
    wRect.top    = lround( rect.top() );
    wRect.left   = lround( rect.left() );
    wRect.right  = lround( rect.right() );

    InvalidateRect(_hwnd, &wRect, FALSE);
}


void MainWindowImpl::show(bool v)
{
    if(v)
    {
        LONG style = GetWindowLong(_hwnd, GWL_EXSTYLE);

        if(style & WS_EX_NOACTIVATE)
          ShowWindow(_hwnd, SW_SHOWNOACTIVATE);
        else
          ShowWindow(_hwnd, SW_SHOW);
    }
    else
    {
        ShowWindow(_hwnd, SW_HIDE);
    }
}


void MainWindowImpl::activate()
{
    SetActiveWindow(_hwnd);
}


void MainWindowImpl::enable(bool e)
{
    EnableWindow(_hwnd, e);
}


void MainWindowImpl::move(const Gfx::PointF& p)
{
    SetWindowPos(_hwnd, 0, p.x(), p.y(), 0, 0, 
                 SWP_DRAWFRAME|SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOZORDER);
}


void MainWindowImpl::resize(const Gfx::SizeF& size)
{
    RECT clientRect;
    SetRect(&clientRect, 0, 0, lround(size.width()) - 1, 
                               lround(size.height()) - 1);

    LONG style = GetWindowLong(_hwnd, GWL_STYLE);
    LONG exStyle = GetWindowLong(_hwnd, GWL_EXSTYLE);

    AdjustWindowRectEx(&clientRect, style, FALSE, exStyle);
    
    LONG clientWidth  = clientRect.right  - clientRect.left + 1;
    LONG clientHeight = clientRect.bottom - clientRect.top  + 1;
    SetWindowPos(_hwnd, NULL, 0, 0, clientWidth, clientHeight, 
                 SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE);
}


void MainWindowImpl::setTitle(const std::string& text)
{
    SetWindowText(_hwnd, text.c_str());
}


void MainWindowImpl::setIcon(const Gfx::Image& icon)
{
    if(icon.width() == 0 || icon.height() == 0)
    {
        SendMessage(_hwnd, WM_SETICON, ICON_SMALL, 0);
        return;
    }

    const size_t planes = 4;
    std::vector<Pt::uint8_t> bitmapBuffer(icon.width() * icon.height() * planes);
        
    for(size_t y = 0; y < icon.height(); ++y)
    {
        const size_t offsetLine = y * (icon.width() * planes);

        for(size_t x = 0; x < icon.width(); ++x)
        {
          const size_t index = offsetLine + (x*planes);

          Gfx::ConstPixel pixel(icon.view(), x, y);
          Gfx::Color color = icon.format().getColor(pixel);
                
          bitmapBuffer[index]     = static_cast<unsigned char>(color.blue());    
          bitmapBuffer[index + 1] = static_cast<unsigned char>(color.green());
          bitmapBuffer[index + 2] = static_cast<unsigned char>(color.red());
          bitmapBuffer[index + 3] = static_cast<unsigned char>(color.alpha());
        }        
    }

    HINSTANCE hInstance = GetModuleHandle(NULL);
    HICON hIcon = ::CreateIcon(hInstance, icon.width(), icon.height(), 
                               4, 8, 0, (BYTE*)&bitmapBuffer[0]);

    SendMessage(_hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
    DestroyIcon(hIcon);   
}


void MainWindowImpl::setAbove(bool isTop)
{
    HWND insertBelow = isTop ? HWND_TOPMOST : HWND_NOTOPMOST;
    
    SetWindowPos(_hwnd, insertBelow, 0, 0, 0, 0, 
                 SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);
}


void MainWindowImpl::setState(const WindowState& s)
{
    LONG style = GetWindowLong(_hwnd, GWL_STYLE);

    switch(s)
    {
        case WindowState::Normal:
        break;

        case WindowState::Maximized:
            style |= WS_MAXIMIZE;
        break;

        case WindowState::Minimized:
            style |= WS_MINIMIZE;
        break;
    }

    SetWindowLong(_hwnd, GWL_STYLE, style); 
}


void MainWindowImpl::setMinimumSize(const Gfx::SizeF& s)
{
}


void MainWindowImpl::setMaximumSize(const Gfx::SizeF& s)
{
}

} // namespace

} // namespace
