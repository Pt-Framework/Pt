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


void EventLoopImpl::pointerMotion(Pt::Gui::Widget& widget, PhEvent_t& ev)
{
	PhPointerEvent_t* pev = (PhPointerEvent_t*) PhGetData(&ev);

	MouseMoveEvent::Modifier mod = MouseMoveEvent::NoButton;
	if(pev->button_state & Ph_BUTTON_SELECT)
		mod = MouseMoveEvent::LeftButtonDown;
	else if(pev->button_state & Ph_BUTTON_MENU)
		mod = MouseMoveEvent::RightButtonDown;
	else if(pev->button_state & Ph_BUTTON_ADJUST)
		mod = MouseMoveEvent::MiddleButtonDown;

	MouseMoveEvent::Action action = MouseMoveEvent::Moved;
	if(ev.type == Ph_EV_BOUNDARY)
	{
		if(ev.subtype == Ph_EV_PTR_ENTER_FROM_PARENT ||
		   ev.subtype == Ph_EV_PTR_ENTER_FROM_CHILD)
		{
			action = MouseMoveEvent::Entered;
		}
		else if(ev.subtype == Ph_EV_PTR_LEAVE_TO_PARENT ||
		           ev.subtype == Ph_EV_PTR_LEAVE_TO_CHILD)
		{
			action = MouseMoveEvent::Exited;
		}
	}

	MouseMoveEvent mev(widget, 
	                                    pev->pos.x + ev.translation.x, 
	                                    pev->pos.y + ev.translation.y, 
	                                    action, 
	                                    mod);
	                                  
	// std::cerr << mev.x() << " " << mev.y() << std::endl;
	_app->event(mev);
}


void EventLoopImpl::buttonPress(Pt::Gui::Widget& widget, PhEvent_t& ev)
{
	PhPointerEvent_t* pev = (PhPointerEvent_t*) PhGetData(&ev);
	
	MouseEvent::Button button = MouseEvent::LeftButton;
	if(pev->buttons & Ph_BUTTON_SELECT)
		button = MouseEvent::LeftButton;
	else if(pev->buttons & Ph_BUTTON_MENU)
		button = MouseEvent::RightButton;
	else if(pev->buttons & Ph_BUTTON_ADJUST)
		button = MouseEvent::MiddleButton;

	MouseEvent::Action action = MouseEvent::Press;
	if(pev->click_count >= 2)
		action = MouseEvent::DoubleClick;

	Pt::Gui::MouseEvent mev(widget, 
	                                       pev->pos.x + ev.translation.x, 
	                                       pev->pos.y + ev.translation.y,
	                                       button,
	                                       action,
	                                       0);
	                                     
	 // std::cerr << pev.pos.x << " " << pev.pos.y << std::endl;
	_app->event(mev);
}


void EventLoopImpl::buttonRelease(Pt::Gui::Widget& widget, PhEvent_t& ev)
{
	PhPointerEvent_t* pev = (PhPointerEvent_t*) PhGetData(&ev);

	MouseEvent::Button button = MouseEvent::LeftButton;
	if(pev->buttons & Ph_BUTTON_SELECT)
		button = MouseEvent::LeftButton;
	else if(pev->buttons & Ph_BUTTON_MENU)
		button = MouseEvent::RightButton;
	else if(pev->buttons & Ph_BUTTON_ADJUST)
		button = MouseEvent::MiddleButton;

	Pt::Gui::MouseEvent mev(widget, 
	                                       pev->pos.x + ev.translation.x, 
	                                       pev->pos.y + ev.translation.y,
	                                       button,
	                                       MouseEvent::Release,
	                                       0);
	_app->event(mev);
}


void EventLoopImpl::exposeEvent(Pt::Gui::Widget& widget, PhEvent_t& ev)
{
	PhRect_t* rect = PhGetRects(&ev);
	
	for(int n = 0; n < ev.num_rects;++n, ++rect)
	{
		PaintEvent pev(widget, 
		                         Math::Point( rect->ul.x, rect->ul.y), 
		                         Math::Size( rect->lr.x - rect->ul.x + 1, rect->lr.y - rect->ul.y + 1) );


		// std::cerr << rect->ul.x << " " << rect->ul.y << " " << (rect->lr.x - rect->ul.x + 1) << " " <<(rect->lr.y - rect->ul.y + 1) << std::endl;
		_app->event(pev);
	}
}


void EventLoopImpl::windowEvent(Pt::Gui::Widget& widget, PhWindowEvent_t& ev)
{
	// TODO: Minimized.

	switch(ev.event_f)
	{
		case Ph_WM_MAX:
		{
			ResizeEvent rev(widget,  ev.size.w,  ev.size.h , ResizeEvent::Maximized);
			_app->event(rev);
			break;
		}
		case Ph_WM_RESTORE:
		{
			ResizeEvent rev(widget,  ev.size.w,  ev.size.h , ResizeEvent::Restored);
			_app->event(rev);
			break;
		}
		case Ph_WM_RESIZE:
		{
			ResizeEvent rev(widget,  ev.size.w,  ev.size.h , ResizeEvent::Resize);
			_app->event(rev);
			// std::cerr << "Resize:" << ev.size.w << " " << ev.size.h << std::endl;
			break;
		}
		case Ph_WM_MOVE:
		{
			MoveEvent mev(widget, ev.pos.x, ev.pos.y);
			_app->event(mev);
			// std::cerr << "Move:" << ev.pos.x << " " << ev.pos.y << std::endl;
			break;
		}
		default: 
			// std::cerr << "WM: " << ev.event_f << std::endl;
			break;
	}
}


int EventLoopImpl::photonEvent(PtWidget_t* pw, void* data, PtCallbackInfo_t* info)
{
	Widget* widget = reinterpret_cast<Widget*>(data);
	PhEvent_t* ev = (PhEvent_t*)(info->event);

	switch(info->event->type)
	{
		case Ph_EV_PTR_MOTION_BUTTON:
		case Ph_EV_PTR_MOTION_NOBUTTON:
		{
			EventLoopImpl::instance().pointerMotion( *widget, *ev );
			break;
		}
		case Ph_EV_BUT_PRESS:
		{
			EventLoopImpl::instance().buttonPress(*widget, *ev);
			break;
		}
		case Ph_EV_BUT_RELEASE:
		{
			EventLoopImpl::instance().buttonRelease(*widget, *ev);
			break;
		}
		case Ph_EV_BOUNDARY:
		{
			EventLoopImpl::instance().pointerMotion(*widget, *ev);
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

