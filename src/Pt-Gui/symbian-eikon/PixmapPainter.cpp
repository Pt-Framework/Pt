/***************************************************************************
 *   Copyright (C) 2008 Marc Boris Duerner                                 *
 *   Copyright (C) 2008 Peter Barth                                        *
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
#include "PixmapPainter.h"
#include "PixmapImpl.h"

namespace Pt {

namespace Gui {

PixmapPainter::PixmapPainter(PixmapImpl& parentPixmapImpl)
: _parentPixmapImpl(parentPixmapImpl), _active(false)
{
}


PixmapPainter::~PixmapPainter()
{
    if (_active)
        end(); 
}


void PixmapPainter::begin()
{
    if (!_active)
    {
        _parentPixmapImpl.beginDraw();
        _active = true;
    }
    
    PainterImpl::begin();
}


void PixmapPainter::end()
{
    PainterImpl::end();
    
    if (_active)
    {
        _parentPixmapImpl.endDraw();        
        _active = false;
    }
}

void PixmapPainter::cleanUp()
{
    end();
}

} // namespace Gui

} // namespace Pt

