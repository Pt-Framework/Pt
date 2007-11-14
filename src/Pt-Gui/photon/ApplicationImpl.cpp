/***************************************************************************
 *   Copyright (C) 2006 Marc Boris Duerner                                 *
 *   Copyright (C) 2006 Aloysius Indrayanto                                *
 *   Copyright (C) 2006 Sebastian Pieck                                    *
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
#include "Pt/Gui/Widget.h"
#include "Pt/Gui/MouseEvent.h"
#include "Pt/Gui/MouseMoveEvent.h"
#include <iostream>
#include <cerrno>
#include <stdexcept>
#include <cstdlib>
#include <cerrno>
#include <sys/neutrino.h>
#include <sys/select.h>

namespace Pt {

namespace Gui {

EventLoopImpl::EventLoopImpl()
: _app(0)
{
	if( PtInit(NULL) == -1 )
		PtExit(EXIT_FAILURE);
}

void EventLoopImpl::setApp(Application& app)
{
	_app = &app;
}
			
int EventLoopImpl::run()
{
	PhEventArm();
	PtMainLoop();
	return 0;
}
			

void EventLoopImpl::exit()
{
	PtExit(EXIT_SUCCESS);
}			


int EventLoopImpl::photonEvent(PtWidget_t* pw, void* data, PtCallbackInfo_t* info)
{
	Widget* widget = reinterpret_cast<Widget*>(data);
std::cerr << widget << std::endl;
	switch(info->event->type)
	{
		case Ph_EV_PTR_MOTION_BUTTON:
		case Ph_EV_PTR_MOTION_NOBUTTON:
		{
			PhPointerEvent_t* pev = (PhPointerEvent_t*) PhGetData(info->event);
			Pt::Gui::MouseMoveEvent ev(*widget, pev->pos.x, pev->pos.y, Pt::Gui::MouseMoveEvent::Moved, 0);
			EventLoopImpl::instance()._app->event(ev);
			printf("move\n");
			break;
		}
		case Ph_EV_BUT_PRESS:
		{
			printf("press\n");
			break;
		}
		case Ph_EV_BUT_RELEASE:
		{
			printf("release\n");
			break;
		}
	}
	
	//EventLoopImpl::instance()._app->event;
	return 0;
}


ApplicationImpl::ApplicationImpl(Application& app)
{
	EventLoopImpl::instance().setApp(app);
}


ApplicationImpl::~ApplicationImpl()
{

}


void ApplicationImpl::commitEvent(const Pt::Event& event)
{

}


void ApplicationImpl::queueEvent(const Pt::Event& event)
{

}


void ApplicationImpl::processEvents()
{

}


int ApplicationImpl::run()
{  
	return EventLoopImpl::instance().run();
}


void ApplicationImpl::wake()
{

}


void ApplicationImpl::exit()
{
	EventLoopImpl::instance().exit();
}

} // namespace Gui

} // namespace Pt

