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

#include "Pt/Gui/ResizeEvent.h"
#include "Pt/Gui/Widget.h"

#include <string>
#include <iostream>
using namespace std;


namespace Pt {

namespace Gui {


const type_info& ResizeEvent::TYPE_INFO = typeid(ResizeEvent);

ResizeEvent::ResizeEvent(Widget& widget, size_t width, size_t height, ResizeEvent::Type resizeType)
: Event(widget), _width(width), _height(height), _resizeType(resizeType)
{
}


ResizeEvent::~ResizeEvent()
{}


size_t ResizeEvent::width() const
{
	return _width;
}


size_t ResizeEvent::height() const
{
	return _height;
}

ResizeEvent::Type ResizeEvent::resizeType() const
{
	return _resizeType;
}


const std::type_info& ResizeEvent::typeInfo() const
{
	static const std::type_info& ti = typeid(ResizeEvent);
	return ti;
}

} // namespace Gui

} // namespace Pt
