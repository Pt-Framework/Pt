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


void DockingLayout::addItem(Widget& w, DockMode d)
{
    _docking[&w] = d;
    add(w);
}


void DockingLayout::removeItem(Widget& w)
{
  remove(w);
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


Gfx::Size DockingLayout::onMeasure(const SizePolicy& policy)
{
    Gfx::Size fillSize = policy.size();
    fillSize.subWidth( padding().leftRight() );
    fillSize.subHeight( padding().topBottom() );

    Widget* fillWidget = 0;
    bool hasDockedLeftRight = false;
    bool hasDockedTopBottom = false;

    std::vector<Widget*>::const_iterator it;
    std::vector<Widget*>::const_iterator end = widgets().end();

    for(it = widgets().begin(); it != end; ++it)
    {
        if( ! (*it)->isVisible() )
            continue;

        std::map<Widget*, DockMode>::iterator docking = _docking.find(*it);
        if( docking == _docking.end() )
            continue;

        Pt::ssize_t hspace = padding().leftRight() + (*it)->margin().leftRight();
        Pt::ssize_t vspace = padding().topBottom() + (*it)->margin().topBottom();

        DockMode d = docking->second;
        switch(d)
        {
            case DockingLayout::Fill:
                fillWidget = *it;
                break;

            case DockingLayout::Left:
            case DockingLayout::Right:
            {
                SizePolicy itemPolicy(SizePolicy::Preferred, SizePolicy::Preferred);
                itemPolicy.setWidth( policy.size().width() - hspace );
                itemPolicy.setHeight( fillSize.height() - (*it)->margin().topBottom() );
                
                (*it)->measure(itemPolicy); 
                Gfx::Size itemSize = (*it)->preferredSize();
                
                Pt::ssize_t itemWidth = itemSize.width() + (*it)->margin().leftRight();
                fillSize.subWidth(itemWidth);
                break;  
            }

            case DockingLayout::Top:
            case DockingLayout::Bottom:
            {
                SizePolicy itemPolicy(SizePolicy::Preferred, SizePolicy::Preferred);
                itemPolicy.setWidth( fillSize.width() - (*it)->margin().leftRight() );
                itemPolicy.setHeight( policy.size().height() - vspace );
                
                (*it)->measure(itemPolicy);  
                Gfx::Size itemSize = (*it)->preferredSize();

                Pt::ssize_t itemHeight = itemSize.height() + (*it)->margin().topBottom();
                fillSize.subHeight(itemHeight);
                break;  
            }
        }
    }

    Gfx::Size contentSize;

    if(fillWidget)
    {
        SizePolicy itemPolicy(policy.horizontal(), policy.vertical());
        itemPolicy.setWidth( fillSize.width() - fillWidget->margin().leftRight() );
        itemPolicy.setHeight( fillSize.height() - fillWidget->margin().topBottom() );

        fillWidget->measure(itemPolicy);
        Gfx::Size itemSize = fillWidget->preferredSize();
        
        Pt::ssize_t itemWidth = itemSize.width() + fillWidget->margin().leftRight();
        Pt::ssize_t itemHeight = itemSize.height() + fillWidget->margin().topBottom();
                
        contentSize.addWidth(itemWidth);
        contentSize.addHeight(itemHeight);
    }

    std::vector<Widget*>::const_reverse_iterator rit;
    std::vector<Widget*>::const_reverse_iterator rend = widgets().rend();

    for(rit = widgets().rbegin(); rit != rend; ++rit)
    {
        if( ! (*rit)->isVisible() )
            continue;

        std::map<Widget*, DockMode>::iterator docking = _docking.find(*rit);
        if( docking == _docking.end() )
            continue;

        DockMode d = docking->second;
        switch(d)
        {

            case DockingLayout::Left:
            case DockingLayout::Right:
            {
                Gfx::Size itemSize = (*rit)->preferredSize();
                Pt::ssize_t itemWidth = itemSize.width() + (*rit)->margin().leftRight();
                Pt::ssize_t itemHeight = itemSize.height() + (*rit)->margin().topBottom();
                
                contentSize.addWidth(itemWidth);
                contentSize.setHeight( std::max(contentSize.height(), itemHeight) );
                break;  
            }

            case DockingLayout::Top:
            case DockingLayout::Bottom:
            {
                Gfx::Size itemSize = (*rit)->preferredSize();
                Pt::ssize_t itemWidth = itemSize.width() + (*rit)->margin().leftRight();
                Pt::ssize_t itemHeight = itemSize.height() + (*rit)->margin().topBottom();
                
                contentSize.setWidth( std::max(contentSize.width(), itemWidth) );
                contentSize.addHeight(itemHeight);
                break;  
            }
        }
    }

    contentSize.addWidth( padding().leftRight() );
    contentSize.addHeight( padding().topBottom() );
    return contentSize;
}


void DockingLayout::onLayout(const Gfx::Rect& rect)
{
    Base::onLayout(rect);

    std::vector<Widget*>::const_iterator it = this->widgets().begin();
    std::vector<Widget*>::const_iterator end = this->widgets().end();

    Pt::ssize_t posLeft   = padding().left();
    Pt::ssize_t posTop    = padding().top();
    Pt::ssize_t posRight  = rect.width() - padding().right();
    Pt::ssize_t posBottom = rect.height() - padding().bottom();
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
                Pt::ssize_t x = posLeft + (*it)->margin().left();
                Pt::ssize_t y = posTop  + (*it)->margin().top();            

                posLeft += (*it)->preferredSize().width() + (*it)->margin().leftRight(); 
                            
                Gfx::Size itemSize( (*it)->preferredSize().width(), 
                                     (posBottom - posTop) - 
                                     (*it)->margin().topBottom() );

                Gfx::Point pos(x, y);                   
                (*it)->layout( pos, itemSize );             
                break;  
            }

            case DockingLayout::Top:
            {
                Pt::ssize_t x = posLeft + (*it)->margin().left();
                Pt::ssize_t y = posTop  + (*it)->margin().top();         

                posTop += (*it)->preferredSize().height() + (*it)->margin().topBottom();      
        
                Gfx::Size itemSize( (posRight - posLeft) - (*it)->margin().leftRight(), 
                                     (*it)->preferredSize().height() );

                Gfx::Point pos(x, y);                   
                (*it)->layout( pos, itemSize );              
                break;
            }
          
            case DockingLayout::Right:
            {
                posRight -= (*it)->preferredSize().width()  + (*it)->margin().right();  
                     
                Pt::ssize_t x = posRight;
                Pt::ssize_t y = posTop + (*it)->margin().top();                             
                    
                posRight -=  (*it)->margin().left();

                Gfx::Size itemSize( (*it)->preferredSize().width(), 
                                     (posBottom - posTop) - 
                                     (*it)->margin().topBottom() );

                Gfx::Point pos(x, y);                   
                (*it)->layout( pos, itemSize );              
                break;
            }

            case DockingLayout::Bottom:
            {
                posBottom -= (*it)->preferredSize().height() + (*it)->margin().bottom();   
                   
                Pt::ssize_t x = posLeft + (*it)->margin().left();
                Pt::ssize_t y = posBottom;       
                  
                posBottom -= (*it)->margin().top();                      

                Gfx::Size itemSize( (posRight - posLeft) - (*it)->margin().leftRight(), 
                                     (*it)->preferredSize().height() );

                Gfx::Point pos(x, y);                   
                (*it)->layout( pos, itemSize );              
                break;
            }
        }
    }

    if( ! hasFilled )
        return;

    Gfx::Size fillSize(posRight - posLeft, 
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

            Gfx::Point pos(posLeft + margin.left(), 
                            posTop + margin.top());

            Gfx::Size itemSize( fillSize.width() - margin.leftRight(), 
                                 fillSize.height() - margin.topBottom()  );

            (*it)->layout( pos, itemSize );            
        }
    }
}

} // namespace

} // namespace
