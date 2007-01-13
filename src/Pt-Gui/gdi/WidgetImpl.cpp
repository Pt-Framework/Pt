/***************************************************************************
 *   Copyright (C) 2006 Marc Boris D�rner                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "WidgetImpl.h"
#include "ApplicationImpl.h"
#include "WidgetPainter.h"
#include "win32.h"

#include <Pt/Gui/Application.h>
#include <Pt/Gui/Widget.h>
#include <Pt/Gui/Painter.h>
#include <Pt/Text/TextStream.h>
#include <Pt/Text/Utf16Codec.h>

#include <iostream>
#include <sstream>
using namespace std;


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


void WidgetImpl::setTitle(const Pt::Text::String& text)
{
	std::stringstream ss;
	Pt::Text::TextStream textStream(ss, new Pt::Text::Utf16Codec());
	textStream << text << Char(0); // Append extra \0 for proper line termination.
	textStream.flush();

	SetWindowTextW(_hwnd, (wchar_t*)ss.str().c_str());
}


Pt::Text::String WidgetImpl::title()
{
	std::vector<wchar_t> buffer(255);
	GetWindowTextW(_hwnd, &buffer[0], buffer.size());

	std::stringstream ss((char*)&buffer[0]);
	Pt::Text::TextStream textStream(ss, new Pt::Text::Utf16Codec());
	Pt::Text::String result;
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
