/***************************************************************************
 *   Copyright (C) 2006 Marc Boris Dürner                                  *
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

#include <Pt/Gui/Application.h>
#include <Pt/Gui/Widget.h>
#include <Pt/Gui/ResizeEvent.h>

#include <iostream>
using namespace std;


namespace Pt {

namespace Gui {

WidgetImpl::WidgetImpl(Widget& apiWidget, Widget* parent, const Gfx::Point& at, const Gfx::Size& size)
: _apiWidget(apiWidget),
  _parent(parent),
  _isMain(false),
  _rect(at, size)
{
	clog << "[" << this << "] WidgetImpl::WidgetImpl" << endl;


}


void WidgetImpl::unparent()
{

}


void WidgetImpl::setTitle(const char* text)
{

}


WidgetImpl::~WidgetImpl()
{
	clog << "[" << this << "] WidgetImpl::~WidgetImpl()" << endl;
	this->destroy();
}


void WidgetImpl::setMainWidget(bool isMain)
{
	_isMain = isMain;
}


bool WidgetImpl::isMainWidget()
{
	return _isMain;
}


void WidgetImpl::show()
{

}


void WidgetImpl::hide()
{

}


void WidgetImpl::resizeEvent(const ResizeEvent& event)
{

}


void WidgetImpl::move(size_t x, size_t y)
{

}


void WidgetImpl::resize(size_t width, size_t height)
{

}


void WidgetImpl::destroy()
{

}


bool WidgetImpl::valid()
{
	return false;
}

const Gfx::Rect& WidgetImpl::rect() const
{
	return _rect;
}

} // namespace Gui

} // namespace Pt
