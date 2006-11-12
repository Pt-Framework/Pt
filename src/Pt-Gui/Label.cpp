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

#include "Pt/Gfx/Brush.h"
#include "Pt/Gfx/Pen.h"
#include "Pt/Gfx/FontMetrics.h"
#include "Pt/Gfx/Point.h"
#include "Pt/Gfx/Size.h"
#include "Pt/Gfx/Rect.h"
#include "Pt/Gui/Label.h"
#include "Pt/Gui/MouseEvent.h"
#include "Pt/Gui/PaintEvent.h"
#include "Pt/Gui/Painter.h"
#include "Pt/Gui/ResizeEvent.h"
#include "Pt/Gui/Pixmap.h"

#include <iostream>

using namespace Pt::Gfx;
using namespace std;


namespace Pt {

namespace Gui {


Label::Label(Widget& parent, const Gfx::Point& at, const Gfx::Size& size, const std::string& text)
: Widget(parent, at, size)
, _backbuffer(new Pixmap(size.width(), size.height()))
, _text(text)
{
}


Label::~Label()
{
}


void Label::setText(const std::string& text)
{
	_text = text;
	this->update();
}


const std::string& Label::text() const
{
	return _text;
}


void Label::update()
{
	Painter widgetPainter     = painter();
	Painter backbufferPainter = _backbuffer->painter();

	Brush brush(backgroundColor());

	widgetPainter.setBrush(brush);
	widgetPainter.fillRect(Rect(Point(0, 0), size()));

	backbufferPainter.setBrush(brush);
	backbufferPainter.fillRect(Rect(Point(0, 0), size()));

	if( !_text.empty() ) {
		Pen pen(1, foregroundColor());
		widgetPainter.setPen(pen);
		backbufferPainter.setPen(pen);

		widgetPainter.drawText( Point(0, widgetPainter.fontMetrics().ascent()), _text.c_str() );
		backbufferPainter.drawText( Point(0, widgetPainter.fontMetrics().ascent()), _text.c_str() );
	}
}


Size Label::minimumSize()
{
	return Size(0, 0);
}


Size Label::preferredSize()
{
	FontMetrics metrics = painter().fontMetrics(_text);
	return Size(metrics.width(), metrics.height());
}


void Label::_resizeEvent(const ResizeEvent& event)
{
	_backbuffer.reset(new Pixmap(event.width(), event.height()));

	this->update();
}


void Label::_paintEvent(const PaintEvent& event)
{
	painter().drawPixmap( event.origin(), *_backbuffer, event.rect() );
}


void Label::_mouseEvent(const MouseEvent& event)
{
	if( event.button() == MouseEvent::LeftButton )
	{
		clicked.send();
	}
}


} // namespace Gui

} // namespace Pt
