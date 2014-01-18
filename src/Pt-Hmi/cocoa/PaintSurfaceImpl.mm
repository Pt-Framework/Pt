/***************************************************************************
 *   Copyright (C) 2006 Marc Boris Duerner                                 *
 *   Copyright (C) 2014 Laurentiu-Gheorghe Crisan                          *
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
#include "PaintSurfaceImpl.h"

namespace Pt {
namespace Hmi {

PaintSurfaceImpl::PaintSurfaceImpl(const Pt::Gfx::SizeF& size)
: _size(size)
{
    _image = [[NSImage alloc] initWithSize:NSMakeSize(_size.width(),  _size.height())];
}


PaintSurfaceImpl::~PaintSurfaceImpl()
{
    [_image release];
}

void PaintSurfaceImpl::resize(const Pt::Gfx::SizeF& size)
{
	_size = size;
	[_image release];
	_image = [[NSImage alloc] initWithSize:NSMakeSize(_size.width(),  _size.height())];
}


Pt::Gfx::ARgbImage PaintSurfaceImpl::toImage()
{
	Pt::Gfx::ARgbImage image(800,600);
	return image;
}

}}
