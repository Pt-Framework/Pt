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

#include "WindowImpl.h"
#include "ScreenImpl.h"
#include "ApplicationImpl.h"

#include "../generic/GenericGraphicsBackend.h"

#if defined(PT_FORMS_WIN32_GDI)
#include "../gdi/GdiGraphicsBackend.h"
#endif

#if defined(PT_FORMS_WIN32_DIRECT2D)
#include "../direct2d/Direct2dGraphicsBackend.h"
#endif

#include "../direct2d/D2DDevice.h"

#include <Pt/Forms/GraphicsBackend.h>
#include <Pt/Forms/Application.h>
#include <Pt/Forms/Window.h>
#include <Pt/Forms/WindowManager.h>
#include <Pt/Forms/WindowStateEvent.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Math.h>
#include <cassert>
#include <stdexcept>

namespace Pt {

namespace Forms {

WindowImpl::WindowImpl(ScreenImpl& wm, Window& w, GraphicsBackend& graphicsBackend)
: WindowFrame(wm, w)
, _wm(wm)
, _window(w)
, _hwnd(0)
, _iconHandle(0)
, _genericBackend(0)
, _direct2dBackend(0)
, _gdiBackend(0)
, _paintWindow(&WindowImpl::paintWindowNone)
, _swapChain(0)
, _presentCtx(0)
, _targetBmp(0)
, _swapChainWidth(0)
, _swapChainHeight(0)
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

    _hwnd = CreateWindowEx(exStyle, "Pt-Forms", "", style,
                           0, 0, 10, 10, GetDesktopWindow(), 
                           NULL, hInstance, NULL);

    bindBackend(graphicsBackend);

