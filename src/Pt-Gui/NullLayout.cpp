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

#include "Pt/Gui/NullLayout.h"
#include "Pt/Math/Size.h"

#include <list>


namespace Pt {

namespace Gui {

NullLayout::NullLayout(Widget& widget)
: Layout(widget)
{
}


void NullLayout::update()
{
	// Null layout. Don't do anything.
}


void NullLayout::remove(Widget& widget)
{
	// Null layout. Don't do anyhting.
}


Math::Size NullLayout::minimumSize()
{
	return Math::Size(0, 0);
}


Math::Size NullLayout::preferredSize()
{
	return Math::Size(0, 0);
}


NullLayout* NullLayout::createFor(Widget& widget)
{
	return new NullLayout(widget);
}


} // namespace Gui

} // namespace Pt
