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


int onRealize(PtWidget_t* pw, void* data, PtCallbackInfo_t* info)
{
	printf("REALIZE\n");
	
	Pt::Gui::Widget* widget = (Pt::Gui::Widget*)data;

	PhDim_t dim = { 0, 0 };
	PtWidgetDim(pw, &dim);

	PhWindowEvent_t wev;
	memset(&wev, 0, sizeof(wev));
	wev.event_f = Ph_WM_RESIZE;
	wev.size = dim;
	Pt::Gui::EventLoopImpl::instance().windowEvent(*widget, wev);

	Pt::Gui::PaintEvent pev(*widget, 
	               Pt::Math::Point( 0, 0), 
	               Pt::Math::Size( dim.w, dim.h ) );
	Pt::Gui::EventLoopImpl::instance().commitEvent(pev);
	
	return Pt_CONTINUE;
}


namespace Pt {

namespace Gui {

WidgetImpl::WidgetImpl(Widget& apiWidget, Widget* parent, const Math::Point& at, const Math::Size& size)
: _apiWidget(apiWidget)
, _ptwidget(0)
, _isShown(false)
{
	PtArg_t args[5];
	
	PhDim_t dim = { size.width(), size.height() };
	PtSetArg(&args[0],Pt_ARG_DIM,&dim, 0);

	PhPoint_t pos = { at.x(), at.y() };
	PtSetArg(&args[1],Pt_ARG_POS,&pos, 0);

	PtSetArg(&args[2], Pt_ARG_POINTER, &apiWidget, 0);

	PtSetArg(&args[3], Pt_ARG_RAW_DRAW_F, &onWidgetDraw, 1);

	PtCallback_t realizeCallback;
	realizeCallback.event_f = &onRealize;
	realizeCallback.data = &_apiWidget;
  PtSetArg(&args[4], Pt_CB_REALIZED, &realizeCallback, 0);
  
  if(parent)
  {
		_ptwidget =  PtCreateWidget(PtRaw, parent->impl().photonWidget(), 5, args);
		//std::cerr << "child: " << &_apiWidget << std::endl;
		PtRealizeWidget(_ptwidget);
		_isShown = true;
    _painter.setRid( PtWidgetRid(_ptwidget) );
	}
  else
  {
		_ptwidget =  PtCreateWidget(PtWindow, Pt_NO_PARENT, 5, args);
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
#1544:2690:monospace9:0:-1:0
#2690:2888:monospace9:-3:-3:0
#2888:3294:monospace9:0:-1:0
#3294:3509:monospace9:-3:-3:0
#3509:3602:monospace9:0:-1:0
#3602:3617:monospace9:-3:-3:0
#3617:3743:monospace9:0:-1:0
#3743:3764:monospace9:-3:-3:0
#3764:3788:monospace9:0:-1:0
#3788:3793:monospace9:-3:-3:0
#3793:3795:monospace9:0:-1:0
#3795:4242:monospace9:-3:-3:0
#4242:4286:monospace9:0:-1:0
#4286:4411:monospace9:-3:-3:0
#4411:4457:monospace9:0:-1:0
#4457:4509:monospace9:-3:-3:0
#4509:4599:monospace9:0:-1:0
#4599:4786:monospace9:-3:-3:0
#4786:4837:monospace9:0:-1:0
#4837:4896:monospace9:-3:-3:0
#4896:5951:monospace9:0:-1:0
#5951:5993:monospace9:-3:-3:0
#**  PhEDIT attribute block ends (-0000765)**/
