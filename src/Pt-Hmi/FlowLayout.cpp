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

#include <Pt/Hmi/FlowLayout.h>

namespace Pt {

namespace Hmi {

void StackLeft(Widget& parent)
{
    std::vector<Widget*>::iterator it = parent.widgets().begin();
    std::vector<Widget*>::iterator end = parent.widgets().end();

    double posLeft = parent.padding().left();
    
    for( ; it != end; ++it)
    {
        Widget* item = *it;   

        double x = posLeft + item->margin().left();
        double y = parent.padding().top() + item->margin().top(); 
                
        posLeft += item->size().width() + item->margin().left() + item->margin().right();

        const Gfx::SizeF childSize( item->size().width(), 
                                    parent.size().height() - 
                                    parent.padding().top() - 
                                    parent.padding().bottom() -
                                    item->margin().top() - 
                                    item->margin().bottom() );
                 
        Gfx::PointF pos(x, y);
        item->setGeometry(pos, childSize);
        //item->update();
    }
}


void StackRight(Widget& parent)
{
    std::vector<Widget*>::iterator it = parent.widgets().begin();
    std::vector<Widget*>::iterator end = parent.widgets().end();

    double posRight  = parent.size().width() - parent.padding().right();

    for( ; it != end; ++it)
    {
        Widget* item = *it; 

        posRight -= item->size().width();
        posRight -= item->margin().right();
                
        double x = posRight;              
        double y = parent.padding().top() + item->margin().top(); 
                
        posRight -= item->margin().left();

        const Gfx::SizeF childSize( item->size().width(), 
                                    parent.size().height() - 
                                    parent.padding().top() - 
                                    parent.padding().bottom() - 
                                    item->margin().top() - 
                                    item->margin().bottom());
                   
        Gfx::PointF pos(x, y);                   
        item->setGeometry(pos, childSize);
        //item->update();
    }
}


void StackTop(Widget& parent)
{
    std::vector<Widget*>::iterator it = parent.widgets().begin();
    std::vector<Widget*>::iterator end = parent.widgets().end();

    double posTop = parent.padding().top();

    for( ; it != end; ++it)
    {
        Widget* item = *it; 

        double x = parent.padding().left() + item->margin().left();
        double y = posTop + item->margin().top();
                
        posTop += item->size().height() + item->margin().top() + item->margin().bottom();
                
        const Gfx::SizeF childSize( parent.size().width() - 
                                    parent.padding().left() -
                                    parent.padding().right() -
                                    item->margin().left() - 
                                    item->margin().right(), 
                                    item->size().height());
                                         

                   
        Gfx::PointF pos(x, y);                   
        item->setGeometry(pos, childSize);
        //item->update();
    }
}


void StackBottom(Widget& parent)
{
    std::vector<Widget*>::iterator it = parent.widgets().begin();
    std::vector<Widget*>::iterator end = parent.widgets().end();

    double posBottom = parent.size().height() - parent.padding().bottom();

    for( ; it != end; ++it)
    {
        Widget* item = *it; 
        
        posBottom -= item->size().height();
        posBottom -= item->margin().bottom();
                
        double x = parent.padding().left() + item->margin().left();
        double y = posBottom;
                
        posBottom -= item->margin().top();    

        const Gfx::SizeF childSize( parent.size().width() - 
                                    parent.padding().left() - 
                                    parent.padding().right() -
                                    item->margin().left() - 
                                    item->margin().right(), 
                                    item->size().height() );
                                         
        Gfx::PointF pos(x, y);                   
        item->setGeometry(pos, childSize);
        //item->update();
    }
}


FlowLayout::FlowLayout()
: _alignment(Left)
{
}


FlowLayout::~FlowLayout()
{
}


void FlowLayout::onLayout()
{
    std::vector<Widget*>::iterator it = this->widgets().begin();
    std::vector<Widget*>::iterator end = this->widgets().end();

    switch(_alignment)
    {
        default:
        case Left:
            StackLeft(*this);
            break;

        case Right:
            StackRight(*this);
            break;

        case Top:
            StackTop(*this);
            break;

        case Bottom:
            StackBottom(*this);
            break;
    }
}

} // namespace

} // namespace
