/***************************************************************************
 *   Copyright (C) 2006 Marc Boris Duerner                                  *
 *   Copyright (C) 2005-2007 by Aloysius Indrayanto                        *
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
#include "PixmapPainterImpl.h"
#include "PixmapImpl.h"

namespace Pt {

namespace Gui {

PixmapPainterImpl::PixmapPainterImpl( )
{
}


PixmapPainterImpl::~PixmapPainterImpl()
{
}


void PixmapPainterImpl::set(PixmapImpl& pixmap)
{
	_pixmap = &pixmap;
}


void PixmapPainterImpl::begin()
{
	PmMemStart( _pixmap->mc() );
}


void PixmapPainterImpl::end()
{
	PmMemFlush( _pixmap->mc(), _pixmap->image() );
	PmMemStop( _pixmap->mc() );
}

} // namespace Gui

} // namespace Pt

