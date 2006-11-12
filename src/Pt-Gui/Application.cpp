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
#include "Pt/Gui/Event.h"
#include "Pt/Gui/Application.h"
#include "Pt/Gui/PaintEvent.h"
#include "Pt/Gui/CloseEvent.h"
#include "Pt/Gui/ResizeEvent.h"
#include "Pt/Gui/MoveEvent.h"
#include "Pt/Gui/MouseEvent.h"
#include "Pt/Gui/MouseMoveEvent.h"
#include "Pt/Gui/KeyEvent.h"
#include "Pt/Gui/Widget.h"

using namespace std;


namespace Pt {

namespace Gui {


Application::Application()
{
	_impl = new ApplicationImpl(*this);
	connect(event, *this, &Application::dispatchEvent);
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
	const Pt::Gui::Event* guiEvent = dynamic_cast<const Pt::Gui::Event*>(&e);

	if (guiEvent) {
		guiEvent->widget().event(*guiEvent);
	}
}

} // namespace Gui

} // namespace Pt