    Base::onSetParent(&wm);
}


WindowImpl::~WindowImpl()
{
    if(_targetBmp)
    {
        _targetBmp->Release();
        _targetBmp = 0;
    }

    if(_presentCtx)
    {
        _presentCtx->Release();
        _presentCtx = 0;
    }

    if(_swapChain)
    {
        _swapChain->Release();
        _swapChain = 0;
    }

    DestroyWindow( _hwnd);

    if(_iconHandle != 0)
        DestroyIcon(_iconHandle);
}


void WindowImpl::onInit(Window& w)
{
    Pixmap& surface = pixmap();
    Gfx::PointF surfacePos(0, 0);
    w.setSurface(&surface, surfacePos);

    w.setNextResponder(this);

    if( screen() )
    {
        double scaling = scaleFactor();
    
        RescaleEvent ev(w, scaling);
        w.processEvent(ev);
    }
}


void WindowImpl::onRelease(Window& w)
{
    w.setNextResponder(0);
    w.setSurface( 0, Gfx::PointF() );
}


void WindowImpl::onConnect(Screen& screen)
{
    Base::onConnect(screen);
}


void WindowImpl::onDisconnect()
{
    Base::onDisconnect();
}


Gfx::PointF WindowImpl::toScreen(const Gfx::PointF& pos) const
{
    const Gfx::Scaling& scaling = this->scaling();
    
    Gfx::PointF physicalPos = scaling.toPhysical(pos);
  
    POINT p = { lround(physicalPos.x()), 
                lround(physicalPos.y()) };

    ClientToScreen(_hwnd, &p);

    Gfx::PointF screenPos(p.x, p.y);
    Gfx::PointF logicalPos = scaling.toLogical(screenPos);
    return logicalPos;
}


Gfx::PointF WindowImpl::fromScreen(const Gfx::PointF& pos) const
{
    const Gfx::Scaling& scaling = this->scaling();

    Gfx::PointF physicalPos = scaling.toPhysical(pos);

    POINT p = { lround(physicalPos.x()), 
                lround(physicalPos.y()) };

    ScreenToClient(_hwnd, &p);

    Gfx::PointF clientPos(p.x, p.y);
    Gfx::PointF logicalPos = scaling.toLogical(clientPos);
    return logicalPos;
}


Gfx::PointF WindowImpl::onToWindow(const Window& w, 
                                       const Gfx::PointF& pos) const
{
    return pos;
}


Gfx::PointF WindowImpl::onFromWindow(const Window& w, 
                                         const Gfx::PointF& pos) const
{
    return pos;
}


Gfx::PointF WindowImpl::onToParent(const Gfx::PointF& pos) const
{
    return _wm.fromFrame(*this, pos); 
}
     
        
Gfx::PointF WindowImpl::onFromParent(const Gfx::PointF& pos) const
{ 
    return _wm.toFrame(*this, pos); 
}


void WindowImpl::paint(const Gfx::RectF& rect)
{
    RECT wRect;
    wRect.left   = lround( rect.left() );
    wRect.top    = lround( rect.top() );
    wRect.right  = lround( rect.right() + 1 );
    wRect.bottom = lround( rect.bottom() + 1 );
    
    InvalidateRect(_hwnd, &wRect, FALSE);
}


void WindowImpl::onRepaint(Window& w, const Gfx::RectF& rect)
{
    Gfx::PointF screenPos = toScreen(rect.topLeft());
    Gfx::RectF screenRect( screenPos, rect.size() );

    _wm.repaint(screenRect);
}


void WindowImpl::onProcessPaintEvent(const PaintEvent& ev)
{
    PaintEvent rev( _window, ev.rect() );
    _window.processEvent(rev);

    Base::onProcessPaintEvent(ev);
}


void WindowImpl::onPaintEvent(const PaintEvent& ev)
{
    Base::onPaintEvent(ev);

    Gfx::RectF updateRect = ev.rect();
    updateRect = Gfx::RectF( updateRect.topLeft() * scaleFactor(), 
                             updateRect.size() * scaleFactor() );

    PAINTSTRUCT ps;
    HDC windowContext = BeginPaint(_hwnd, &ps);

    (this->*_paintWindow)(windowContext, updateRect);

    EndPaint(_hwnd, &ps);
}


void WindowImpl::bindBackend(GraphicsBackend& graphicsBackend)
{
    _paintWindow = &WindowImpl::paintWindowNone;
    _genericBackend = 0;
    _direct2dBackend = 0;
    _gdiBackend = 0;

#if defined(PT_FORMS_WIN32_DIRECT2D)
    _direct2dBackend = dynamic_cast<Direct2dGraphicsBackend*>(&graphicsBackend);
    if(_direct2dBackend)
    {
        _paintWindow = &WindowImpl::paintWindowDirect2d;
        return;
    }
#endif

#if defined(PT_FORMS_WIN32_GDI)
    _gdiBackend = dynamic_cast<GdiGraphicsBackend*>(&graphicsBackend);
    if(_gdiBackend)
    {
        _paintWindow = &WindowImpl::paintWindowGdi;
        return;
    }
#endif

    _genericBackend = dynamic_cast<GenericGraphicsBackend*>(&graphicsBackend);
    if(_genericBackend)
    {
        _paintWindow = &WindowImpl::paintWindowGeneric;
        return;
    }

    throw std::invalid_argument("invalid graphics");
}


void WindowImpl::paintWindowNone(HDC /*windowContext*/, const Gfx::RectF& /*updateRect*/)
{
}


void WindowImpl::paintWindowGeneric(HDC /*windowContext*/, const Gfx::RectF& /*updateRect*/)
{
    createSwapChain();

    UINT sourceWidth = 0;
    UINT sourceHeight = 0;

    RECT clientRect;
    if(GetClientRect(_hwnd, &clientRect))
    {
        sourceWidth = static_cast<UINT>(clientRect.right - clientRect.left);
        sourceHeight = static_cast<UINT>(clientRect.bottom - clientRect.top);
        resizeSwapChain(sourceWidth, sourceHeight);
    }

    if(_targetBmp && _swapChain && sourceWidth > 0 && sourceHeight > 0)
    {
        const Gfx::Image& image = _genericBackend->image( pixmap() );
        UINT imgW = static_cast<UINT>(image.width());
        UINT imgH = static_cast<UINT>(image.height());

        UINT copyW = sourceWidth < imgW ? sourceWidth : imgW;
        UINT copyH = sourceHeight < imgH ? sourceHeight : imgH;

        if(copyW > 0 && copyH > 0)
        {
            D2D1_RECT_U destRect = D2D1::RectU(0, 0, copyW, copyH);
            UINT pitch = imgW * 4;
            _targetBmp->CopyFromMemory(&destRect, image.data(), pitch);
        }

        _swapChain->Present(0, 0);
    }
}

#if defined(PT_FORMS_WIN32_DIRECT2D)

void WindowImpl::paintWindowDirect2d(HDC /*windowContext*/, const Gfx::RectF& /*updateRect*/)
{
    assert(_direct2dBackend != nullptr);

    createSwapChain();

    UINT sourceWidth = 0;
    UINT sourceHeight = 0;

    RECT clientRect;
    if( GetClientRect(_hwnd, &clientRect) )
    {
        sourceWidth = static_cast<UINT>(clientRect.right - clientRect.left);
        sourceHeight = static_cast<UINT>(clientRect.bottom - clientRect.top);
        resizeSwapChain(sourceWidth, sourceHeight);
    }

    if(_targetBmp && _swapChain && sourceWidth > 0 && sourceHeight > 0)
    {
        ID2D1Bitmap1* d2dBmp = _direct2dBackend->bitmap( pixmap() );
        if(d2dBmp)
        {
            D2D1_SIZE_U sourceSize = d2dBmp->GetPixelSize();

            UINT32 copyW = sourceWidth < sourceSize.width ? sourceWidth : sourceSize.width;
            UINT32 copyH = sourceHeight < sourceSize.height ? sourceHeight : sourceSize.height;

            if(copyW > 0 && copyH > 0)
            {
                D2D1_RECT_U srcRect = D2D1::RectU(0, 0, copyW, copyH);
                _targetBmp->CopyFromBitmap(nullptr, d2dBmp, &srcRect);
            }
        }

        _swapChain->Present(0, 0);
    }
}

#endif

#if defined(PT_FORMS_WIN32_GDI)

void WindowImpl::paintWindowGdi(HDC windowContext, const Gfx::RectF& updateRect)
{
    if( ! _gdiBackend)
        return;

    HDC bitmapContext = _gdiBackend->bitmapContext(pixmap());
    if( ! bitmapContext)
        return;

    BitBlt(windowContext, updateRect.x(), updateRect.y(),
           updateRect.width(), updateRect.height(),
           bitmapContext, updateRect.x(), updateRect.y(), SRCCOPY);
}

#endif

void WindowImpl::onProcessRescaleEvent(const RescaleEvent& ev)
{
    HDC screen = GetDC(_hwnd);
    int dpix = GetDeviceCaps(screen, LOGPIXELSX);
    //std::clog << "HWND SCALING DPI: " << dpix << std::endl;

    double hwndScaleFactor = dpix / 96.0;
    //std::clog << "HWND SCALING: " << dpix / 96.0 << std::endl;

    double scaling = ev.scaleFactor();
    scaling *= hwndScaleFactor;

    RescaleEvent rev(*this, scaling);
    Base::onProcessRescaleEvent(rev);

    RescaleEvent wev(_window, scaling);
    _window.processEvent(wev);
}


void WindowImpl::onRescaleEvent(const RescaleEvent& ev)
{
    Base::onRescaleEvent(ev);
}


void WindowImpl::onShow(Window& w, bool visible)
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


void WindowImpl::onProcessShowEvent(const ShowEvent& ev)
{
    Base::onProcessShowEvent(ev);

    ShowEvent rev( _window, ev.visible() );
    _window.processEvent(rev);
}


void WindowImpl::onShowEvent(const ShowEvent& ev)
{
    Base::onShowEvent(ev);
}


void WindowImpl::onActivate(Window& w, bool active)
{
    if( ! active )
        return;

    SetActiveWindow(_hwnd);
}


void WindowImpl::onProcessActivateEvent(const ActivateEvent& ev)
{
    Base::onProcessActivateEvent(ev);

    ActivateEvent aev( _window, ev.isActive() );
    _window.processEvent(aev);
}


void WindowImpl::onActivateEvent(const ActivateEvent& ev)
{
    Base::onActivateEvent(ev);
}


void WindowImpl::onEnable(Window& w, bool enable)
{
    EnableWindow(_hwnd, enable);
}


void WindowImpl::onProcessEnableEvent(const EnableEvent& ev)
{
    Base::onProcessEnableEvent(ev);

    EnableEvent eev( _window, ev.enabled() );
    _window.processEvent(eev);
}


void WindowImpl::onEnableEvent(const EnableEvent& ev)
{    
    Base::onEnableEvent(ev);
}


void WindowImpl::onMove(Window& w, const Gfx::PointF& pos)
{
    const Gfx::Scaling& scaling = w.scaling();

    Gfx::PointF aligedPos = scaling.align(pos);

    Gfx::PointF p = scaling.toPhysical(aligedPos);

    SetWindowPos(_hwnd, 0, p.x(), p.y(), 0, 0, 
                 SWP_DRAWFRAME|SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOZORDER);
}


void WindowImpl::onProcessMoveEvent(const MoveEvent& ev)
{
    Base::onProcessMoveEvent(ev);

    MoveEvent mev( _window, ev.position() );
    _window.processEvent(mev);
}


void WindowImpl::onSetAbove(Window& w, bool above)
{
    HWND insertBelow = above ? HWND_TOPMOST : HWND_NOTOPMOST;
    
    SetWindowPos(_hwnd, insertBelow, 0, 0, 0, 0, 
                 SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);
}


void WindowImpl::onSetState(Window& w, const WindowState& state)
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


void WindowImpl::onProcessWindowStateEvent(const WindowStateEvent& ev)
{
    Base::onProcessWindowStateEvent(ev);

    WindowStateEvent wse( _window, ev.state() );
    Application::instance().processEvent(wse);
}


void WindowImpl::onWindowStateEvent(const WindowStateEvent& ev)
{
}


void WindowImpl::onSetTitle(Window& w, const std::string& text)
{
    SetWindowText( _hwnd, text.c_str() );
}


void WindowImpl::onSetIcon(Window& w, const Gfx::Image& icon)
{
    if (_iconHandle != 0)
    {
        DestroyIcon(_iconHandle);
        _iconHandle = 0;
    }

    if(icon.width() == 0 || icon.height() == 0)
    {
        SendMessage(_hwnd, WM_SETICON, ICON_SMALL, 0);
        return;
    }

    const size_t planes = 4;
    std::vector<Pt::uint8_t> bitmapBuffer(icon.width() * icon.height() * planes);

    size_t index = 0;

    for(const auto& pixel : pixelView(icon))
    {
        Gfx::Color color = pixel.getColor();

        bitmapBuffer[index]     = static_cast<unsigned char>(color.blue());
        bitmapBuffer[index + 1] = static_cast<unsigned char>(color.green());
        bitmapBuffer[index + 2] = static_cast<unsigned char>(color.red());
        bitmapBuffer[index + 3] = static_cast<unsigned char>(color.alpha());
        index += planes;
    }

    HINSTANCE hInstance = GetModuleHandle(NULL);
    _iconHandle = ::CreateIcon(hInstance, icon.width(), icon.height(),
                               4, 8, 0, (BYTE*)&bitmapBuffer[0]);

    SendMessage(_hwnd, WM_SETICON, ICON_SMALL, (LPARAM)_iconHandle);  
}


void WindowImpl::onSetSizeLimits(Window& w, const Gfx::SizeF& minSize, 
                                                const Gfx::SizeF& maxSize)
{
}


void WindowImpl::onAutoCenter(Window& w, const Gfx::SizeF* size) 
{
    _wm.onAutoCenter(*this, size);
}


Gfx::SizeF WindowImpl::onResize(Window& w, const Gfx::SizeF& s)
{
    //
    // align to physical pixel grid
    //
    Gfx::SizeF alignedSize = scaling().align(s);

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

    const Gfx::SizeF size = w.scaling().toPhysical(alignedSize);

    RECT clientRect;
    SetRect(&clientRect, 0, 0, lround(size.width()) - 1, 
                               lround(size.height()) - 1);

    LONG style = GetWindowLong(_hwnd, GWL_STYLE);
    LONG exStyle = GetWindowLong(_hwnd, GWL_EXSTYLE);

    AdjustWindowRectEx(&clientRect, style, FALSE, exStyle);

    LONG clientWidth  = clientRect.right  - clientRect.left + 1;
    LONG clientHeight = clientRect.bottom - clientRect.top  + 1;

    //std::clog << "SetWindowPos " << clientWidth << "x" << clientHeight << std::endl;
    SetWindowPos(_hwnd, NULL, 0, 0, clientWidth, clientHeight, 
                 SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE);

    return alignedSize;
}


void WindowImpl::onProcessResizeEvent(const ResizeEvent& ev)
{
    Base::onProcessResizeEvent(ev);

    ResizeEvent rev( _window, ev.size() );
    _window.processEvent(rev);
}


void WindowImpl::onClose(Window& w)
{
    PostMessage(_hwnd, WM_CLOSE, 0, 0);
}


void WindowImpl::onProcessCloseEvent(const CloseEvent& ev)
{
    onCloseEvent(ev);

    CloseEvent cev(_window);
    _window.processEvent(cev);
}


void WindowImpl::onCloseEvent(const CloseEvent& ev)
{
}


void WindowImpl::createSwapChain()
{
    if(_swapChain)
        return;

    D2DDevice& d2d = Application::instance().impl()->d2d();

    IDXGIFactory2* dxgiFactory = d2d.dxgiFactory();
    if( ! dxgiFactory)
        return;

    IDXGIDevice* dxgiDevice = d2d.dxgiDevice();
    if( ! dxgiDevice)
        return;

    DXGI_SWAP_CHAIN_DESC1 desc = {};
    desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.BufferCount = 1;
    desc.SwapEffect = DXGI_SWAP_EFFECT_SEQUENTIAL;
    desc.Scaling = DXGI_SCALING_STRETCH;

    HRESULT hr = dxgiFactory->CreateSwapChainForHwnd(
        dxgiDevice, _hwnd, &desc, nullptr, nullptr, &_swapChain);

    if(FAILED(hr) || ! _swapChain)
    {
        _swapChain = 0;
        return;
    }

    DXGI_SWAP_CHAIN_DESC1 actualDesc = {};
    if(SUCCEEDED(_swapChain->GetDesc1(&actualDesc)))
    {
        _swapChainWidth = actualDesc.Width;
        _swapChainHeight = actualDesc.Height;
    }

    if( ! _presentCtx)
    {
        d2d.d2dDevice()->CreateDeviceContext(
            D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &_presentCtx);
    }

    if(_presentCtx && ! _targetBmp)
    {
        IDXGISurface* surface = 0;
        if(SUCCEEDED(_swapChain->GetBuffer(0, __uuidof(IDXGISurface),
                                           reinterpret_cast<void**>(&surface))))
        {
            D2D1_BITMAP_PROPERTIES1 bmpProps = D2D1::BitmapProperties1(
                D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                                  D2D1_ALPHA_MODE_PREMULTIPLIED));
            _presentCtx->CreateBitmapFromDxgiSurface(surface, bmpProps, &_targetBmp);
            surface->Release();
        }
    }
}


