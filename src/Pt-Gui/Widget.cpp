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

#include "Pt/Gui/Widget.h"
#include "Pt/Gui/Application.h"
#include "Pt/Gui/CloseEvent.h"
#include "Pt/Gui/MouseEvent.h"
#include "Pt/Gui/MoveEvent.h"
#include "Pt/Gui/PaintEvent.h"
#include "Pt/Gui/ResizeEvent.h"
#include "Pt/Gui/KeyEvent.h"

#include "WidgetImpl.h"
#include "WidgetPainterImpl.h"

#include <string>
#include <iostream>
#include <typeinfo>

using namespace Pt::Gfx;
using namespace std;


namespace Pt {

namespace Gui {


Widget::Widget(Widget& parent, const Gfx::Point& at, const Gfx::Size& size)
: _foregroundColor( ARgbColor(0, 0, 0) ),
  _backgroundColor( ARgbColor(1, 1, 1) )
{
	_impl = new WidgetImpl( *this, &parent, at, size );
}


Widget::Widget(const Gfx::Point& at, const Gfx::Size& size)
: _foregroundColor( ARgbColor(0, 0, 0) ),
  _backgroundColor( ARgbColor(1, 1, 1) )
{
	_impl = new WidgetImpl( *this, 0, at, size );
}


Widget::~Widget()
{
	delete _impl;
}


void Widget::setTitle(const char* text)
{
	_impl->setTitle(text);
}

std::string Widget::title()
{
	return _impl->title();
}


const Gfx::ARgbColor& Widget::backgroundColor() const
{
	return _backgroundColor;
}


void Widget::setBackgroundColor(const Gfx::ARgbColor& color)
{
	_backgroundColor = color;
	this->update();
}


const Gfx::ARgbColor& Widget::foregroundColor() const
{
	return _foregroundColor;
}


void Widget::setForegroundColor(const Gfx::ARgbColor& color)
{
	_foregroundColor = color;
	this->update();
}


const Gfx::Rect& Widget::rect() const
{
	return _impl->rect();
}

const Gfx::Size& Widget::size() const
{
	return rect().size();
}

void Widget::move(size_t x, size_t y)
{
	_impl->move(x, y);
}


void Widget::resize(size_t width, size_t height)
{
	_impl->resize(width, height);
}


void Widget::show()
{
	_impl->show();
}


void Widget::hide()
{
	_impl->hide();
}


Gui::Painter& Widget::getPainter()
{
	return _impl->getPainter();
}


void Widget::closeEvent(const CloseEvent& event)
{
	//std::clog << "Widget::closeEvent" << std::endl;
	this->_closeEvent(event);
}


void Widget::mouseEvent(const MouseEvent& event)
{
	//std::clog << "[" << this << "] Widget::mouseEvent" << std::endl;
	this->_mouseEvent(event);
}


void Widget::mouseMoveEvent(const MouseMoveEvent& event)
{
	//std::clog << "[" << this << "] Widget::mouseEvent" << std::endl;
	this->_mouseMoveEvent(event);
}



void Widget::moveEvent(const MoveEvent& event)
{
	//std::clog << "[" << this << "] Widget::moveEvent" << std::endl;
	this->_moveEvent(event);
}


void Widget::paintEvent(const PaintEvent& event)
{
	//std::clog << "[" << this << "] Widget::paintEvent" << std::endl;
	this->_paintEvent(event);
}


void Widget::resizeEvent(const ResizeEvent& event)
{
	//std::clog << "[" << this << "] Widget::resizeEvent" << std::endl;

	// we have be notified that our size has changed, so update
	// the private members in WidgetImpl
	_impl->resizeEvent(event);
	this->_resizeEvent(event);
}


void Widget::keyEvent(const KeyEvent& event)
{
	this->_keyEvent(event);
}


void Widget::_closeEvent(const CloseEvent& event)
{
	//std::clog << "Widget::_closeEvent" << std::endl;
	this->hide();
	closed.send();
}


void Widget::_mouseEvent(const MouseEvent& event)
{
	//std::clog << "[" << this << "] Widget::_mouseEvent" << std::endl;
}


void Widget::_mouseMoveEvent(const MouseMoveEvent& event)
{
	//std::clog << "[" << this << "] Widget::_mouseMoveEvent" << std::endl;
}


void Widget::_moveEvent(const MoveEvent& event)
{
	//std::clog << "[" << this << "] Widget::_moveEvent" << std::endl;
}


void Widget::_paintEvent(const PaintEvent& event)
{
	//std::clog << "[" << this << "] Widget::_paintEvent" << std::endl;
}


void Widget::_resizeEvent(const ResizeEvent& event)
{
	//std::clog << "[" << this << "] Widget::_resizeEvent" << std::endl;
}


void Widget::_keyEvent(const KeyEvent& event)
{
	//std::clog << "[" << this << "] Widget::_keyEvent" << std::endl;
	//std::clog << "[" << this << "] text: " << event.text() << std::endl;
	//std::clog << "[" << this << "] code: " << event.code() << std::endl;
}


} // namespace gui

} // namespace ptv
