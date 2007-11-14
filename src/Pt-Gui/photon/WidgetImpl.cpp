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

namespace Pt {

namespace Gui {

WidgetImpl::WidgetImpl(Widget& apiWidget, Widget* parent, const Math::Point& at, const Math::Size& size)
: _apiWidget(apiWidget)
, _ptwidget(0)
, _isShown(false)
{
    if(parent)
        _ptwidget =  PtCreateWidget(PtWindow, Pt_NO_PARENT, 0, NULL);
    else
        _ptwidget =  PtCreateWidget(PtWindow, Pt_NO_PARENT, 0, NULL);
    
    this->move(at.x(), at.y());
    this->resize(size.width(), size.height());

    _painter.setRid( PtWidgetRid(_ptwidget ) );

		PtAddEventHandler(_ptwidget, 
		                  Ph_EV_BUT_PRESS |Ph_EV_BUT_RELEASE |Ph_EV_PTR_MOTION, 
		                  &EventLoopImpl::photonEvent,&_apiWidget );
}


WidgetImpl::~WidgetImpl()
{
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
	PtSetResource(_ptwidget,	Pt_ARG_DIM,&dim,0);;
}


} // namespace Gui

} // namespace Pt

#/** PhEDIT attribute block
#-11:16777215
#0:1840:monospace9:-3:-3:0
#1840:1905:monospace9:0:-1:0
#1905:2012:monospace9:-3:-3:0
#2012:2036:monospace9:0:-1:0
#2036:2467:monospace9:-3:-3:0
#2467:2509:monospace9:0:-1:0
#2509:2561:monospace9:-3:-3:0
#2561:2651:monospace9:0:-1:0
#2651:2838:monospace9:-3:-3:0
#2838:2858:monospace9:0:-1:0
#2858:2917:monospace9:-3:-3:0
#2917:2998:monospace9:0:-1:0
#2998:3041:monospace9:-3:-3:0
#**  PhEDIT attribute block ends (-0000470)**/
