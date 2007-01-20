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

#include "Pt/Math/Point.h"
#include "Pt/Math/Size.h"
#include "Pt/Math/Rect.h"
#include "Pt/Gui/ImageButton.h"
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
using namespace Pt::Math;
using namespace std;


namespace Pt {

namespace Gui {


ImageButton::ImageButton(Widget& parent, const  Pt::Math::Point& at, const  Pt::Math::Size& size, const Pt::Gfx::ARgbImage& image)
: Widget(parent, at, size)
, _pressed(false)
, _backbuffer(new Pixmap(size.width(), size.height()))
, _image(image)
{
	setForegroundColor(ARgbColor(0, 0, 0));
	setBackgroundColor(ARgbColor(0xffff, 0xffff, 0xffff));
}


ImageButton::~ImageButton()
{
}

void ImageButton::update()
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


void ImageButton::drawPressed(Painter& painter)
{
	painter.setBrush(Brush(backgroundColor()));
	painter.fillRect(Rect(Point(0, 0), this->size()));

	ssize_t x = ((ssize_t)this->size().width()  - (ssize_t)_image.width())  / 2;
	ssize_t y = ((ssize_t)this->size().height() - (ssize_t)_image.height()) / 2;
	painter.drawImage(Point(x + 1, y + 1), _image);
}


void ImageButton::drawNormal(Painter& painter, bool focused)
{
	painter.setBrush(Brush(backgroundColor()));
	painter.fillRect(Rect(Point(0, 0), this->size()));

	ssize_t x = ((ssize_t)this->size().width()  - (ssize_t)_image.width())  / 2;
	ssize_t y = ((ssize_t)this->size().height() - (ssize_t)_image.height()) / 2;
	painter.drawImage(Point(x, y), _image);
}



Pt::Math::Size ImageButton::minimumSize()
{
	return Size(_image.width(), _image.height());
}


Pt::Math::Size ImageButton::preferredSize()
{
	return Pt::Math::Size(_image.width() + insets().left() + insets().right(),
	                       _image.height() + insets().top()  + insets().bottom());
}


void ImageButton::_resizeEvent(const ResizeEvent& event)
{
	_backbuffer.reset( new Pixmap( event.width(), event.height() ) );

	Painter backbufferPainter = _backbuffer->painter();
	this->update();
}


void ImageButton::_paintEvent(const PaintEvent& event)
{
	painter().drawPixmap( event.origin(), *_backbuffer, event.region() );
}


void ImageButton::_mouseEvent(const MouseEvent& event)
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

void ImageButton::_mouseMoveEvent(const MouseMoveEvent& event)
{
	if (_pressed && event.action() == MouseMoveEvent::Exited) {
		_pressed = false;
		this->update();
	}
}


} // namespace Gui

} // namespace Pt
