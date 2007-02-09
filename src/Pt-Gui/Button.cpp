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

#include "Pt/Math/Point.h"
#include "Pt/Math/Size.h"
#include "Pt/Math/Rect.h"
#include "Pt/Gui/Button.h"
#include <Pt/Gui/Painter.h>
#include "Pt/Gui/CloseEvent.h"
#include "Pt/Gui/MouseEvent.h"
#include "Pt/Gui/MouseMoveEvent.h"
#include "Pt/Gui/MoveEvent.h"
#include "Pt/Gui/PaintEvent.h"
#include "Pt/Gui/Painter.h"
#include "Pt/Gui/ResizeEvent.h"
#include "Pt/Gui/KeyEvent.h"
#include "Pt/Gui/Pixmap.h"
#include "Pt/Gfx/Pen.h"
#include "Pt/Gfx/Brush.h"
#include "Pt/Gfx/Font.h"
#include "Pt/Gfx/FontMetrics.h"

#include <iostream>

using namespace Pt::Gfx;
using namespace std;


namespace Pt {

namespace Gui {


Button::Button(Widget& parent, const Math::Point& at, const Math::Size& size, const Pt::String& text)
: Widget(parent, at, size)
, _pressed(false)
, _backbuffer(new Pixmap(size.width(), size.height()))
, _text(text)
{
	setForegroundColor(ARgbColor(0, 0, 0));
	setBackgroundColor(ARgbColor(54272, 53248, 51200));
	setInsets(Insets(3, 5, 3, 5));
}


Button::~Button()
{
}


void Button::setText(const Pt::String& text)
{
	_text = text;
	this->update();
}


const Pt::String& Button::text() const
{
	return _text;
}


void Button::update()
{
	Painter widgetPainter = painter();
	Painter backbufferPainter = _backbuffer->painter();

	if (_pressed) {
		drawPressed(widgetPainter);
		drawPressed(backbufferPainter);
	} else {
		drawNormal(widgetPainter, false);
		drawNormal(backbufferPainter, false);
	}
}


void Button::drawPressed(Painter& painter)
{
	// Draw backround and border. First paint the background in the button's background color...
	Brush brush(this->backgroundColor());

	painter.setBrush(brush);
	painter.fillRect( Math::Rect( Math::Point(0, 0), size() ) );

	// ... then draw the border around the button.
	Pen borderPen(1, ARgbColor(16384, 16384, 16384));

	painter.setPen(borderPen);
	painter.drawLine(Math::Point(0, 0), Math::Point(size().width() - 1, 0));
	painter.drawLine(Math::Point(0, 0), Math::Point(0, size().height() - 1));

	borderPen = Pen(1, ARgbColor(32768, 32768, 32768));
	painter.setPen(borderPen);

	painter.drawLine(Math::Point(1, 1), Math::Point(size().width() - 2, 1));
	painter.drawLine(Math::Point(1, 1), Math::Point(1, size().height() - 2));

	borderPen = Pen(1, ARgbColor(65535, 65535, 65535));
	painter.setPen(borderPen);

	painter.drawLine(Math::Point(size().width() - 1, 0), Math::Point(size().width() - 1, size().height()));
	painter.drawLine(Math::Point(0, size().height() - 1), Math::Point(size().width(), size().height() - 1));

	// Draw button's text.
	Pen pen(4, this->foregroundColor());
	painter.setFont(Font("Tahoma", 11, Font::NormalStyle)); // TODO Font name
	painter.setPen(pen);

	if( !_text.empty() ) {
		// Calculate the position of the text to center it inside the button.
		FontMetrics metrics = painter.fontMetrics(_text);
		ssize_t x = (size().width()  - metrics.width())  / 2;
		ssize_t y = (size().height() - metrics.height()) / 2 + metrics.ascent();

		// Draw the button's text.
		painter.drawText( Math::Point(x + 1, y + 1), _text.c_str() );
	}
}


void Button::drawNormal(Painter& painter, bool focused)
{
	// Draw backround and border. First paint the background in the button's background color...
	Brush brush(this->backgroundColor());

	painter.setBrush(brush);
	painter.fillRect( Math::Rect( Math::Point(0, 0), size() ) );

	// ... then draw the border around the button.
	Pen borderPen(1, ARgbColor(65535, 65535, 65535));

	painter.setPen(borderPen);
	painter.drawLine(Math::Point(0, 0), Math::Point(size().width() - 1, 0));
	painter.drawLine(Math::Point(0, 0), Math::Point(0, size().height() - 1));

	borderPen = Pen(1, ARgbColor(16384, 16384, 16384));
	painter.setPen(borderPen);

	painter.drawLine(Math::Point(size().width() - 1, 0), Math::Point(size().width() - 1, size().height()));
	painter.drawLine(Math::Point(0, size().height() - 1), Math::Point(size().width(), size().height() - 1));

	borderPen = Pen(1, ARgbColor(32768, 32768, 32768));
	painter.setPen(borderPen);

	painter.drawLine(Math::Point(size().width() - 2, 1), Math::Point(size().width() - 2, size().height() - 1));
	painter.drawLine(Math::Point(1, size().height() - 2), Math::Point(size().width() - 1, size().height() - 2));

	// Draw button's text.
	Pen pen(4, this->foregroundColor());
	painter.setFont(Font("Tahoma", 11, Font::NormalStyle)); // TODO Font name
	painter.setPen(pen);

	if( !_text.empty() ) {
		// Calculate the position of the text to center it inside the button.
		FontMetrics metrics = painter.fontMetrics(_text);
		ssize_t x = (size().width()  - metrics.width())  / 2;
		ssize_t y = (size().height() - metrics.height()) / 2 + metrics.ascent();

		// Draw the button's text.
		painter.drawText( Math::Point(x, y), _text.c_str() );
	}
}



Math::Size Button::minimumSize()
{
	return Math::Size(0, 0);
}


Math::Size Button::preferredSize()
{
	FontMetrics metrics = painter().fontMetrics(_text);

	return Math::Size(metrics.width()  + insets().left() + insets().right(),
	            metrics.height() + insets().top()  + insets().bottom());
}


void Button::_resizeEvent(const ResizeEvent& event)
{
	_backbuffer.reset( new Pixmap( event.width(), event.height() ) );

	Painter backbufferPainter = _backbuffer->painter();
	this->update();
}


void Button::_paintEvent(const PaintEvent& event)
{
	painter().drawPixmap( event.origin(), *_backbuffer, event.region() );
}


void Button::_mouseEvent(const MouseEvent& event)
{
	if (event.action() == MouseEvent::Press && event.button() == MouseEvent::LeftButton )
	{
		_pressed = true;
		this->update();
	}
	else if (event.action() == MouseEvent::Release && event.button() == MouseEvent::LeftButton && _pressed)
	{
		_pressed = false;
		this->update();
		clicked.send();
	}
}

void Button::_mouseMoveEvent(const MouseMoveEvent& event)
{
	if (_pressed && event.action() == MouseMoveEvent::Exited) {
		_pressed = false;
		this->update();
	}
}


} // namespace Gui

} // namespace Pt
