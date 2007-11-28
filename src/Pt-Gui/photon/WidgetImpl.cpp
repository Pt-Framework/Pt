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


void onWidgetDraw(PtWidget_t* pw, PhTile_t* damage)
{
	void* w = 0;
	PtGetResource(pw, Pt_ARG_POINTER, &w, 0);

	PhDim_t dim = { 0, 0 };
	PtWidgetDim(pw, &dim);
	Pt::Gui::Widget* widget = (Pt::Gui::Widget*)w;

	//std::cerr << "draw:: " << w << " " << dim.w << ":" << dim.h << std::endl;
	//std::cerr << "draw:: " << PtWidgetRid(pw ) << std::endl;
	
	Pt::Gui::PaintEvent pev(*widget, 
	                        Pt::Math::Point( 0, 0), 
	                        Pt::Math::Size( dim.w, dim.h ) );
	Pt::Gui::EventLoopImpl::instance().commitEvent(pev);
}


namespace Pt {

namespace Gui {

WidgetImpl::WidgetImpl(Widget& apiWidget, Widget* parent, const Math::Point& at, const Math::Size& size)
: _apiWidget(apiWidget)
, _ptwidget(0)
, _isShown(false)
{
	PtArg_t args[4];
	
	PhDim_t dim = { size.width(), size.height() };
	PtSetArg(&args[0],Pt_ARG_DIM,&dim, 0);

	PhPoint_t pos = { at.x(), at.y() };
	PtSetArg(&args[1],Pt_ARG_POS,&pos, 0);

	PtSetArg(&args[2], Pt_ARG_POINTER, &apiWidget, 0);

	PtSetArg(&args[3], Pt_ARG_RAW_DRAW_F, &onWidgetDraw, 1);

  if(parent)
  {
		_ptwidget =  PtCreateWidget(PtRaw, parent->impl().photonWidget(), 4, args);
		//std::cerr << "child: " << &_apiWidget << std::endl;
		PtRealizeWidget(_ptwidget);
		_isShown = true;
    _painter.setRid( PtWidgetRid(_ptwidget) );
	}
  else
  {
		_ptwidget =  PtCreateWidget(PtWindow, Pt_NO_PARENT, 4, args);
		//std::cerr << "top-level: " << &_apiWidget << std::endl;
	}

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
	_painter.setRid( PtWidgetRid(_ptwidget ) );
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

	/*PaintEvent pev(_apiWidget, 
	               Math::Point( 0, 0), 
	               Math::Size( dim.w, dim.h ) );
	EventLoopImpl::instance().commitEvent(pev);*/

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
#1544:2127:monospace9:0:-1:0
#2127:2325:monospace9:-3:-3:0
#2325:2623:monospace9:0:-1:0
#2623:2777:monospace9:-3:-3:0
#2777:2870:monospace9:0:-1:0
#2870:2885:monospace9:-3:-3:0
#2885:3011:monospace9:0:-1:0
#3011:3032:monospace9:-3:-3:0
#3032:3056:monospace9:0:-1:0
#3056:3506:monospace9:-3:-3:0
#3506:3550:monospace9:0:-1:0
#3550:3675:monospace9:-3:-3:0
#3675:3721:monospace9:0:-1:0
#3721:3736:monospace9:-3:-3:0
#3736:3794:monospace9:0:-1:0
#3794:3799:monospace9:-3:-3:0
#3799:3819:monospace9:0:-1:0
#3819:3820:monospace9:-3:-3:0
#3820:3899:monospace9:0:-1:0
#3899:4010:monospace9:-3:-3:0
#4010:4100:monospace9:0:-1:0
#4100:4287:monospace9:-3:-3:0
#4287:4338:monospace9:0:-1:0
#4338:4397:monospace9:-3:-3:0
#4397:5452:monospace9:0:-1:0
#5452:5494:monospace9:-3:-3:0
#**  PhEDIT attribute block ends (-0000883)**/
