/* Copyright (C) 2025 Marc Boris Duerner 

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

#include <Pt/Forms/CanvasLayout.h>
#include <Pt/Forms/PaintContext.h>

namespace Pt {

namespace Forms {

CanvasLayout::CanvasLayout()
{
}


CanvasLayout::~CanvasLayout()
{
}


void CanvasLayout::addItem(Control& control, const LayoutParams& params)
{
    add(control);
    _items[&control] = params;
}


void CanvasLayout::addItem(Control& control, 
                           const Gfx::PointF& pos,
                           const Gfx::SizeF& size)
{
    addItem( control, LayoutParams(pos, size) );
}


void CanvasLayout::removeItem(Control& control)
{
    remove(control);
}


void CanvasLayout::onRemoveControl(Control& control)
{
    Base::onRemoveControl(control);

    _items.erase(&control);
}


Gfx::SizeF CanvasLayout::onMeasure(PaintContext& ctx, const SizePolicy& policy)
{
    Base::onMeasure(ctx, policy);

    Gfx::SizeF size;

    ItemMap::iterator it;
    for(it = _items.begin(); it != _items.end(); ++it)
    {
        //
        // TODO: A bug in Control::measure prevents passing the policy
        //       directly to the children. Control::measure optimizes
        //       layouting of fixed children away, although internal
        //       changes might require a measure, as in case of button
        //       text metrics.
        //
        SizePolicy itemPolicy(SizePolicy::Preferred, SizePolicy::Preferred);
        itemPolicy.setWidth( policy.size().width() );
        itemPolicy.setHeight( policy.size().height() );
        
        Control* control = it->first;
        control->measure(itemPolicy);

        const LayoutParams& params = it->second;
        double w = params.position().x() + params.size().width();
        double h = params.position().y() + params.size().height();
        
        if( w > size.width() )
            size.setWidth(w);

        if( h > size.height() )
            size.setHeight(h);
    }

    size.addWidth( padding().leftRight() );
    size.addHeight( padding().topBottom() );
    return size;
}


void CanvasLayout::onLayout(PaintContext& ctx, const Gfx::RectF& rect)
{
    Base::onLayout(ctx, rect);

    ItemMap::iterator it;
    for(it = _items.begin(); it != _items.end(); ++it)
    {
        Control* control = it->first;
        const LayoutParams& params = it->second;
        
        double x = params.position().x() + padding().left();
        double y = params.position().y() + padding().top();

        control->move( Gfx::PointF(x, y) );
        control->resize( params.size() );
    }
}

} // namespace

} // namespace
