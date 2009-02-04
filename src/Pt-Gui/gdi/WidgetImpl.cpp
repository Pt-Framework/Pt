/*
 * Copyright (C) 2006 Marc Boris Duerner
 * Copyright (C) 2006 Tobias Mueller
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "WidgetImpl.h"
#include "ApplicationImpl.h"
#include "WidgetPainter.h"
#include "win32.h"

#include <Pt/Gui/Application.h>
#include <Pt/Gui/Widget.h>
#include <Pt/Gui/Painter.h>
#include <Pt/Gui/PaintEvent.h>
#include <Pt/Utf8Codec.h>
#include <Pt/TextStream.h>
#include <sstream>

namespace Pt {

namespace Gui {

WidgetImpl::WidgetImpl(Widget& widget, Widget* parent, const Math::Point& at, const Math::Size& size)
: _widget(widget)
, _painter(0)
, _deviceContextUsageCount(0)
{
    init(widget, parent, at, size);
}


WidgetImpl::WidgetImpl(Widget& widget, Widget* parent)
: _widget(widget)
, _painter(0)
, _deviceContextUsageCount(0)
{
    init(widget, parent, Math::Point(CW_USEDEFAULT, CW_USEDEFAULT), Math::Size(CW_USEDEFAULT, CW_USEDEFAULT));
}


void WidgetImpl::init(Widget& widget, Widget* parent, const Math::Point& at, const Math::Size& size)
{
    basic_string<TCHAR> windowClassName;
    HWND                parentWindowHandle;

    if (parent) {
        // Child window attributes
        windowClassName    = win32::fromMultiByte(GDIRegistry::CHILD_WINDOW_CLASS_NAME);
        parentWindowHandle = parent->impl()._hwnd;
        _windowStyle       = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

    } else {

        // Top level window attributes
        windowClassName = win32::fromMultiByte(GDIRegistry::TOP_WINDOW_CLASS_NAME);
        parentWindowHandle = NULL;
        #ifdef _WIN32_WCE
            _windowStyle       = WS_SYSMENU;
        #else
            _windowStyle       = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
        #endif
    }

    _hwnd = CreateWindow(
        windowClassName.c_str(),
        _T(""),
        _windowStyle,
        at.x(),
        at.y(),
        size.width(),
        size.height(),
        parentWindowHandle,
        NULL,
        GDIRegistry::instance().getInstanceHandle(),
        NULL
    );

    GDIRegistry::instance().registerWidget(_hwnd, widget);

    // Bring (child) widget to front, as windows seems to add child widgets behind all
    // existing widgets. We want new widgets to be on top of all existing widgets.
    BringWindowToTop(_hwnd);

    // Hide top-level windows from start.
    if (!parent) {
        ShowWindow(_hwnd, SW_HIDE);
    }
}


WidgetImpl::~WidgetImpl()
{
    // Destroy the painter (in case we created one).
    delete _painter;

    // Unregister from GDIRegistry.
    GDIRegistry::instance().unregisterWidget(_hwnd);

    // Destroy GDI window.
    DestroyWindow(_hwnd);

    // Release this window's device context.
    ReleaseDC(_hwnd, _deviceContext);
}


void WidgetImpl::setTitle(const Pt::String& text)
{
    std::wstring wtext;
    text.toUtf16( std::back_inserter(wtext) );
    SetWindowTextW( _hwnd, wtext.c_str() );
}


Pt::String WidgetImpl::title()
{
    int length = GetWindowTextLengthW(_hwnd);
    
    std::vector<wchar_t> buffer(length + 1);
    GetWindowTextW(_hwnd, &buffer[0], length);

    std::stringstream ss(Pt::win32::toUTF8(&buffer[0]));
    Pt::TextStream textStream(ss, new Pt::Utf8Codec());
    Pt::String result;
    getline(textStream, result);
    
    return result;
}


void WidgetImpl::setParent(Widget* newParent)
{
    if (newParent) {
        SetParent(_hwnd, newParent->impl()._hwnd);
    } else {
        SetParent(_hwnd, NULL);
    }
}


void WidgetImpl::show()
{
    ShowWindow(_hwnd, SW_SHOW);
}


void WidgetImpl::hide()
{
    ShowWindow(_hwnd, SW_HIDE);
}


void WidgetImpl::move(size_t x, size_t y)
{
    SetWindowPos(_hwnd, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}


void WidgetImpl::resize(size_t width, size_t height)
{
    RECT clientRect;
    SetRect(&clientRect, 0, 0, width - 1, height - 1);

    // TODO We currently expect the window to not have a menu. If we natively support that,
    // we should change this "false" here to a varying value.
    AdjustWindowRectEx(&clientRect, _windowStyle, false, 0);

    LONG clientWidth  = clientRect.right  - clientRect.left + 1;
    LONG clientHeight = clientRect.bottom - clientRect.top  + 1;
    SetWindowPos(_hwnd, NULL, 0, 0, clientWidth, clientHeight, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
}


HWND WidgetImpl::hwnd()
{
    return _hwnd;
}


Painter WidgetImpl::painter()
{
    if (0 == _painter) {
        _painter = new WidgetPainter(*this);
    }

    return Painter(_painter);
}


HDC WidgetImpl::beginPaint()
{
    if (_deviceContextUsageCount == 0) {
        _deviceContext = GetDC(_hwnd);

        _oldPen   = (HPEN)  GetCurrentObject(_deviceContext, OBJ_PEN);
        _oldBrush = (HBRUSH)GetCurrentObject(_deviceContext, OBJ_BRUSH);
        _oldFont  = (HFONT) GetCurrentObject(_deviceContext, OBJ_FONT);
    }

    _deviceContextUsageCount++;

    return _deviceContext;
}


void WidgetImpl::endPaint()
{
    if (_deviceContextUsageCount > 0) {
        _deviceContextUsageCount--;
    }

    if (_deviceContextUsageCount == 0) {
        HPEN oldPen = (HPEN)SelectObject(_deviceContext, _oldPen);
        DeleteObject(oldPen);

        HPEN oldBrush = (HPEN)SelectObject(_deviceContext, _oldBrush);
        DeleteObject(oldBrush);

        HPEN oldFont = (HPEN)SelectObject(_deviceContext, _oldFont);
        DeleteObject(oldFont);

        ReleaseDC(_hwnd, _deviceContext);
        _deviceContext = 0;
    }
}


HDC WidgetImpl::deviceContext() const
{
    return _deviceContext;
}


bool WidgetImpl::isPainting() const
{
    return _deviceContextUsageCount != 0;
}


} // namespace Gui

} // namespace Pt
