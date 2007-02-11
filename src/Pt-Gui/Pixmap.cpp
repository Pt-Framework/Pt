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
#include "PixmapImpl.h"

#include "Pt/Gui/Pixmap.h"
#include "Pt/Gui/Painter.h"


namespace Pt {

namespace Gui {


Pixmap::Pixmap(size_t width, size_t height)
{
    _impl = new PixmapImpl(width, height);
}


Pixmap::Pixmap(const Pixmap& pixmap)
{
    _impl = new PixmapImpl( pixmap.impl() );
}


Pixmap::~Pixmap()
{
    delete _impl;
}


const Math::Size& Pixmap::size() const
{
    return _impl->size();
}


Painter Pixmap::painter()
{
    return _impl->painter();
}



} // namespace Gui

} // namespace Pt
