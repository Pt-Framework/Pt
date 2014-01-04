/*
 * Copyright (C) 2006 Marc Boris Duerner
 * Copyright (C) 2006 Aloysius Indrayanto
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
#include "WidgetImpl.h"
#include "ApplicationImpl.h"
#include <Pt/Gui/Widget.h>
#include <Pt/Gui/PaintEvent.h>

namespace Pt {

namespace Gui {

void WidgetImpl::onDraw(PtWidget_t* pw, PhTile_t* damage)
{
	void* w = 0;
	PtGetResource(pw, Pt_ARG_POINTER, &w, 0);
	Pt::Gui::Widget* widget = (Pt::Gui::Widget*)w;

	PhDim_t dim = { 0, 0 };
	PtWidgetDim(pw, &dim);

	PhTile_t* tile = 0;
	for(tile = damage; tile; tile = tile->next)
	{
		size_t x = tile->rect.ul.x;
		size_t y = tile->rect.ul.y;
		size_t width = tile->rect.lr.x - tile->rect.ul.x;
		size_t height = tile->rect.lr.y - tile->rect.ul.y;
		PaintEvent pev(*widget, Gfx::Point( x, y), Gfx::Size( width, height ) );
		EventLoopImpl::instance().commitEvent(pev);
	}
}


int WidgetImpl::onRealize(PtWidget_t* pw, void* data, PtCallbackInfo_t* info)
{
	Pt::Gui::Widget* widget = (Pt::Gui::Widget*)data;
	widget->impl()._painter.setRid( PtWidgetRid(pw) );

	if( PtWidgetParent(pw) == NULL )
	{
		PhDim_t dim = { 0, 0 };
		PtWidgetDim(pw, &dim);
		PaintEvent pev(*widget, Gfx::Point( 0, 0), Gfx::Size( dim.w, dim.h ) );
		EventLoopImpl::instance().commitEvent(pev);
	}

	return Pt_CONTINUE;
}


int WidgetImpl::onResize(PtWidget_t* pw, void* data, PtCallbackInfo_t* info)
{
	Pt::Gui::Widget* widget = (Pt::Gui::Widget*)data;

	PhDim_t dim = { 0,	 0 };
	PtWidgetDim(pw, &dim);

 	PhWindowEvent_t wev;
 	memset(&wev, 0, sizeof(wev));
	wev.event_f = Ph_WM_RESIZE;
	wev.size = dim;
	Pt::Gui::EventLoopImpl::instance().windowEvent(*widget, wev);

	widget->impl().setClipping();

	return Pt_CONTINUE;
}




WidgetImpl::WidgetImpl(Widget& apiWidget, Widget* parent, const Gfx::Point& at, const Gfx::Size& size)
: _apiWidget(apiWidget)
, _ptwidget(0)
, _isShown(false)
{
	PtArg_t args[7];

	PhDim_t dim = { size.width(), size.height() };
	PtSetArg(&args[0],Pt_ARG_DIM,&dim, 0);

	PhPoint_t pos = { at.x(), at.y() };
	PtSetArg(&args[1],Pt_ARG_POS,&pos, 0);
	PtSetArg(&args[2], Pt_ARG_POINTER, &apiWidget, 0);
	PtSetArg(&args[3], Pt_ARG_RAW_DRAW_F, &WidgetImpl::onDraw, 1);

	PtCallback_t realizeCallback;
	realizeCallback.event_f = &WidgetImpl::onRealize;
	realizeCallback.data = &_apiWidget;
	PtSetArg(&args[4], Pt_CB_REALIZED, &realizeCallback, 0);

  	PtCallback_t resizeCallback;
	resizeCallback.event_f = &WidgetImpl::onResize;
	resizeCallback.data = &_apiWidget;
	PtSetArg(&args[5], Pt_CB_RESIZE, &resizeCallback, 0);

  PtSetArg(&args[6], Pt_ARG_FILL_COLOR, Pg_TRANSPARENT, 0);

  if(parent)
  {
		_ptwidget =  PtCreateWidget(PtRaw, parent->impl().photonWidget(), 7, args);
	}
  else
  {
		_ptwidget =  PtCreateWidget(PtWindow, Pt_NO_PARENT, 7, args);
	}

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
}


void WidgetImpl::resize(size_t width, size_t height)
{
	PhDim_t dim = { width, height };
	PtSetResource(_ptwidget,	Pt_ARG_DIM,&dim,0);
}


void WidgetImpl::setClipping()
{
	PhDim_t dim = { 0, 0 };
	PtWidgetDim(_ptwidget, &dim);

	PhTile_t* backTile = PhGetTile();
	backTile->rect.ul.x = 0;
	backTile->rect.ul.y = 0;
	backTile->rect.lr.x = dim.w;
	backTile->rect.lr.y = dim.h;
	backTile->next = NULL;

	PhTile_t* tile = NULL;
	PhTile_t* tiles = NULL;
	PtWidget_t* child = 0;
	for(child = PtWidgetChildBack( _ptwidget ); child ; child = PtWidgetBrotherInFront(child))
	{
		if( !tiles )
		{
  			tile = PhGetTile();
  			tiles = tile;
		}
		else
		{
			tile->next = PhGetTile();
			tile = tile->next;
		}

		PhArea_t area;
		PtWidgetArea( child, &area );

		tile->rect.ul.x = area.pos.x;
		tile->rect.ul.y = area.pos.y;
		tile->rect.lr.x = area.pos.x + area.size.w - 1;
		tile->rect.lr.y = area.pos.y + area.size.h - 1;
		tile->next = NULL;
	}

	if(tiles)
	{
		PhTile_t* clipped = PhClipTilings(backTile, tiles, NULL);
		_painter.setClipping(clipped);
		PhFreeTiles(tiles);
	}
}

} // namespace Gui

} // namespace Pt
