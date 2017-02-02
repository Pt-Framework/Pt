/* Copyright (C) 2017 Marc Boris Duerner
 
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

#include <Pt/Hmi/ListBox.h>
#include <Pt/Hmi/Painter.h>

namespace Pt {

namespace Hmi {

//
// ListBoxItem
//

ListBoxItem::ListBoxItem()
{
    setFocusPolicy(Widget::NormalFocus);
}


ListBoxItem::~ListBoxItem()
{
}


void ListBoxItem::onInvalidate()
{
    Base::onInvalidate();
}


void ListBoxItem::onPaint(PaintSurface& surface, const Gfx::RectF& rect)
{
    const StyleOptions& options = Application::instance().styleOptions();

    Painter painter(surface);
    painter.setClip(rect);
    
    Gfx::Pen pen     = Gfx::Color::fromRgb8(0, 0, 0);
    Gfx::Brush brush = Gfx::Color::fromRgb8(255, 255, 255);
    
    painter.setPen(pen);
    painter.setBrush(brush);

    painter.fillRect( Gfx::RectF( size() ) );
    painter.drawRect( Gfx::RectF( size() ) );
}


void ListBoxItem::onResizeEvent(const ResizeEvent& ev)
{
    Base::onResizeEvent(ev);
}


//
// ListBoxLayout
//

ListBoxLayout::ListBoxLayout()
: FlowLayout(FlowLayout::Top)
{
    setFocusPolicy(Widget::NormalFocus);
}


ListBoxLayout::~ListBoxLayout()
{
}


void ListBoxLayout::onAddWidget(Widget& w)
{
    Base::onAddWidget(w);

    onContentChanged();
}


void ListBoxLayout::onRemoveWidget(Widget& w)
{
    Base::onRemoveWidget(w);
    
    onContentChanged();
}


void ListBoxLayout::onContentChanged()
{
    double itemsHeight = 0;

    for(std::size_t i = 0; i < widgets().size(); ++i)
    {
        ListBoxItem* item = static_cast<ListBoxItem*>(widgets().at(i));

        // item size with margin
        Gfx::SizeF itemSize = item->preferredSize();
        itemSize.addWidth( item->margin().leftRight() );
        itemSize.addHeight( item->margin().topBottom() );

        // the sum of the item heights
        itemsHeight += itemSize.height();
    }

    Gfx::SizeF size = this->size();
    size.setHeight(itemsHeight);

    resize(size);
}

//
// ListBox
//

ListBox::ListBox()
{
    _scrollView.setWidget(_layout);

    setContent(_scrollView);
}


ListBox::~ListBox()
{
}


void ListBox::addItem(ListBoxItem& item)
{   
    _layout.add(item);
}


void ListBox::removeItem(ListBoxItem& item)
{
    _layout.remove(item);
}


void ListBox::onInvalidate()
{
    Base::onInvalidate();
}


void ListBox::onPaint(PaintSurface& surface, const Gfx::RectF& rect)
{
}


void ListBox::onResizeEvent(const ResizeEvent& ev)
{
    Gfx::SizeF size = _layout.size();
    size.setWidth( ev.size().width() );

    _layout.resize(size);

    Base::onResizeEvent(ev);
}

} // namespace

} // namespace


