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

#include "Pt/Gfx/Point.h"
#include "Pt/Gfx/Size.h"
#include "Pt/Gfx/Rect.h"
#include "Pt/Gui/Button.h"
#include "Pt/Gui/CloseEvent.h"
#include "Pt/Gui/MouseEvent.h"
#include "Pt/Gui/MoveEvent.h"
#include "Pt/Gui/PaintEvent.h"
#include "Pt/Gui/ResizeEvent.h"
#include "Pt/Gui/KeyEvent.h"
#include "Pt/Gui/Pixmap.h"
#include "Pt/Gfx/Pen.h"
#include "Pt/Gfx/Brush.h"


#include <iostream>

using namespace Pt::Gfx;
using namespace std;


namespace Pt {

namespace Gui {


Button::Button(Widget& parent, const Gfx::Point& at, const Gfx::Size& size)
: Widget(parent, at, size),
 _backbuffer( new Pixmap( size.width(), size.height() ) )
{
	
}


void Button::setText(const std::string& text)
{
	_text = text;
	this->update();
}


const std::string& Button::text() const
{
	return _text;
}


void Button::update()
{
	Painter& widgetPainter = getPainter();
	Painter& backbufferPainter = _backbuffer->getPainter();

	// draw backround
	Brush brush(Brush::SolidFill, this->backgroundColor());
	Pen pen(4, this->foregroundColor());

	widgetPainter.setBrush(brush);
	backbufferPainter.setBrush(brush);

	widgetPainter.fillRect( Rect( Point(0, 0), size() ) );
	backbufferPainter.fillRect( Rect( Point(0, 0), _backbuffer->size() ) );

	widgetPainter.setPen(pen);

	// TODO Replace with drawRect
	widgetPainter.drawLine(Point(0, 0), Point(size().width(), 0));
	widgetPainter.drawLine(Point(size().width(), 0), Point(size().width(), size().height()));
	widgetPainter.drawLine(Point(size().width(), size().height()), Point(0, size().height()));
	widgetPainter.drawLine(Point(0, size().height()), Point(0, 0));

	backbufferPainter.setPen(pen);

	// TODO Replace with drawRect
	backbufferPainter.drawLine(Point(0, 0), Point(size().width(), 0));
	backbufferPainter.drawLine(Point(size().width(), 0), Point(size().width(), size().height()));
	backbufferPainter.drawLine(Point(size().width(), size().height()), Point(0, size().height()));
	backbufferPainter.drawLine(Point(0, size().height()), Point(0, 0));


	if( !_text.empty() ) {
		widgetPainter.drawText( Point(7, 16), _text.c_str() );
		backbufferPainter.drawText( Point(7, 16), _text.c_str() );
	}
}


void Button::_resizeEvent(const ResizeEvent& event)
{
	_backbuffer.reset( new Pixmap( event.width(), event.height() ) );

	this->update();
}


void Button::_paintEvent(const PaintEvent& event)
{
	getPainter().drawPixmap( event.origin(), *_backbuffer, event.rect() );
}


void Button::_mouseEvent(const MouseEvent& event)
{
	if( event.button() == MouseEvent::LeftButton )
	{
		clicked.send();
	}
}


} // namespace Gui

} // namespace Pt
