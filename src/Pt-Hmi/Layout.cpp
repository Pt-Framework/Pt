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

#include <Pt/Hmi/Layout.h>

namespace Pt {

namespace Hmi {

void DockingLayouter::onUpdate(LayoutItem::Iterator begin, LayoutItem::Iterator end)
{
    LayoutItem::Iterator it = begin;

    double posLeft   = padding().left();
    double posTop    = padding().top();
    double posRight  = size().width() - padding().right();
    double posBottom = size().height() - padding().bottom();
    bool hasFilled   = false;

    for( ; it != end; ++it)
    {
        switch( it->docking().type() )
        {
            default:
            case Docking::Fill:
                hasFilled = true;
                break;

            case Docking::Left:
            {
              double x = posLeft + it->margin().left();
              double y = posTop  + it->margin().top();            

              const Gfx::SizeF childSize( it->size().width(), 
                                          (posBottom - posTop) - 
                                          it->margin().topBottom() );

              posLeft += it->size().width() + it->margin().leftRight(); 
                            
              Gfx::PointF pos(x, y);                   
              it->setGeometry(pos, childSize);
              break;  
            }

            case Docking::Top:
            {
              double x = posLeft + it->margin().left();
              double y = posTop  + it->margin().top();         
                     
              const Gfx::SizeF childSize( (posRight - posLeft) - it->margin().leftRight(), 
                                          it->size().height() );

              posTop += it->size().height() + it->margin().topBottom();      
        
              Gfx::PointF pos(x, y);                   
              it->setGeometry(pos, childSize);
              break;
            }
          
            case Docking::Right:
            {
              posRight -= it->size().width()  + it->margin().right();  
                     
              double x = posRight;
              double y = posTop + it->margin().top();                             
                    
              posRight -=  it->margin().left();

              const Gfx::SizeF childSize( it->size().width(), 
                                          (posBottom - posTop) - 
                                          it->margin().topBottom() );

              Gfx::PointF pos(x, y);                   
              it->setGeometry(pos, childSize);
              break;
            }

            case Docking::Bottom:
            {
              posBottom -= it->size().height() + it->margin().bottom();   
                   
              double x = posLeft + it->margin().left();
              double y = posBottom;       
                  
              posBottom -= it->margin().top();                      

              const Gfx::SizeF childSize( (posRight - posLeft) - it->margin().leftRight(), 
                                          it->size().height() );

              Gfx::PointF pos(x, y);                   
              it->setGeometry(pos, childSize);
              break;
            }
        }
    }

    if( ! hasFilled)
        return;
    
    const Gfx::PointF fillPos(posLeft, posTop);
    const Gfx::SizeF  fillSize(posRight - posLeft, posBottom - posTop);

    for(it = begin; it != end; ++it)
    {
        if( it->docking().type() == Docking::Fill)
            it->setGeometry(fillPos, fillSize);
    }
}


/////////////////////////////////////////////////////////////////////////////
// DockingLayout
/////////////////////////////////////////////////////////////////////////////

DockingLayout::DockingLayout()
{
}


DockingLayout::~DockingLayout()
{
}


void DockingLayout::onLayout(LayoutItem::Iterator begin, LayoutItem::Iterator end)
{
    _layouter.setSize( size() );
    _layouter.setPadding( padding() );
    _layouter.update(begin, end);
}


/////////////////////////////////////////////////////////////////////////////
// StackLeft
/////////////////////////////////////////////////////////////////////////////

inline void StackLeft(LayoutItem& parent)
{
    LayoutItem::Iterator it = parent.begin();
    LayoutItem::Iterator end = parent.end();

    double posLeft = parent.padding().left();
    
    for( ; it != end; ++it)
    {
        LayoutItem& item = *it;   

        double x = posLeft + it->margin().left();
        double y = parent.padding().top() + it->margin().top(); 
                
        posLeft += it->size().width() + it->margin().left() + it->margin().right();

        const Gfx::SizeF childSize( it->size().width(), 
                                    parent.size().height() - 
                                    parent.padding().top() - 
                                    parent.padding().bottom() -
                                    it->margin().top() - 
                                    it->margin().bottom() );
                 
        Gfx::PointF pos(x, y);
        it->setGeometry(pos, childSize);
    }
}


inline void StackRight(LayoutItem& parent)
{
    LayoutItem::Iterator it = parent.begin();
    LayoutItem::Iterator end = parent.end();

    double posRight  = parent.size().width() - parent.padding().right();

    for( ; it != end; ++it)
    {
        posRight -= it->size().width();
        posRight -= it->margin().right();
                
        double x = posRight;              
        double y = parent.padding().top() + it->margin().top(); 
                
        posRight -= it->margin().left();

        const Gfx::SizeF childSize( it->size().width(), 
                                    parent.size().height() - 
                                    parent.padding().top() - 
                                    parent.padding().bottom() - 
                                    it->margin().top() - 
                                    it->margin().bottom());
                   
        Gfx::PointF pos(x, y);                   
        it->setGeometry(pos, childSize);
    }
}


inline void StackTop(LayoutItem& parent)
{
    LayoutItem::Iterator it = parent.begin();
    LayoutItem::Iterator end = parent.end();

    double posTop = parent.padding().top();

    for( ; it != end; ++it)
    {
        double x = parent.padding().left() + it->margin().left();
        double y = posTop + it->margin().top();
                
        posTop += it->size().height() + it->margin().top() + it->margin().bottom();
                
        const Gfx::SizeF childSize( parent.size().width() - 
                                    parent.padding().left() -
                                    parent.padding().right() -
                                    it->margin().left() - 
                                    it->margin().right(), 
                                    it->size().height());
                                         

                   
        Gfx::PointF pos(x, y);                   
        it->setGeometry(pos, childSize);
    }
}


inline void StackBottom(LayoutItem& parent)
{
    LayoutItem::Iterator it = parent.begin();
    LayoutItem::Iterator end = parent.end();

    double posBottom = parent.size().height() - parent.padding().bottom();

    for( ; it != end; ++it)
    {
        posBottom -= it->size().height();
        posBottom -= it->margin().bottom();
                
        double x = parent.padding().left() + it->margin().left();
        double y = posBottom;
                
        posBottom -= it->margin().top();    

        const Gfx::SizeF childSize( parent.size().width() - 
                                    parent.padding().left() - 
                                    parent.padding().right() -
                                    it->margin().left() - 
                                    it->margin().right(), 
                                    it->size().height() );
                                         
        Gfx::PointF pos(x, y);                   
        it->setGeometry(pos, childSize);
    }
}


inline void Docked(LayoutItem& parent)
{
    LayoutItem::Iterator it = parent.begin();
    LayoutItem::Iterator end = parent.end();

    double posLeft   = parent.padding().left();
    double posTop    = parent.padding().top();
    double posRight  = parent.size().width() - parent.padding().right();
    double posBottom = parent.size().height() - parent.padding().bottom();
    bool hasFilled   = false;

    for( ; it != end; ++it)
    {
        switch( it->docking().type() )
        {
            default:
            case Docking::Fill:
                hasFilled = true;
                break;

            case Docking::Left:
            {
              double x = posLeft + it->margin().left();
              double y = posTop  + it->margin().top();            

              const Gfx::SizeF childSize( it->size().width(), 
                                          (posBottom - posTop) - 
                                          it->margin().topBottom() );

              posLeft += it->size().width() + it->margin().leftRight(); 
                            
              Gfx::PointF pos(x, y);                   
              it->setGeometry(pos, childSize);
              break;  
            }

            case Docking::Top:
            {
              double x = posLeft + it->margin().left();
              double y = posTop  + it->margin().top();         
                     
              const Gfx::SizeF childSize( (posRight - posLeft) - it->margin().leftRight(), 
                                          it->size().height() );

              posTop += it->size().height() + it->margin().topBottom();      
        
              Gfx::PointF pos(x, y);                   
              it->setGeometry(pos, childSize);
              break;
            }
          
            case Docking::Right:
            {
              posRight -= it->size().width()  + it->margin().right();  
                     
              double x = posRight;
              double y = posTop + it->margin().top();                             
                    
              posRight -=  it->margin().left();

              const Gfx::SizeF childSize( it->size().width(), 
                                          (posBottom - posTop) - 
                                          it->margin().topBottom() );

              Gfx::PointF pos(x, y);                   
              it->setGeometry(pos, childSize);
              break;
            }

            case Docking::Bottom:
            {
              posBottom -= it->size().height() + it->margin().bottom();   
                   
              double x = posLeft + it->margin().left();
              double y = posBottom;       
                  
              posBottom -= it->margin().top();                      

              const Gfx::SizeF childSize( (posRight - posLeft) - it->margin().leftRight(), 
                                          it->size().height() );

              Gfx::PointF pos(x, y);                   
              it->setGeometry(pos, childSize);
              break;
            }
        }
    }

    if( ! hasFilled)
        return;
    
    const Gfx::PointF fillPos(posLeft, posTop);
    const Gfx::SizeF  fillSize(posRight - posLeft, posBottom - posTop);

    for(LayoutItem::Iterator it = parent.begin(); it != end; ++it)
    {
        if( it->docking().type() == Docking::Fill)
            it->setGeometry(fillPos, fillSize);
    }
}

} // namespace

} // namespace
