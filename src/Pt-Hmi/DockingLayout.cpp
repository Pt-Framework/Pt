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

DockingLayout::DockingLayout(DockMode d)
: _defaultDocking(d)
{
}


DockingLayout::~DockingLayout()
{
}


void DockingLayout::dock(Widget& w, DockMode d)
{
    _docking[&w] = d;
    add(w);
}


void DockingLayout::setDockingStyle(Widget& w, DockMode d)
{
    std::map<Widget*, DockMode>::iterator it = _docking.find(&w);
    if( it != _docking.end() )
        it->second = d;
}


void DockingLayout::onAddWidget(Widget& w)
{
}


void DockingLayout::onRemoveWidget(Widget& w)
{
    std::map<Widget*, DockMode>::iterator it = _docking.find(&w);
    
    if( it != _docking.end() )
        _docking.erase(it);
}


Gfx::SizeF DockingLayout::onMeasure(const SizePolicy& policy)
{
    std::vector<Widget*>::const_iterator it = widgets().begin();
    std::vector<Widget*>::const_iterator end = widgets().end();

    double posLeft   = padding().left();
    double posTop    = padding().top();
    double posRight  = policy.size().width() - padding().right();
    double posBottom = policy.size().height() - padding().bottom();
    bool hasFilled   = false;

    for( ; it != end; ++it)
    {
        if( ! (*it)->isVisible() )
            continue;

        std::map<Widget*, DockMode>::iterator docking = _docking.find(*it);
        if( docking == _docking.end() )
            continue;

        DockMode d = docking->second;

        switch(d)
        {
            default:
            case DockingLayout::Fill:
                hasFilled = true;
                break;

            case DockingLayout::Left:
            {
                Gfx::SizeF itemSize( (*it)->preferredSize().width(), 
                                     (posBottom - posTop) - 
                                     (*it)->margin().topBottom() );

                posLeft += (*it)->preferredSize().width() + (*it)->margin().leftRight();
                            
                SizePolicy itemPolicy(SizePolicy::Fixed, SizePolicy::Fixed);
                itemPolicy.setSize(itemSize);
                
                (*it)->measure(itemPolicy);             
                break;  
            }

            case DockingLayout::Top:
            {            
                Gfx::SizeF itemSize( (posRight - posLeft) - (*it)->margin().leftRight(), 
                                     (*it)->preferredSize().height() );

                posTop += (*it)->preferredSize().height() + (*it)->margin().topBottom();      
        
                SizePolicy itemPolicy(SizePolicy::Fixed, SizePolicy::Fixed);
                itemPolicy.setSize(itemSize);
                
                (*it)->measure(itemPolicy);               
                break;
            }
          
            case DockingLayout::Right:
            {
                posRight -= (*it)->preferredSize().width()  + (*it)->margin().right();           
                posRight -=  (*it)->margin().left();

                Gfx::SizeF itemSize( (*it)->preferredSize().width(), 
                                     (posBottom - posTop) - 
                                     (*it)->margin().topBottom() );

                SizePolicy itemPolicy(SizePolicy::Fixed, SizePolicy::Fixed);
                itemPolicy.setSize(itemSize);
                
                (*it)->measure(itemPolicy);               
                break;
            }

            case DockingLayout::Bottom:
            {
                posBottom -= (*it)->preferredSize().height() + (*it)->margin().bottom();                
                posBottom -= (*it)->margin().top();                      

                Gfx::SizeF itemSize( (posRight - posLeft) - (*it)->margin().leftRight(), 
                                     (*it)->preferredSize().height() );

                SizePolicy itemPolicy(SizePolicy::Fixed, SizePolicy::Fixed);
                itemPolicy.setSize(itemSize);
                
                (*it)->measure(itemPolicy);              
                break;
            }
        }
    }

    if( ! hasFilled )
        return Base::onMeasure(policy);
    
    Gfx::SizeF fillSize(posRight - posLeft, 
                        posBottom - posTop);

    for(it = widgets().begin(); it != end; ++it)
    {
        std::map<Widget*, DockMode>::iterator docking = _docking.find(*it);
        if( docking == _docking.end() )
            continue;

        DockMode d = docking->second;

        if(d == DockingLayout::Fill)
        {
            if( ! (*it)->isVisible() )
                continue;

            const Spacing& margin = (*it)->margin();

            Gfx::SizeF itemSize( fillSize.width() - margin.leftRight(), 
                                 fillSize.height() - margin.topBottom()  );

            SizePolicy itemPolicy(SizePolicy::Fixed, SizePolicy::Fixed);
            itemPolicy.setSize(itemSize);
                
            (*it)->measure(itemPolicy);           
        }
    }

    return Base::onMeasure(policy);
}


void DockingLayout::onLayout()
{
    Base::onLayout();

    std::vector<Widget*>::const_iterator it = this->widgets().begin();
    std::vector<Widget*>::const_iterator end = this->widgets().end();

    double posLeft   = padding().left();
    double posTop    = padding().top();
    double posRight  = measuredSize().width() - padding().right();
    double posBottom = measuredSize().height() - padding().bottom();
    bool hasFilled   = false;

    for( ; it != end; ++it)
    {
        if( ! (*it)->isVisible() )
            continue;

        std::map<Widget*, DockMode>::iterator docking = _docking.find(*it);
        if( docking == _docking.end() )
            continue;

        DockMode d = docking->second;

        switch(d)
        {
            default:
            case DockingLayout::Fill:
                hasFilled = true;
                break;

            case DockingLayout::Left:
            {
                double x = posLeft + (*it)->margin().left();
                double y = posTop  + (*it)->margin().top();            

                posLeft += (*it)->preferredSize().width() + (*it)->margin().leftRight(); 
                            
                Gfx::PointF pos(x, y);                   
                (*it)->layout( pos, (*it)->measuredSize() );             
                break;  
            }

            case DockingLayout::Top:
            {
                double x = posLeft + (*it)->margin().left();
                double y = posTop  + (*it)->margin().top();         

                posTop += (*it)->preferredSize().height() + (*it)->margin().topBottom();      
        
                Gfx::PointF pos(x, y);                   
                (*it)->layout( pos, (*it)->measuredSize() );              
                break;
            }
          
            case DockingLayout::Right:
            {
                posRight -= (*it)->preferredSize().width()  + (*it)->margin().right();  
                     
                double x = posRight;
                double y = posTop + (*it)->margin().top();                             
                    
                posRight -=  (*it)->margin().left();

                Gfx::PointF pos(x, y);                   
                (*it)->layout( pos, (*it)->measuredSize() );              
                break;
            }

            case DockingLayout::Bottom:
            {
                posBottom -= (*it)->preferredSize().height() + (*it)->margin().bottom();   
                   
                double x = posLeft + (*it)->margin().left();
                double y = posBottom;       
                  
                posBottom -= (*it)->margin().top();                      

                Gfx::PointF pos(x, y);                   
                (*it)->layout( pos, (*it)->measuredSize() );              
                break;
            }
        }
    }

    if( ! hasFilled )
        return;

    for(it = widgets().begin(); it != end; ++it)
    {
        std::map<Widget*, DockMode>::iterator docking = _docking.find(*it);
        if( docking == _docking.end() )
            continue;

        DockMode d = docking->second;

        if(d == DockingLayout::Fill)
        {
            if( ! (*it)->isVisible() )
                continue;

            const Spacing& margin = (*it)->margin();

            Gfx::PointF pos(posLeft + margin.left(), 
                            posTop + margin.top());

            (*it)->layout( pos, (*it)->measuredSize() );            
        }
    }
}

} // namespace

} // namespace
