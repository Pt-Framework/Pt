/*
 * Copyright (C) 2006 Marc Boris Duerner
 * Copyright (C) 2006 Aloysius Indrayanto
 * Copyright (C) 2006 Sebastian Pieck
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#include "ApplicationImpl.h"
#include "WidgetImpl.h"
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
		                         Gfx::Point( rect->ul.x, rect->ul.y),
		                         Gfx::Size( rect->lr.x - rect->ul.x + 1, rect->lr.y - rect->ul.y + 1) );

		_app->event(pev);
	}
}


void EventLoopImpl::windowEvent(Pt::Gui::Widget& widget, PhWindowEvent_t& ev)
{
	// TODO: Minimized. Could be Ph_WM_HIDE

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
			size_t oldHeight = widget.size().height();
			size_t oldWidth = widget.size().width();

			widget.impl().setClipping();

			ResizeEvent rev(widget,  ev.size.w,  ev.size.h , ResizeEvent::Resize);
			_app->event(rev);

			if( ev.size.h > oldHeight )
			{
				PaintEvent pev (widget,  Gfx::Point( 0, oldHeight ), Gfx::Size( ev.size.w, ev.size.h - oldHeight) );
				_app->event(pev);
			}

			if( ev.size.w > oldWidth )
			{
				PaintEvent pev (widget,  Gfx::Point( oldWidth, 0 ), Gfx::Size( ev.size.w-oldWidth, oldHeight) );
				_app->event(pev);
			}

			break;
		}
		case Ph_WM_MOVE:
		{
			MoveEvent mev(widget, ev.pos.x, ev.pos.y);
			_app->event(mev);
			break;
		}

		case Ph_WM_TOFRONT:
		case Ph_WM_FFRONT:
		case Ph_WM_FOCUS:
		case Ph_WM_CLOSE:
		case Ph_WM_MENU:
		case Ph_WM_TOBACK:
		case Ph_WM_CONSWITCH:
		case Ph_WM_HIDE:
		case Ph_WM_BACKDROP:
		case Ph_WM_HELP:
		case Ph_WM_COLLAPSE:
		case Ph_WM_TASKBAR:
		case Ph_WM_NO_FOCUS_LIST:
			break;

		default:
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
		case Ph_EV_INFO:
		case Ph_EV_KEY:
		case Ph_EV_RAW:
		case Ph_EV_SERVICE:
		case Ph_EV_SYSTEM:
			break;

		default:
			std::cerr << "Unknown event: " << info->event->type  << std::endl;
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
#0:7888:default:-3:-3:0
#7888:7988:monospace9:0:-1:0
#7988:8624:default:-3:-3:0
#**  PhEDIT attribute block ends (-0000169)**/
