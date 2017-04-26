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
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
  MA 02110-1301 USA
*/

#include <Pt/Hmi/TabView.h>
#include <Pt/Hmi/Painter.h>

namespace Pt {

namespace Hmi {

//////////////////////////////////////////////////////////////////////////
// TabButton
//////////////////////////////////////////////////////////////////////////

TabButton::TabButton()
: _isBeingToggled(false)
{
}


TabButton::~TabButton()
{
}


void TabButton::click()
{
    setPressed( ! isPressed() );
    clicked().send();
}


void TabButton::onPressed()
{
    Base::onPressed();
    
    setPressed( ! isPressed() );
    _isBeingToggled = true;
}


void TabButton::onReleased()
{
    Base::onReleased();

    _isBeingToggled = false;
    clicked().send();
}


void TabButton::onCanceled()
{
    Base::onCanceled();

    if(_isBeingToggled)
    {
        _isBeingToggled = false;
        setPressed( ! isPressed() );
    }
}


void TabButton::onInvalidate()
{
    Base::onInvalidate();
}


Gfx::SizeF TabButton::onMeasure(const SizePolicy& policy)
{
    const Gfx::Font& font = Application::instance().styleOptions().font();
    Gfx::FontMetrics fm = Painter::fontMetrics( font, text() );

    double spacing =  fm.descent() * 4;
    double itemsWidth = itemsWidth = fm.width() + spacing;
    double itemsHeight = itemsHeight = fm.height() + spacing;

    return Gfx::SizeF( itemsWidth + padding().leftRight(), 
                       itemsHeight + padding().topBottom() );
}


void TabButton::onLayout(const Gfx::RectF& rect)
{
}


void TabButton::onPaint(PaintSurface& surface, const Gfx::RectF& rect)
{
    const StyleOptions& options = Application::instance().styleOptions();

    Painter painter(surface);
    painter.setClip(rect);

    if( isPressed() )
    {
        const Gfx::Brush& brush = Application::instance().styleOptions().foreground();
        painter.setBrush(brush);
        painter.fillRect(rect);
    }

    const Gfx::Pen& pen = Application::instance().styleOptions().textColor();
    painter.setPen(pen);

    const Gfx::Font& font = Application::instance().styleOptions().font();
    painter.setFont(font);

    Gfx::FontMetrics fm = Painter::fontMetrics( font, text() );

    double textX = fm.descent() * 2;
    double textY = size().height() / 2 + fm.ascent() / 2;
    Gfx::PointF textPos(textX, textY);
    painter.drawText( textPos, text() );
}

//////////////////////////////////////////////////////////////////////////
// TabBar
//////////////////////////////////////////////////////////////////////////

TabBar::TabBar()
{
    setContent(_layout);
}


TabBar::~TabBar()
{
    clear();
}


void TabBar::clear()
{
    std::vector<TabButton*>::iterator it;
    for(it = _buttons.begin(); it != _buttons.end(); ++it)
    {
        delete *it;
    }

    _buttons.clear();
}


void TabBar::addItem(const Pt::String& title)
{
    TabButton* tb = new TabButton;
    _buttons.push_back(tb);

    tb->setText(title);

    _layout.addItem(*tb);
}


void TabBar::setCurrent(std::size_t n)
{
    if( n >= _buttons.size() )
        return;

    for(std::size_t i = 0; i < _buttons.size(); ++i)
    {
        TabButton* tb = _buttons.at(i);

        if( i == n )
        {
            if( ! tb->isPressed() )
                tb->click();
        }
        else
        {
            if( tb->isPressed() )
                tb->click();
        }
    }
}


void TabBar::onInvalidate()
{
    Base::onInvalidate();
}


Gfx::SizeF TabBar::onMeasure(const SizePolicy& policy)
{
    _layout.measure(policy);
    return _layout.preferredSize();
}


void TabBar::onLayout(const Gfx::RectF& rect)
{
    Gfx::PointF pos(padding().left() + _layout.margin().left(), 
                    padding().top()  + _layout.margin().top());
        
    double hspace = padding().leftRight() + _layout.margin().leftRight();
    double vspace = padding().topBottom() + _layout.margin().topBottom();

    Gfx::SizeF size;
    size.setWidth( rect.width() - hspace );
    size.setHeight( rect.height() - vspace );

    _layout.layout(rect);
}


void TabBar::onPaint(PaintSurface& surface, const Gfx::RectF& rect)
{
}

//////////////////////////////////////////////////////////////////////////
// TabLayout
//////////////////////////////////////////////////////////////////////////

TabLayout::TabLayout()
: _current(0)
{
}


TabLayout::~TabLayout()
{
}


void TabLayout::addItem(Widget& w)
{
    _widgets.push_back(&w);

    w.show(false);
    add(w);
}


void TabLayout::removeItem(Widget& w)
{
    remove(w);
}


void TabLayout::onRemoveWidget(Widget& w)
{
    Widget::onRemoveWidget(w);

    if(_current == &w)
        _current = 0;

    _widgets.erase( std::remove(_widgets.begin(), _widgets.end(), &w),
                    _widgets.end() );
}


void TabLayout::setCurrent(std::size_t n)
{
    if( n >= _widgets.size() )
        return;

    if(_current)
        _current->show(false);

    _current = _widgets.at(n);

    _current->show(true);
}


Gfx::SizeF TabLayout::onMeasure(const SizePolicy& policy)
{
    Gfx::SizeF s;

    std::vector<Widget*>::iterator it;
    for(it = _widgets.begin(); it != _widgets.end(); ++it)
    {
        Widget* item = *it;
        item->measure(policy);
        Gfx::SizeF preferredSize = item->preferredSize();

        double width = std::max( preferredSize.width(), s.width() );
        double height = std::max( preferredSize.height(), s.height() );
        
        s.set(width, height);
    }

    return s;
}


void TabLayout::onLayout(const Gfx::RectF& rect)
{
    if(_current)
    {
        Gfx::PointF pos(padding().left() + _current->margin().left(), 
                        padding().top()  + _current->margin().top());
        
        double hspace = padding().leftRight() + _current->margin().leftRight();
        double vspace = padding().topBottom() + _current->margin().topBottom();

        Gfx::SizeF size;
        size.setWidth( rect.width() - hspace );
        size.setHeight( rect.height() - vspace );

        _current->layout( pos, size );
    }
}

//////////////////////////////////////////////////////////////////////////
// TabView
//////////////////////////////////////////////////////////////////////////

TabView::TabView()
{
    _layout.addItem(_tabBar, DockingLayout::Top);
    _layout.addItem(_tabLayout, DockingLayout::Fill);

    add(_layout);
}


TabView::~TabView()
{
}


void TabView::addItem(const Pt::String& title, Widget& w)
{
    _tabBar.addItem(title);
    _tabLayout.addItem(w);

    // TODO: remove tab button from tab bar if widget is destructed
}


void TabView::setCurrent(std::size_t n)
{
    _tabBar.setCurrent(n);
    _tabLayout.setCurrent(n);
}


void TabView::onInvalidate()
{
    Base::onInvalidate();
}


Gfx::SizeF TabView::onMeasure(const SizePolicy& policy)
{
    _layout.measure(policy);
    return _layout.preferredSize();
}


void TabView::onLayout(const Gfx::RectF& rect)
{
    Gfx::PointF pos(padding().left() + _layout.margin().left(), 
                    padding().top()  + _layout.margin().top());
        
    double hspace = padding().leftRight() + _layout.margin().leftRight();
    double vspace = padding().topBottom() + _layout.margin().topBottom();

    Gfx::SizeF size;
    size.setWidth( rect.width() - hspace );
    size.setHeight( rect.height() - vspace );

    _layout.layout( pos, size );
}


void TabView::onPaint(PaintSurface& surface, const Gfx::RectF& rect)
{
}

} // namespace

} // namespace
