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
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.
  
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
  MA 02110-1301 USA
*/

#include <Pt/Hmi/FlowLayout.h>

namespace Pt {

namespace Hmi {

FlowLayout::FlowLayout(Direction d)
: _direction(d)
, _center(false)
, _reverse(false)
{
}


FlowLayout::~FlowLayout()
{
}


void FlowLayout::addItem(Widget& w)
{
    add(w);
}


void FlowLayout::removeItem(Widget& w)
{
    remove(w);
}


void FlowLayout::setDirection(Direction d)
{ 
    _direction = d;
    
    relayout();
}


void FlowLayout::setCenter(bool b)
{
    _center = b;
}

void FlowLayout::setReverse(bool b)
{
    _reverse = b;
}


Gfx::SizeF FlowLayout::onMeasure(const SizePolicy& policy)
{
    Base::onMeasure(policy);

    switch(_direction)
    {
        default:
        case Left:
            return onMeasureHorizontal(policy);
            break;

        case Right:
            return onMeasureHorizontal(policy);
            break;

        case Top:
            return onMeasureVertical(policy);
            break;

        case Bottom:
            return onMeasureVertical(policy);
            break;
    }

    return Gfx::SizeF();
}


void FlowLayout::onLayout(const Gfx::RectF& rect)
{
    Base::onLayout(rect);

    switch(_direction)
    {
        default:
        case Left:
            onLayoutLeft(rect, _center);
            break;

        case Right:
            onLayoutRight(rect, _center);
            break;

        case Top:
            onLayoutTop(rect, _center);
            break;

        case Bottom:
            onLayoutBottom(rect, _center);
            break;
    }
}


Gfx::SizeF FlowLayout::onMeasureHorizontal(const SizePolicy& policy)
{
    std::vector<Widget*>::const_iterator it = widgets().begin();
    std::vector<Widget*>::const_iterator end = widgets().end();

    Gfx::SizeF contentSize;

    for( ; it != end; ++it)
    {
        Widget* item = *it;
        
        if( ! item->isVisible() )
            continue;  

        Gfx::SizeF itemSize( policy.width(), 
                             policy.size().height() - 
                             padding().topBottom() - 
                             item->margin().topBottom() );

        SizePolicy itemPolicy(SizePolicy::Preferred, policy.vertical());
        itemPolicy.setSize(itemSize);

        onMeasureChild(*item, itemPolicy);

        Gfx::SizeF prefSize = item->preferredSize();
        prefSize.addWidth( item->margin().leftRight() );
        prefSize.addHeight( padding().topBottom() + item->margin().topBottom() );

        contentSize.addWidth( prefSize.width() );
        contentSize.setHeight( std::max(contentSize.height(), 
                                        prefSize.height()) );
    }

    contentSize.addWidth( padding().leftRight() );
    contentSize.addHeight( padding().topBottom() );
    return contentSize;
}


void FlowLayout::onLayoutLeft(const Gfx::RectF& rect, bool center)
{
    double posX = padding().left();
    
    if(center)
    {
        double itemsWidth = 0;

        std::vector<Pt::Hmi::Widget*>::const_iterator it;
        for(it = widgets().begin(); it != widgets().end(); ++it)
        {
          Widget* item = *it;
          itemsWidth += item->preferredSize().width();
          itemsWidth += item->margin().leftRight();
        }

        posX = (rect.width() - itemsWidth) / 2;
    }

    if (!_reverse)
    {
        std::vector<Widget*>::const_iterator it = widgets().begin();
        std::vector<Widget*>::const_iterator end = widgets().end();

        for (; it != end; ++it)
        {
            Widget* item = *it;

            if (!item->isVisible())
                continue;

            double x = posX + item->margin().left();
            double y = padding().top() + item->margin().top();

            posX += item->preferredSize().width() +
                item->margin().leftRight();

            Gfx::SizeF itemSize(item->preferredSize().width(),
                rect.size().height() -
                padding().top() -
                padding().bottom() -
                item->margin().top() -
                item->margin().bottom());

            Gfx::PointF pos(x, y);
            layoutContent(*item, pos, itemSize);
        }
    }
    else
    {
        std::vector<Widget*>::const_reverse_iterator it = widgets().rbegin();
        std::vector<Widget*>::const_reverse_iterator end = widgets().rend();

        for (; it != end; ++it)
        {
            Widget* item = *it;

            if (!item->isVisible())
                continue;

            double x = posX + item->margin().left();
            double y = padding().top() + item->margin().top();

            posX += item->preferredSize().width() +
                item->margin().leftRight();

            Gfx::SizeF itemSize(item->preferredSize().width(),
                rect.size().height() -
                padding().top() -
                padding().bottom() -
                item->margin().top() -
                item->margin().bottom());

            Gfx::PointF pos(x, y);
            layoutContent(*item, pos, itemSize);
        }
    }
}


void FlowLayout::onLayoutRight(const Gfx::RectF& rect, bool center)
{
    double posRight  = rect.width() - padding().right();

    if(center)
    {
        double itemsWidth = 0;

        std::vector<Pt::Hmi::Widget*>::const_iterator it;
        for(it = widgets().begin(); it != widgets().end(); ++it)
        {
          Widget* item = *it;
          itemsWidth += item->preferredSize().width();
          itemsWidth += item->margin().leftRight();
        }

        posRight -= (rect.width() - itemsWidth) / 2;
    }

    if (!_reverse)
    {
        std::vector<Widget*>::const_iterator it = widgets().begin();
        std::vector<Widget*>::const_iterator end = widgets().end();

        for (; it != end; ++it)
        {
            Widget* item = *it;

            if (!item->isVisible())
                continue;

            posRight -= item->preferredSize().width();
            posRight -= item->margin().right();

            double x = posRight;
            double y = padding().top() + item->margin().top();

            posRight -= item->margin().left();

            Gfx::SizeF itemSize(item->preferredSize().width(),
                rect.size().height() -
                padding().top() -
                padding().bottom() -
                item->margin().top() -
                item->margin().bottom());

            Gfx::PointF pos(x, y);
            layoutContent(*item, pos, itemSize);
        }
    }
    else
    {
        std::vector<Widget*>::const_reverse_iterator it = widgets().rbegin();
        std::vector<Widget*>::const_reverse_iterator end = widgets().rend();

        for (; it != end; ++it)
        {
            Widget* item = *it;

            if (!item->isVisible())
                continue;

            posRight -= item->preferredSize().width();
            posRight -= item->margin().right();

            double x = posRight;
            double y = padding().top() + item->margin().top();

            posRight -= item->margin().left();

            Gfx::SizeF itemSize(item->preferredSize().width(),
                rect.size().height() -
                padding().top() -
                padding().bottom() -
                item->margin().top() -
                item->margin().bottom());

            Gfx::PointF pos(x, y);
            layoutContent(*item, pos, itemSize);
        }
    }
}


Gfx::SizeF FlowLayout::onMeasureVertical(const SizePolicy& policy)
{
    std::vector<Widget*>::const_iterator it = widgets().begin();
    std::vector<Widget*>::const_iterator end = widgets().end();

    Gfx::SizeF contentSize;

    for( ; it != end; ++it)
    {
        Widget* item = *it; 

        if( ! item->isVisible() )
            continue; 

        Gfx::SizeF itemSize( policy.size().width() - 
                             padding().leftRight() -
                             item->margin().leftRight(), 
                             policy.height() );

        SizePolicy itemPolicy(policy.horizontal(), SizePolicy::Preferred);
        itemPolicy.setSize(itemSize);                  
        
        onMeasureChild(*item, itemPolicy);
        
        Gfx::SizeF prefSize = item->preferredSize();
        prefSize.addWidth( padding().leftRight() + item->margin().leftRight() );
        prefSize.addHeight( item->margin().topBottom() );

        contentSize.setWidth( std::max(contentSize.width(), 
                                       prefSize.width())  );
        contentSize.addHeight( prefSize.height() );
    }

    contentSize.addWidth( padding().leftRight() );
    contentSize.addHeight( padding().topBottom() );
    return contentSize;
}


void FlowLayout::onLayoutTop(const Gfx::RectF& rect, bool center)
{
    double posTop = padding().top();
    
    if(center)
    {
        double itemsHeight = 0;

        std::vector<Pt::Hmi::Widget*>::const_iterator it;
        for(it = widgets().begin(); it != widgets().end(); ++it)
        {
          Widget* item = *it;
          itemsHeight += item->preferredSize().height();
          itemsHeight += item->margin().topBottom();
        }

        posTop = (rect.height() - itemsHeight) / 2;
    }

    if (!_reverse)
    {
        std::vector<Widget*>::const_iterator it = widgets().begin();
        std::vector<Widget*>::const_iterator end = widgets().end();

        for (; it != end; ++it)
        {
            Widget* item = *it;

            if (!item->isVisible())
                continue;

            double x = padding().left() + item->margin().left();
            double y = posTop + item->margin().top();

            posTop += item->preferredSize().height() +
                item->margin().topBottom();

            Gfx::SizeF itemSize(rect.size().width() -
                padding().left() -
                padding().right() -
                item->margin().left() -
                item->margin().right(),
                item->preferredSize().height());

            Gfx::PointF pos(x, y);
            layoutContent(*item, pos, itemSize);
        }
    }
    else
    {
        std::vector<Widget*>::const_reverse_iterator it = widgets().rbegin();
        std::vector<Widget*>::const_reverse_iterator end = widgets().rend();

        for (; it != end; ++it)
        {
            Widget* item = *it;

            if (!item->isVisible())
                continue;

            double x = padding().left() + item->margin().left();
            double y = posTop + item->margin().top();

            posTop += item->preferredSize().height() +
                item->margin().topBottom();

            Gfx::SizeF itemSize(rect.size().width() -
                padding().left() -
                padding().right() -
                item->margin().left() -
                item->margin().right(),
                item->preferredSize().height());

            Gfx::PointF pos(x, y);
            layoutContent(*item, pos, itemSize);
        }
    }
}


void FlowLayout::onLayoutBottom(const Gfx::RectF& rect, bool center)
{
    double posBottom = rect.height() - padding().bottom();

    if(center)
    {
        double itemsHeight = 0;

        std::vector<Pt::Hmi::Widget*>::const_iterator it;
        for(it = widgets().begin(); it != widgets().end(); ++it)
        {
          Widget* item = *it;
          itemsHeight += item->preferredSize().height();
          itemsHeight += item->margin().topBottom();
        }

        posBottom -= (rect.height() - itemsHeight) / 2;
    }

    if (!_reverse)
    {
        std::vector<Widget*>::const_iterator it = widgets().begin();
        std::vector<Widget*>::const_iterator end = widgets().end();

        for (; it != end; ++it)
        {
            Widget* item = *it;

            if (!item->isVisible())
                continue;

            posBottom -= item->preferredSize().height();
            posBottom -= item->margin().bottom();

            double x = padding().left() + item->margin().left();
            double y = posBottom;

            posBottom -= item->margin().top();

            Gfx::SizeF itemSize(rect.size().width() -
                padding().left() -
                padding().right() -
                item->margin().left() -
                item->margin().right(),
                item->preferredSize().height());

            Gfx::PointF pos(x, y);
            layoutContent(*item, pos, itemSize);
        }
    }
    else
    {
        std::vector<Widget*>::const_reverse_iterator it = widgets().rbegin();
        std::vector<Widget*>::const_reverse_iterator end = widgets().rend();

        for (; it != end; ++it)
        {
            Widget* item = *it;

            if (!item->isVisible())
                continue;

            posBottom -= item->preferredSize().height();
            posBottom -= item->margin().bottom();

            double x = padding().left() + item->margin().left();
            double y = posBottom;

            posBottom -= item->margin().top();

            Gfx::SizeF itemSize(rect.size().width() -
                padding().left() -
                padding().right() -
                item->margin().left() -
                item->margin().right(),
                item->preferredSize().height());

            Gfx::PointF pos(x, y);
            layoutContent(*item, pos, itemSize);
        }
    }
}

} // namespace

} // namespace
