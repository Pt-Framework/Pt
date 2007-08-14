/***************************************************************************
 *   Copyright (C) 2006 Marc Boris Dürner                                  *
 *   Copyright (C) 2006 Aloysius Indrayanto                                *
 *   Copyright (C) 2007 Sebastian Pieck                                    *
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
#include "ApplicationImpl.h"
#include "WidgetImpl.h"

#include <Pt/Gui/Application.h>
#include <Pt/Gui/Widget.h>
#include <Pt/Gui/ResizeEvent.h>
#include <Pt/Gui/PaintEvent.h>

#include <Pt/Text/TextStream.h>
#include <Pt/Text/Utf16Codec.h>

#include <iostream>
#include <sstream>
#include <algorithm>
#include <stdio.h>

namespace Pt {

namespace Gui {

WidgetImpl::WidgetImpl(Widget& apiWidget, Widget* parent, const Math::Point& at, const Math::Size& size)
: _apiWidget(apiWidget)
, _isShown(false)
{
    GfEventLoop::instance().registerWidget(_apiWidget);
}


WidgetImpl::~WidgetImpl()
{
   // GfEventLoop::instance().unregisterWidget(_apiWidget);
}


void WidgetImpl::setTitle(const Pt::String& text)
{

}


Painter WidgetImpl::painter()
{
    return Painter(&_painter);
}


void WidgetImpl::show()
{
    if(_isShown)
        return;

    size_t width = Screen::instance().width();
    size_t height = Screen::instance().height();
    PaintEvent ev( _apiWidget, Pt::Math::Point(0,0), Pt::Math::Size(width, height) );
    GfEventLoop::instance().commitEvent(ev);
    _isShown = true;
}


void WidgetImpl::hide()
{

}


void WidgetImpl::setParent(Widget* parent)
{

}


void WidgetImpl::move(size_t x, size_t y)
{

}


void WidgetImpl::resize(size_t width, size_t height)
{
    size_t w = Screen::instance().width();
    size_t h = Screen::instance().height();
    ResizeEvent ev( _apiWidget, w, h );
    GfEventLoop::instance().commitEvent(ev);
}


} // namespace Gui

} // namespace Pt
