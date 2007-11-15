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
#include "Pt/Gui/Application.h"
#include "Pt/Gui/Widget.h"
#include "Pt/Gui/MouseEvent.h"
#include "Pt/Gui/MouseMoveEvent.h"
#include "Pt/Gui/ResizeEvent.h"
#include "Pt/Gui/MoveEvent.h"
#include "Pt/Gui/PaintEvent.h"
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


void EventLoopImpl::pointerMotion(Pt::Gui::Widget& widget, PhPointerEvent_t& pev)
{
	MouseMoveEvent::Modifier mod = MouseMoveEvent::NoButton;
	
	if(pev.button_state & Ph_BUTTON_SELECT)
		mod = MouseMoveEvent::LeftButtonDown;
	else if(pev.button_state & Ph_BUTTON_MENU)
		mod = MouseMoveEvent::RightButtonDown;
	else if(pev.button_state & Ph_BUTTON_ADJUST)
		mod = MouseMoveEvent::MiddleButtonDown;

	MouseMoveEvent ev(widget, 
	                                  pev.pos.x, 
	                                  pev.pos.y, 
	                                  MouseMoveEvent::Moved, 
	                                  mod);
	                                  
	//std::cerr << pev.pos.x << " " << pev.pos.y << std::endl;
	_app->event(ev);
}


void EventLoopImpl::buttonPress(Pt::Gui::Widget& widget, PhPointerEvent_t& pev)
{
	MouseEvent::Button button = MouseEvent::LeftButton;
	
	if(pev.buttons & Ph_BUTTON_SELECT)
		button = MouseEvent::LeftButton;
	else if(pev.buttons & Ph_BUTTON_MENU)
		button = MouseEvent::RightButton;
	else if(pev.buttons & Ph_BUTTON_ADJUST)
		button = MouseEvent::MiddleButton;

	Pt::Gui::MouseEvent ev(widget, 
	                                     pev.pos.x, 
	                                     pev.pos.y,
	                                     button,
	                                     MouseEvent::Press,
	                                     0);
	                                     
	 //std::cerr << pev.pos.x << " " << pev.pos.y << std::endl;
	_app->event(ev);
}


void EventLoopImpl::buttonRelease(Pt::Gui::Widget& widget, PhPointerEvent_t& pev)
{
	MouseEvent::Button button = MouseEvent::LeftButton;
	
	if(pev.buttons & Ph_BUTTON_SELECT)
		button = MouseEvent::LeftButton;
	else if(pev.buttons & Ph_BUTTON_MENU)
		button = MouseEvent::RightButton;
	else if(pev.buttons & Ph_BUTTON_ADJUST)
		button = MouseEvent::MiddleButton;

	Pt::Gui::MouseEvent ev(widget, 
	                                     pev.pos.x, 
	                                     pev.pos.y,
	                                     button,
	                                     MouseEvent::Release,
	                                     0);
	_app->event(ev);
}


void EventLoopImpl::exposeEvent(Pt::Gui::Widget& widget, PhEvent_t& ev)
{
	PhRect_t* rect = PhGetRects(&ev);
	
	for(int n = 0; n < ev.num_rects;++n, ++rect)
	{
		PaintEvent pev(widget, 
		                         Math::Point( rect->ul.x, rect->ul.y), 
		                         Math::Size( rect->lr.x - rect->ul.x + 1, rect->lr.y - rect->ul.y + 1) );


		std::cerr << rect->ul.x << " " << rect->ul.y << " " << (rect->lr.x - rect->ul.x + 1) << " " <<(rect->lr.y - rect->ul.y + 1) << std::endl;
		_app->event(pev);
	}
}


void EventLoopImpl::windowEvent(Pt::Gui::Widget& widget, PhWindowEvent_t& ev)
{
}


int EventLoopImpl::photonEvent(PtWidget_t* pw, void* data, PtCallbackInfo_t* info)
{
	Widget* widget = reinterpret_cast<Widget*>(data);

	switch(info->event->type)
	{
		case Ph_EV_PTR_MOTION_BUTTON:
		case Ph_EV_PTR_MOTION_NOBUTTON:
		{
			PhPointerEvent_t* pev = (PhPointerEvent_t*) PhGetData(info->event);
			EventLoopImpl::instance().pointerMotion(*widget, *pev);
			break;
		}
		case Ph_EV_BUT_PRESS:
		{
			PhPointerEvent_t* pev = (PhPointerEvent_t*) PhGetData(info->event);
			EventLoopImpl::instance().buttonPress(*widget, *pev);
			break;
		}
		case Ph_EV_BUT_RELEASE:
		{
			PhPointerEvent_t* pev = (PhPointerEvent_t*) PhGetData(info->event);
			EventLoopImpl::instance().buttonRelease(*widget, *pev);
			break;
		}
		case Ph_EV_EXPOSE:
		{
			EventLoopImpl::instance().exposeEvent(*widget, *(info->event));
			break;
		}
		case Ph_EV_WM:
		{
			PhWindowEvent_t* wev = (PhWindowEvent_t*) PhGetData(info->event);
			EventLoopImpl::instance().windowEvent(*widget, *wev);
			break;
		}
		//default:
		//	std::cerr << "Unknown event" << std::endl;
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

