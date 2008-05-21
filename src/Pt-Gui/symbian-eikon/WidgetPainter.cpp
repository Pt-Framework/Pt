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
#include "WidgetPainter.h"
#include "WidgetImpl.h"
#include <assert.h>

// symbian APIs
#include <coecntrl.h>

namespace Pt {

namespace Gui {

WidgetPainter::WidgetPainter(WidgetImpl& parentWidgetImpl)
: _parentWidgetImpl(parentWidgetImpl), _active(false)
{
}

WidgetPainter::~WidgetPainter()
{
}

void WidgetPainter::begin()
{
    assert(_gc != 0);
    _active = true;    
}

void WidgetPainter::end()
{
    _active = false;    
}

} // namespace Gui

} // namespace Pt
