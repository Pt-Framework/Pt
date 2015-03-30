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

PaintSurfaceImpl::PaintSurfaceImpl()
: _size(800, 600)
{
    create();
}


PaintSurfaceImpl::~PaintSurfaceImpl()
{
    destroy();
}
    
void PaintSurfaceImpl::destroy()
{
    if(  _context == nullptr)
        return;
    
    CGContextRelease(_context);
    _context = nullptr;
}
    
void PaintSurfaceImpl::create()
{
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    
    _context = CGBitmapContextCreate(nullptr, _size.width(), _size.height(), 8, 0, colorSpace, kCGImageAlphaPremultipliedLast);
    CGColorSpaceRelease(colorSpace);
}
    
    
void PaintSurfaceImpl::resize(const Pt::Gfx::SizeF& size)
{
	_size = size;
    
    if( _size.width() ==  0)
        _size.setWidth(20);
    
    if( _size.height() ==  0)
        _size.setHeight(20);
    
    destroy();
    create();
}


Pt::Gfx::ARgbImage PaintSurfaceImpl::toImage()
{
	//TODO:
	Pt::Gfx::ARgbImage image(800,600);
	return image;
}

}}
