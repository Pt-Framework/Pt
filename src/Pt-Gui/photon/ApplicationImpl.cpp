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


void EventLoopImpl::commitEvent(const Pt::Event& event)
{
	_app->event(event);
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


		//std::cerr << rect->ul.x << " " << rect->ul.y << " " << (rect->lr.x - rect->ul.x + 1) << " " <<(rect->lr.y - rect->ul.y + 1) << std::endl;
		_app->event(pev);
	}
}


void EventLoopImpl::windowEvent(Pt::Gui::Widget& widget, PhWindowEvent_t& ev)
{
	// TODO: Minimized.

	switch(ev.event_f)
	{
		case Ph_WM_MAX:
		{ std::cerr << "WM: " << "Ph_WM_MAX" << std::endl;
			ResizeEvent rev(widget,  ev.size.w,  ev.size.h , ResizeEvent::Maximized);
			_app->event(rev);
			break;
		}
		case Ph_WM_RESTORE:
		{std::cerr << "WM: " << "Ph_WM_RESTORE" << std::endl;
			ResizeEvent rev(widget,  ev.size.w,  ev.size.h , ResizeEvent::Restored);
			_app->event(rev);
			break;
		}
		case Ph_WM_RESIZE:
		{
			//std::cerr << "#Resize:" << ev.size.w << " " << ev.size.h << std::endl;
			//std::cerr << "-Resize:" << widget.size().width() << " " << widget.size().height() << std::endl;
			size_t oldHeight = widget.size().height();
			size_t oldWidth = widget.size().width();
			
			ResizeEvent rev(widget,  ev.size.w,  ev.size.h , ResizeEvent::Resize);
			_app->event(rev);

			if( ev.size.h > oldHeight )
			{
				PaintEvent pev (widget,  Math::Point( 0, oldHeight ), Math::Size( ev.size.w, ev.size.h - oldHeight) );
				_app->event(pev);
			}

			if( ev.size.w > oldWidth )
			{
				PaintEvent pev (widget,  Math::Point( oldWidth, 0 ), Math::Size( ev.size.w-oldWidth, oldHeight) );
				_app->event(pev);
			}

			break;
		}
		case Ph_WM_MOVE:
		{
			MoveEvent mev(widget, ev.pos.x, ev.pos.y);
			_app->event(mev);
			//std::cerr << "Move:" << ev.pos.x << " " << ev.pos.y << std::endl;
			break;
		}
		case Ph_WM_TOFRONT:
		{
			//std::cerr << "WM: " << "Ph_WM_TOFRONT" << std::endl;
			break;
		}
		case Ph_WM_FFRONT:
		{
			//std::cerr << "WM: " <<" Ph_WM_FFRONT" << std::endl;
			break;
		}
		case Ph_WM_FOCUS:
		{
			//std::cerr << "WM: " <<" Ph_WM_FOCUS" << std::endl;
			break;
		}
		case Ph_WM_CLOSE:
		{
			//std::cerr << "WM: " << "Ph_WM_CLOSE" << std::endl;
			break;
		}
		case Ph_WM_MENU:
		{
			//std::cerr << "WM: " <<" Ph_WM_MENU" << std::endl;
			break;
		}
		case Ph_WM_TOBACK:
		{
			//std::cerr << "WM: " <<" Ph_WM_TOBACK" << std::endl;
			break;
		}
		case Ph_WM_CONSWITCH:
		{
			//std::cerr << "WM: " <<" Ph_WM_CONSWITCH" << std::endl;
			break;
		}
		case Ph_WM_HIDE:
		{
			//std::cerr << "WM: " <<" Ph_WM_HIDE" << std::endl;
			break;
		}
		case Ph_WM_BACKDROP:
		{
			//std::cerr << "WM: " <<" Ph_WM_BACKDROP" << std::endl;
			break;
		}
		case Ph_WM_HELP:
		{
			//std::cerr << "WM: " <<" Ph_WM_HELP" << std::endl;
			break;
		}
		case Ph_WM_COLLAPSE:
		{
			//std::cerr << "WM: " <<" Ph_WM_COLLAPSE" << std::endl;
			break;
		}
		case Ph_WM_TASKBAR:
		{
			//std::cerr << "WM: " <<" Ph_WM_TASKBAR" << std::endl;
			break;
		}
		case Ph_WM_NO_FOCUS_LIST:
		{
			//std::cerr << "WM: " <<" Ph_WM_NO_FOCUS_LIST" << std::endl;
			break;
		}
		default: 
			//std::cerr << "WM unknown: " << (unsigned long) ev.event_f<< " - " << ev.size.w << " " <<  ev.size.h << std::endl;
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
		{ //std::cerr << "Ph_EV_PTR_MOTION_NOBUTTON"  << std::endl;
			EventLoopImpl::instance().pointerMotion( *widget, *ev );
			break;
		}
		case Ph_EV_BUT_PRESS:
		{ //std::cerr << "Ph_EV_BUT_PRESS"  << std::endl;
			EventLoopImpl::instance().buttonPress(*widget, *ev);
			break;
		}
		case Ph_EV_BUT_RELEASE:
		{ //std::cerr << "Ph_EV_BUT_RELEASE"  << std::endl;
			EventLoopImpl::instance().buttonRelease(*widget, *ev);
			break;
		}
		case Ph_EV_BOUNDARY:
		{ //std::cerr << "Ph_EV_BOUNDARY"  << std::endl;
			EventLoopImpl::instance().pointerMotion(*widget, *ev);
			break;
		}
		case Ph_EV_EXPOSE:
		{ //std::cerr << "Ph_EV_EXPOSE"  << std::endl;
			EventLoopImpl::instance().exposeEvent(*widget, *(info->event));
			break;
		}
		case Ph_EV_WM:
		{
		    //std::cerr << "Ph_EV_WM"  << std::endl;
			PhWindowEvent_t* wev = (PhWindowEvent_t*) PhGetData(info->event);
			EventLoopImpl::instance().windowEvent(*widget, *wev);
			break;
		}
		case Ph_EV_INFO:
		{
			//std::cerr << "Ph_EV_INFO"  << std::endl;
			break;
		}
		case Ph_EV_KEY:
		{
			//std::cerr << "Ph_EV_KEY"  << std::endl;
			break;
		}
		case Ph_EV_RAW:
				{
			//std::cerr << "Ph_EV_RAW"  << std::endl;
			break;
		}
		case Ph_EV_SERVICE:
		{
			//std::cerr << "Ph_EV_SERVICE"  << std::endl;
			break;
		}
		case Ph_EV_SYSTEM:
		{
			//std::cerr << "Ph_EV_SYSTEM"  << std::endl;
			break;
		}
		default:
			std::cerr << "Unknown event:" << info->event->type  << std::endl;
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


#/** PhEDIT attribute block
#-11:16777215
#0:9876:default:-3:-3:0
#9876:10291:monospace9:0:-1:0
#10291:10925:default:-3:-3:0
#**  PhEDIT attribute block ends (-0000172)**/
