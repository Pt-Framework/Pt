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
#include "ScreenImpl.h"

#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/WindowManager.h>
#include <Pt/Hmi/WindowStateEvent.h>
#include <Pt/Math.h>
#include <cassert>

namespace Pt {

namespace Hmi {

MainWindowImpl::MainWindowImpl(ScreenImpl& wm, Window& w)
: WindowFrame(wm, w)
, _wm(wm)
, _window(w)
, _hwnd(0)
{
    HINSTANCE hInstance = GetModuleHandle(NULL);
  
    LONG style = 0;
    LONG exStyle = 0;
    
    switch( w.type() )
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

    Base::onSetParent(&wm);
}


MainWindowImpl::~MainWindowImpl()
{
    DestroyWindow( _hwnd);
}


void MainWindowImpl::onInit(Window& w)
{
    Gfx::PaintSurface& surface = this->surface();
    Gfx::PointF surfacePos(0, 0);
    w.setSurface(&surface, surfacePos);

    w.setNextResponder(this);

    double scaling = scaleFactor();
    
    RescaleEvent ev(w, scaling);
    w.processEvent(ev);
}


void MainWindowImpl::onRelease(Window& w)
{
    w.setNextResponder(0);
    w.setSurface( 0, Gfx::PointF() );
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


Gfx::PointF MainWindowImpl::toScreen(const Gfx::PointF& pos) const
{
    Gfx::PointF physicalPos = surface().toPhysical(pos);
  
    POINT p = { lround(physicalPos.x()), 
                lround(physicalPos.y()) };

    ClientToScreen(_hwnd, &p);

    Gfx::PointF screenPos(p.x, p.y);
    Gfx::PointF logicalPos = surface().toLogical(screenPos);
    return logicalPos;
}


Gfx::PointF MainWindowImpl::fromScreen(const Gfx::PointF& pos) const
{
    Gfx::PointF physicalPos = surface().toPhysical(pos);

    POINT p = { lround(physicalPos.x()), 
                lround(physicalPos.y()) };

    ScreenToClient(_hwnd, &p);

    Gfx::PointF clientPos(p.x, p.y);
    Gfx::PointF logicalPos = surface().toLogical(clientPos);
    return logicalPos;
}


Gfx::PointF MainWindowImpl::onToWindow(const Window& w, 
                                       const Gfx::PointF& pos) const
{
    return pos;
}


Gfx::PointF MainWindowImpl::onFromWindow(const Window& w, 
                                         const Gfx::PointF& pos) const
{
    return pos;
}


Gfx::PointF MainWindowImpl::onToParent(const Gfx::PointF& pos) const
{
    return _wm.fromFrame(*this, pos); 
}
     
        
Gfx::PointF MainWindowImpl::onFromParent(const Gfx::PointF& pos) const
{ 
    return _wm.toFrame(*this, pos); 
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


void MainWindowImpl::onRepaint(Window& w, const Gfx::RectF& rect)
{
    Gfx::PointF screenPos = toScreen(rect.topLeft());
    Gfx::RectF screenRect( screenPos, rect.size() );

    _wm.repaint(screenRect);
}


void MainWindowImpl::onProcessPaintEvent(const PaintEvent& ev)
{
    Base::onProcessPaintEvent(ev);

    PaintEvent rev( _window, ev.rect() );
    _window.processEvent(rev);
}


void MainWindowImpl::onPaintEvent(const PaintEvent& ev)
{
    Base::onPaintEvent(ev);
}


void MainWindowImpl::onProcessRescaleEvent(const RescaleEvent& ev)
{
    //HDC screen = GetDC(_hwnd);
    //int dpix = GetDeviceCaps(screen, LOGPIXELSX);
    //std::clog << "HWND SCALING DPI: " << dpix << std::endl;

    //double hwndScaleFactor = dpix / 96.0;
    //std::clog << "HWND SCALING: " << dpix / 96.0 << std::endl;
    //scaling *= hwndScaleFactor;

    Base::onProcessRescaleEvent(ev);

    double scaling = ev.scaleFactor();
    RescaleEvent rev(_window, scaling);
    _window.processEvent(rev);
}


void MainWindowImpl::onRescaleEvent(const RescaleEvent& ev)
{
    Base::onRescaleEvent(ev);
}


void MainWindowImpl::onShow(Window& w, bool visible)
{
    if(visible)
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


void MainWindowImpl::onProcessShowEvent(const ShowEvent& ev)
{
    Base::onProcessShowEvent(ev);

    ShowEvent rev( _window, ev.visible() );
    _window.processEvent(rev);
}


void MainWindowImpl::onShowEvent(const ShowEvent& ev)
{
    Base::onShowEvent(ev);
}


void MainWindowImpl::onActivate(Window& w, bool active)
{
    if( ! active )
        return;

    SetActiveWindow(_hwnd);
}


void MainWindowImpl::onProcessActivateEvent(const ActivateEvent& ev)
{
    Base::onProcessActivateEvent(ev);

    ActivateEvent aev( _window, ev.isActive() );
    _window.processEvent(aev);
}


void MainWindowImpl::onActivateEvent(const ActivateEvent& ev)
{
    Base::onActivateEvent(ev);
}


void MainWindowImpl::onEnable(Window& w, bool enable)
{
    EnableWindow(_hwnd, enable);
}


void MainWindowImpl::onProcessEnableEvent(const EnableEvent& ev)
{
    Base::onProcessEnableEvent(ev);

    EnableEvent eev( _window, ev.enabled() );
    _window.processEvent(eev);
}


void MainWindowImpl::onEnableEvent(const EnableEvent& ev)
{    
    Base::onEnableEvent(ev);
}


void MainWindowImpl::onMove(Window& w, const Gfx::PointF& pos)
{
    Gfx::PointF aligedPos = w.surface().align(pos);

    Gfx::PointF p = w.surface().toPhysical(aligedPos);

    SetWindowPos(_hwnd, 0, p.x(), p.y(), 0, 0, 
                 SWP_DRAWFRAME|SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOZORDER);
}


void MainWindowImpl::onSetAbove(Window& w, bool above)
{
    HWND insertBelow = above ? HWND_TOPMOST : HWND_NOTOPMOST;
    
    SetWindowPos(_hwnd, insertBelow, 0, 0, 0, 0, 
                 SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);
}


void MainWindowImpl::onSetState(Window& w, const WindowState& state)
{
    LONG style = GetWindowLong(_hwnd, GWL_STYLE);

    switch(state)
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


void MainWindowImpl::onProcessWindowStateEvent(const WindowStateEvent& ev)
{
    Base::onProcessWindowStateEvent(ev);

    WindowStateEvent wse( _window, ev.state() );
    Application::instance().processEvent(wse);
}


void MainWindowImpl::onWindowStateEvent(const WindowStateEvent& ev)
{
}


void MainWindowImpl::onSetTitle(Window& w, const std::string& text)
{
    SetWindowText( _hwnd, text.c_str() );
}


void MainWindowImpl::onSetIcon(Window& w, const Gfx::Image& icon)
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


void MainWindowImpl::onSetSizeLimits(Window& w, const Gfx::SizeF& minSize, 
                                                const Gfx::SizeF& maxSize)
{
}


void MainWindowImpl::onResize(Window& w, const Gfx::SizeF& s)
{
    //
    // align to physical pixel grid
    //
    Gfx::SizeF alignedSize = surface().align(s);

    //
    // maximum width and height
    //
    if( alignedSize.width() > w.maximumSize().width() )
        alignedSize.setWidth( w.maximumSize().width() );

    if( alignedSize.height() > w.maximumSize().height() )
        alignedSize.setHeight( w.maximumSize().height() );

    if( alignedSize.width() < w.minimumSize().width() )
        alignedSize.setWidth( w.minimumSize().width() );

    if( alignedSize.height() < w.minimumSize().height() )
        alignedSize.setHeight( w.minimumSize().height() );

    const Gfx::SizeF size = w.surface().toPhysical(alignedSize);

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


void MainWindowImpl::onProcessResizeEvent(const ResizeEvent& ev)
{
    Base::onProcessResizeEvent(ev);

    ResizeEvent rev( _window, ev.size() );
    _window.processEvent(rev);
}


void MainWindowImpl::onClose(Window& w)
{
    PostMessage(_hwnd, WM_CLOSE, 0, 0);
}


void MainWindowImpl::onProcessCloseEvent(const CloseEvent& ev)
{
    onCloseEvent(ev);

    CloseEvent cev(_window);
    _window.processEvent(cev);
}


void MainWindowImpl::onCloseEvent(const CloseEvent& ev)
{
}

} // namespace

} // namespace
