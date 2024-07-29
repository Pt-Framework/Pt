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
#include <Pt/Hmi/Application.h>

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


Gfx::SizeF DockingLayout::onMeasure(const SizePolicy& policy)
{
    Gfx::SizeF fillSize = policy.size();
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

        double hspace = padding().leftRight() + (*it)->margin().leftRight();
        double vspace = padding().topBottom() + (*it)->margin().topBottom();

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
                Gfx::SizeF itemSize = (*it)->preferredSize();

                double itemWidth = itemSize.width() + (*it)->margin().leftRight();
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
                Gfx::SizeF itemSize = (*it)->preferredSize();

                double itemHeight = itemSize.height() + (*it)->margin().topBottom();
                fillSize.subHeight(itemHeight);
                break;
            }
        }
    }

    Gfx::SizeF contentSize;

    if(fillWidget)
    {
        SizePolicy itemPolicy(policy.horizontal(), policy.vertical());
        itemPolicy.setWidth( fillSize.width() - fillWidget->margin().leftRight() );
        itemPolicy.setHeight( fillSize.height() - fillWidget->margin().topBottom() );

        fillWidget->measure(itemPolicy);
        Gfx::SizeF itemSize = fillWidget->preferredSize();

        double itemWidth = itemSize.width() + fillWidget->margin().leftRight();
        double itemHeight = itemSize.height() + fillWidget->margin().topBottom();

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
                Gfx::SizeF itemSize = (*rit)->preferredSize();
                double itemWidth = itemSize.width() + (*rit)->margin().leftRight();
                double itemHeight = itemSize.height() + (*rit)->margin().topBottom();

                contentSize.addWidth(itemWidth);
                contentSize.setHeight( std::max<double>(contentSize.height(), itemHeight) );
                break;
            }

            case DockingLayout::Top:
            case DockingLayout::Bottom:
            {
                Gfx::SizeF itemSize = (*rit)->preferredSize();
                double itemWidth = itemSize.width() + (*rit)->margin().leftRight();
                double itemHeight = itemSize.height() + (*rit)->margin().topBottom();

                contentSize.setWidth( std::max<double>(contentSize.width(), itemWidth) );
                contentSize.addHeight(itemHeight);
                break;
            }
        }
    }

    contentSize.addWidth( padding().leftRight() );
    contentSize.addHeight( padding().topBottom() );
    return contentSize;
}


void DockingLayout::onLayout(const Gfx::RectF& rect)
{
    Base::onLayout(rect);

    std::vector<Widget*>::const_iterator it = this->widgets().begin();
    std::vector<Widget*>::const_iterator end = this->widgets().end();

    double posLeft   = padding().left();
    double posTop    = padding().top();
    double posRight  = rect.width() - padding().right();
    double posBottom = rect.height() - padding().bottom();
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

                Gfx::SizeF itemSize( (*it)->preferredSize().width(),
                                     (posBottom - posTop) -
                                     (*it)->margin().topBottom() );

                Gfx::PointF pos(x, y);

                (*it)->move(pos);
                (*it)->resize(itemSize);
                break;
            }

            case DockingLayout::Top:
            {
                double x = posLeft + (*it)->margin().left();
                double y = posTop  + (*it)->margin().top();

                posTop += (*it)->preferredSize().height() + (*it)->margin().topBottom();

                Gfx::SizeF itemSize( (posRight - posLeft) - (*it)->margin().leftRight(),
                                     (*it)->preferredSize().height() );

                Gfx::PointF pos(x, y);
  
                (*it)->move(pos);
                (*it)->resize(itemSize);

                break;
            }

            case DockingLayout::Right:
            {
                posRight -= (*it)->preferredSize().width()  + (*it)->margin().right();

                double x = posRight;
                double y = posTop + (*it)->margin().top();

                posRight -=  (*it)->margin().left();

                Gfx::SizeF itemSize( (*it)->preferredSize().width(),
                                     (posBottom - posTop) -
                                     (*it)->margin().topBottom() );

                Gfx::PointF pos(x, y);

                (*it)->move(pos);
                (*it)->resize(itemSize);
                break;
            }

            case DockingLayout::Bottom:
            {
                //
                // TODO: align preferred size also in all other cases
                //
                const Gfx::Scaling& scaling = (*it)->surface().scaling();

                double preferredHeight = scaling.align( (*it)->preferredSize().height() );

                posBottom -= preferredHeight + (*it)->margin().bottom();

                double x = posLeft + (*it)->margin().left();
                double y = posBottom;

                posBottom -= (*it)->margin().top();

                Gfx::SizeF itemSize( (posRight - posLeft) - (*it)->margin().leftRight(),
                                     preferredHeight );
                
                Gfx::PointF pos(x, y);

                (*it)->move(pos);
                (*it)->resize(itemSize);
                break;
            }
        }
    }

    if( ! hasFilled )
        return;

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

            Gfx::PointF pos(posLeft + margin.left(),
                            posTop + margin.top());

            Gfx::SizeF itemSize( fillSize.width() - margin.leftRight(),
                                 fillSize.height() - margin.topBottom()  );

            (*it)->move(pos);
            (*it)->resize(itemSize);
        }
    }
}

} // namespace

} // namespace
