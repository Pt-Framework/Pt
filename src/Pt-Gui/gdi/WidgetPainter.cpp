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

#include "WidgetPainter.h"
#include "WidgetImpl.h"

#include <windows.h>
#include <iostream>

namespace Pt {

namespace Gui {


WidgetPainter::WidgetPainter(WidgetImpl& widgetImpl)
: PainterImpl(widgetImpl)
, _widgetImpl(widgetImpl)
{
}


WidgetPainter::~WidgetPainter()
{
}


void WidgetPainter::begin()
{
	bool firstUse = !_widgetImpl.isPainting();

	_widgetImpl.beginPaint();

	if (firstUse) {
		// Send default settings to GDI.
		updatePen();
		updateFont();
		updateBrush();

		// Initialize default Device Context settings.
		SetBkMode(_widgetImpl.deviceContext(), TRANSPARENT);
	}
}


void WidgetPainter::end()
{
	_widgetImpl.endPaint();
}

} // namespace Gui

} // namespace Pt

