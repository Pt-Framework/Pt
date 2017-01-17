/* Copyright (C) 2015 Marc Boris Duerner 
   Copyright (C) 2015 Laurentiu-Gheorghe Crisan
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  
  As a special exception, you may use this file as part of a free
  software library without restriction. Specifically, if other files
  instantiate templates or use macros or inline functions from this
  file, or you compile this file and link it with other files to
  produce an executable, this file does not by itself cause the
  resulting executable to be covered by the GNU General Public
  License. This exception does not however invalidate any other
  reasons why the executable file might be covered by the GNU Library
  General Public License.
  
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
  MA 02110-1301 USA
*/

#include <Pt/Hmi/DockingLayout.h>

namespace Pt {

namespace Hmi {

DockingLayout::DockingLayout(DockStyle ds)
: _defaultDocking(ds)
{
}


DockingLayout::~DockingLayout()
{
}


void DockingLayout::dock(Widget& w, DockStyle ds)
{
    _docking[&w] = ds;

    add(w);
}


void DockingLayout::setDockingStyle(Widget& w, DockStyle ds)
{
    std::map<Widget*, DockStyle>::iterator it = _docking.find(&w);
    if( it != _docking.end() )
        it->second = ds;
}


void DockingLayout::onAddWidget(Widget& w)
{
}


void DockingLayout::onRemoveWidget(Widget& w)
{
    std::map<Widget*, DockStyle>::iterator it = _docking.find(&w);
    
    if( it != _docking.end() )
        _docking.erase(it);
}


void DockingLayout::onLayout()
{
    std::vector<Widget*>::const_iterator it = this->widgets().begin();
    std::vector<Widget*>::const_iterator end = this->widgets().end();

    double posLeft   = padding().left();
    double posTop    = padding().top();
    double posRight  = size().width() - padding().right();
    double posBottom = size().height() - padding().bottom();
    bool hasFilled   = false;

    for( ; it != end; ++it)
    {
        if( ! (*it)->isVisible() )
            continue;

        std::map<Widget*, DockStyle>::iterator d = _docking.find(*it);
        if( d == _docking.end() )
            continue;

        DockStyle ds = d->second;

        switch(ds)
        {
            default:
            case DockingLayout::Fill:
                hasFilled = true;
                break;

            case DockingLayout::Left:
            {
                double x = posLeft + (*it)->margin().left();
                double y = posTop  + (*it)->margin().top();            

                const Gfx::SizeF childSize( (*it)->size().width(), 
                                            (posBottom - posTop) - 
                                            (*it)->margin().topBottom() );

                posLeft += (*it)->size().width() + (*it)->margin().leftRight(); 
                            
                Gfx::PointF pos(x, y);                   
                (*it)->setGeometry(pos, childSize);              
                break;  
            }

            case DockingLayout::Top:
            {
                double x = posLeft + (*it)->margin().left();
                double y = posTop  + (*it)->margin().top();         
                     
                const Gfx::SizeF childSize( (posRight - posLeft) - (*it)->margin().leftRight(), 
                                            (*it)->size().height() );

                posTop += (*it)->size().height() + (*it)->margin().topBottom();      
        
                Gfx::PointF pos(x, y);                   
                (*it)->setGeometry(pos, childSize);              
                break;
            }
          
            case DockingLayout::Right:
            {
                posRight -= (*it)->size().width()  + (*it)->margin().right();  
                     
                double x = posRight;
                double y = posTop + (*it)->margin().top();                             
                    
                posRight -=  (*it)->margin().left();

                const Gfx::SizeF childSize( (*it)->size().width(), 
                                            (posBottom - posTop) - 
                                            (*it)->margin().topBottom() );

                Gfx::PointF pos(x, y);                   
                (*it)->setGeometry(pos, childSize);              
                break;
            }

            case DockingLayout::Bottom:
            {
                posBottom -= (*it)->size().height() + (*it)->margin().bottom();   
                   
                double x = posLeft + (*it)->margin().left();
                double y = posBottom;       
                  
                posBottom -= (*it)->margin().top();                      

                const Gfx::SizeF childSize( (posRight - posLeft) - (*it)->margin().leftRight(), 
                                            (*it)->size().height() );

                Gfx::PointF pos(x, y);                   
                (*it)->setGeometry(pos, childSize);              
                break;
            }
        }
    }

    // TODO: keep widgets with Fill style in separate container
    if( ! hasFilled)
        return;
    
    const Gfx::PointF fillPos(posLeft, posTop);
    const Gfx::SizeF  fillSize(posRight - posLeft, posBottom - posTop);

    for(it = widgets().begin(); it != end; ++it)
    {
        std::map<Widget*, DockStyle>::iterator d = _docking.find(*it);
        if( d == _docking.end() )
            continue;

        DockStyle ds = d->second;

        if( ds == DockingLayout::Fill)
        {
            if( ! (*it)->isVisible() )
                continue;
              
            (*it)->setGeometry(fillPos, fillSize);            
        }
    }
}

} // namespace

} // namespace
