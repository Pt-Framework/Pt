/***************************************************************************
 *   Copyright (C) 2006 Marc Boris Duerner                                 *
 *   Copyright (C) 2006 Aloysius Indrayanto                                *
 *   Copyright (C) 2007 Sebastian Pieck                                    *
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
#include "WidgetImpl.h"

//#include <Pt/Gui/Application.h>
#include <Pt/Gui/Widget.h>
//#include <Pt/Gui/ResizeEvent.h>
//#include <Pt/Gui/PaintEvent.h>



namespace Pt {

namespace Gui {

WidgetImpl::WidgetImpl(Widget& apiWidget, Widget* parent, const Math::Point& at, const Math::Size& size)
: _apiWidget(apiWidget)
, _ptwidget(0)
, _isShown(false)
{
    if(true)
        _ptwidget =  PtCreateWidget(PtWindow, Pt_NO_PARENT, 0, NULL);
    else
        _ptwidget =  PtCreateWidget(PtWindow, Pt_NO_PARENT, 0, NULL);
    
    this->move(at.x(), at.y());
    this->resize(size.width(), size.height());

    _painter.setRid( PtWidgetRid(_ptwidget ) );
    
    //GfEventLoop::instance().registerWidget(_apiWidget);
}


WidgetImpl::~WidgetImpl()
{
   //GfEventLoop::instance().unregisterWidget(_apiWidget);
}


void WidgetImpl::setTitle(const Pt::String& text)
{

}


Painter WidgetImpl::painter()
{
    _painter.setRid( PtWidgetRid(_ptwidget) );
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
	PtSetResource(_ptwidget,	Pt_ARG_DIM,&dim,0);;
}


} // namespace Gui

} // namespace Pt

#/** PhEDIT attribute block
#-11:16777215
#0:2019:monospace9:-3:-3:0
#2019:2084:monospace9:0:-1:0
#2084:2191:monospace9:-3:-3:0
#2191:2215:monospace9:0:-1:0
#2215:2486:monospace9:-3:-3:0
#2486:2508:monospace9:0:-1:0
#2508:2690:monospace9:-3:-3:0
#2690:2780:monospace9:0:-1:0
#2780:2967:monospace9:-3:-3:0
#2967:2987:monospace9:0:-1:0
#2987:3046:monospace9:-3:-3:0
#3046:3127:monospace9:0:-1:0
#3127:3170:monospace9:-3:-3:0
#**  PhEDIT attribute block ends (-0000470)**/
