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
#include "ApplicationImpl.h"

#include "Pt/Signal.h"
#include "Pt/Gui/Application.h"
#include "Pt/Gui/PaintEvent.h"
#include "Pt/Gui/CloseEvent.h"
#include "Pt/Gui/ResizeEvent.h"
#include "Pt/Gui/MoveEvent.h"
#include "Pt/Gui/MouseEvent.h"
#include "Pt/Gui/MouseMoveEvent.h"
#include "Pt/Gui/KeyEvent.h"
#include "Pt/Gui/Widget.h"

#include <string>
#include <iostream>
#include <typeinfo>
using namespace std;


namespace Pt {

namespace Gui {

const type_info& Application::TYPE_CLOSE_EVENT     = typeid(CloseEvent);
const type_info& Application::TYPE_MOUSE_EVENT     = typeid(MouseEvent);
const type_info& Application::TYPE_KEY_EVENT       = typeid(KeyEvent);
const type_info& Application::TYPE_MOVE_EVENT      = typeid(MoveEvent);
const type_info& Application::TYPE_RESIZE_EVENT    = typeid(ResizeEvent);
const type_info& Application::TYPE_PAINT_EVENT     = typeid(PaintEvent);
const type_info& Application::TYPE_MOUSEMOVE_EVENT = typeid(MouseMoveEvent);


Application::Application()
{
	_impl = new ApplicationImpl(*this);
	connect(_impl->event, slot(this->event) );

	connect(this->event, *this, &Application::dispatchEvent);
}


Application::~Application()
{
	delete _impl;
}


ApplicationImpl& Application::impl()
{
	return *_impl;
}


int Application::run()
{
	return _impl->run();
}

void Application::exit()
{
	_impl->exit();
}


void Application::commitEvent(const Pt::Event& event)
{
	_impl->commitEvent(event);
}


void Application::queueEvent(const Pt::Event& event)
{
	_impl->queueEvent(event);
}


void Application::processEvents()
{
	_impl->processEvents();
}


void Application::dispatchEvent(const Pt::Event& e) const
{
	const type_info& typeInfo = typeid(e);

	if( typeInfo == TYPE_CLOSE_EVENT ) {
		const CloseEvent& ev = (const CloseEvent&)(e);
		ev.widget().closeEvent( ev );
	}
	else if( typeInfo == TYPE_MOUSE_EVENT ) {
		const MouseEvent& ev = (const MouseEvent&)(e);
		ev.widget().mouseEvent( ev );
	}
	else if( typeInfo == TYPE_KEY_EVENT ) {
		const KeyEvent& ev = (const KeyEvent&)(e);
		ev.widget().keyEvent( ev );
	}
	else if( typeInfo == TYPE_MOVE_EVENT ) {
		const MoveEvent& ev = (const MoveEvent&)(e);
		ev.widget().moveEvent( ev );
	}
	else if( typeInfo == TYPE_MOUSEMOVE_EVENT ) {
		const MouseMoveEvent& ev = (const MouseMoveEvent&)(e);
		ev.widget().mouseMoveEvent( ev );
	}
	else if( typeInfo == TYPE_RESIZE_EVENT ) {
		const ResizeEvent& ev = (const ResizeEvent&)(e);
		ev.widget().resizeEvent( ev );
	}
	else if( typeInfo == TYPE_PAINT_EVENT ) {
		const PaintEvent& ev = (const PaintEvent&)(e);
		ev.widget().paintEvent( ev );
	}
}

} // namespace Gui

} // namespace Pt
