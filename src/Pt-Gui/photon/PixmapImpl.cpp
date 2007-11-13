/***************************************************************************
 *   Copyright (C) 2006 Marc Boris Duerner                                  *
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

#include <Pt/Gui/Application.h>
#include <Pt/Gui/Pixmap.h>

#include <iostream>

namespace Pt {

namespace Gui {

PixmapImpl::PixmapImpl(size_t width, size_t height)
: _size( std::max(width, size_t(1)), std::max(height, size_t(1)) )
{
	PhDim_t dim = { _size.width(), _size.height() };
	PhPoint_t trans = { 0, 0 };
	
	memset( &_image, 0, sizeof(PhImage_t) );
	_image.type = Pg_IMAGE_DIRECT_888;
	_image.image = NULL;
	_image.size = dim;
	
	_mc = PmMemCreateMC( &_image, &dim, &trans ); 
	
	_painter.set(*this);
}


PixmapImpl::PixmapImpl(const PixmapImpl& pimpl)
: _size( pimpl.size() )
{
	PmMemReleaseMC( _mc );

	PhDim_t dim = { _size.width(), _size.height() };
	PhPoint_t trans = { 0, 0 };
	
	memset( &_image, 0, sizeof(PhImage_t) );
	_image.type = Pg_IMAGE_DIRECT_888;
	_image.image = NULL;
	_image.size = dim;
	
	_mc = PmMemCreateMC( &_image, &dim, &trans ); 
	
	PhPoint_t to = {0, 0};
	PgDrawPhImage( &to, &(pimpl._image), 0);
	
	_painter.set(*this);
}


PixmapImpl::~PixmapImpl()
{
	PmMemReleaseMC( _mc );
}


Painter PixmapImpl::painter()
{
    return Painter(&_painter);
}


} // namespace Gui

} // namespace Pt