void WindowImpl::resizeSwapChain(UINT width, UINT height)
{
    if( ! _swapChain)
        return;

    if(width == 0 || height == 0)
        return;

    if(width == _swapChainWidth && height == _swapChainHeight)
        return;

    if(_presentCtx)
        _presentCtx->SetTarget(nullptr);

    if(_targetBmp)
    {
        _targetBmp->Release();
        _targetBmp = 0;
    }

    HRESULT hr = _swapChain->ResizeBuffers(
        0, width, height, DXGI_FORMAT_UNKNOWN, 0);

    if(FAILED(hr))
    {
        _swapChain->Release();
        _swapChain = 0;
        _swapChainWidth = 0;
        _swapChainHeight = 0;
        return;
    }

    _swapChainWidth = width;
    _swapChainHeight = height;

    if(_presentCtx)
    {
        IDXGISurface* surface = 0;
        if(SUCCEEDED(_swapChain->GetBuffer(0, __uuidof(IDXGISurface),
                                           reinterpret_cast<void**>(&surface))))
        {
            D2D1_BITMAP_PROPERTIES1 bmpProps = D2D1::BitmapProperties1(
                D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                                  D2D1_ALPHA_MODE_PREMULTIPLIED));
            _presentCtx->CreateBitmapFromDxgiSurface(surface, bmpProps, &_targetBmp);
            surface->Release();
        }
    }
}


} // namespace

} // namespace
