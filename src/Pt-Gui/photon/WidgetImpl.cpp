/***************************************************************************
 *   Copyright (C) 2006 Marc Boris Duerner                                 *
 *   Copyright (C) 2006 Aloysius Indrayanto                                *
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
#include "WidgetImpl.h"
#include "ApplicationImpl.h"
#include <Pt/Gui/Widget.h>
#include <Pt/Gui/PaintEvent.h>

namespace Pt {

namespace Gui {

WidgetImpl::WidgetImpl(Widget& apiWidget, Widget* parent, const Math::Point& at, const Math::Size& size)
: _apiWidget(apiWidget)
, _ptwidget(0)
, _isShown(false)
{
	PtArg_t args[2];
	
	PhDim_t dim = { size.width(), size.height() };
	PtSetArg(&args[0],Pt_ARG_DIM,&dim, 0);

	PhPoint_t pos = { at.x(), at.y() };
	PtSetArg(&args[1],Pt_ARG_POS,&pos, 0);

  if(parent)
		_ptwidget =  PtCreateWidget(PtWindow, Pt_NO_PARENT, 1, args);
  else
		_ptwidget =  PtCreateWidget(PtWindow, Pt_NO_PARENT, 1, args);

  _painter.setRid( PtWidgetRid(_ptwidget ) );


	PtAddEventHandler(_ptwidget, 
	                  Ph_EV_BUT_PRESS |Ph_EV_BUT_RELEASE |Ph_EV_PTR_MOTION |
	                  Ph_EV_EXPOSE|Ph_EV_KEY|Ph_EV_BOUNDARY |Ph_EV_DRAG |
	                  Ph_EV_WM | Ph_EV_KEY | Ph_EV_RAW,
	                  &EventLoopImpl::photonEvent,&_apiWidget );
}


WidgetImpl::~WidgetImpl()
{
	PtDestroyWidget(_ptwidget);
}


void WidgetImpl::setTitle(const Pt::String& text)
{

}


Painter WidgetImpl::painter()
{
    return Painter(&_painter);
}


void WidgetImpl::show()
{
    if(_isShown)
        return;

    PtRealizeWidget(_ptwidget);
    _painter.setRid( PtWidgetRid(_ptwidget) );
		PtFlush();

		PhDim_t dim = { 0, 0 };
		PtWidgetDim(_ptwidget, &dim);

    PhWindowEvent_t wev;
		memset(&wev, 0, sizeof(wev));
		wev.event_f = Ph_WM_RESIZE;
		wev.size = dim;
		EventLoopImpl::instance().windowEvent(_apiWidget, wev);

    _isShown = true;
}


void WidgetImpl::hide()
{
    if(!_isShown)
        return;

    PtUnrealizeWidget(_ptwidget);
    _isShown = false;
}


void WidgetImpl::setParent(Widget* parent)
{
	// PtReparentWidget(widget, parent)
}


void WidgetImpl::move(size_t x, size_t y)
{
	PhPoint_t pos = { x, y };
	PtSetResource(_ptwidget,Pt_ARG_POS,&pos, 0);
	
	//TODO: synthetic MoveEvent
}


void WidgetImpl::resize(size_t width, size_t height)
{
	PhDim_t dim = { width, height };
	PtSetResource(_ptwidget,	Pt_ARG_DIM,&dim,0);

  PhWindowEvent_t wev;
  memset(&wev, 0, sizeof(wev));
	wev.event_f = Ph_WM_RESIZE;
	wev.size = dim;
	EventLoopImpl::instance().windowEvent(_apiWidget, wev);

	PaintEvent pev(_apiWidget, 
	               Math::Point( 0, 0), 
	               Math::Size( dim.w, dim.h ) );
	EventLoopImpl::instance().commitEvent(pev);

/*
	PhRegion_t region;
	region.rid = PtWidgetRid(_ptwidget);
	
	
	PhRegionChange(Ph_REGION_RECT,
	Ph_EXPOSE_REGION,
	&region,
	NULL,
	NULL );
*/
	
	//PtMoveResizeWidget(_ptwidget, Pt_BLIT);

/*
PhWindowEvent_t wev;
memset(&wev, 0, sizeof(wev));
wev.event_f = Ph_WM_RESIZE;
wev.size = dim;

PhEvent_t ev;
memset(&ev, 0, sizeof(ev));
ev.type = Ph_EV_WM;
ev.subtype = 0;
ev.processing_flags = Ph_FAKE_EVENT;
ev.flags = Ph_EVENT_DIRECT;
ev.num_rects = 1;
ev.data_len = sizeof(wev);
ev.emitter.rid = Ph_DEV_RID;
ev.collector.rid = PtWidgetRid(_ptwidget);

PhRect_t rect;
rect.ul.x = rect.ul.y = 0;
rect.lr.x = rect.lr.y = 200;

PhEmit(&ev, &rect, &wev);
*/
}


} // namespace Gui

} // namespace Pt

#/** PhEDIT attribute block
#-11:16777215
#0:1544:monospace9:-3:-3:0
#1544:1575:monospace9:0:-1:0
#1575:1773:monospace9:-3:-3:0
#1773:1959:monospace9:0:-1:0
#1959:2044:monospace9:-3:-3:0
#2044:2107:monospace9:0:-1:0
#2107:2128:monospace9:-3:-3:0
#2128:2152:monospace9:0:-1:0
#2152:2729:monospace9:-3:-3:0
#2729:2775:monospace9:0:-1:0
#2775:2790:monospace9:-3:-3:0
#2790:2848:monospace9:0:-1:0
#2848:2853:monospace9:-3:-3:0
#2853:2873:monospace9:0:-1:0
#2873:2874:monospace9:-3:-3:0
#2874:2953:monospace9:0:-1:0
#2953:3064:monospace9:-3:-3:0
#3064:3154:monospace9:0:-1:0
#3154:3341:monospace9:-3:-3:0
#3341:3392:monospace9:0:-1:0
#3392:3451:monospace9:-3:-3:0
#3451:4502:monospace9:0:-1:0
#4502:4544:monospace9:-3:-3:0
#**  PhEDIT attribute block ends (-0000765)**/
