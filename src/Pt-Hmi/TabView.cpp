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
{
}


TabButton::~TabButton()
{
}


void TabButton::press()
{
    if( isPressed() )
        return;

    setPressed(true);
    clicked().send();
}


void TabButton::release()
{
    if( ! isPressed() )
        return;

    setPressed(false);
}


void TabButton::onPressed()
{
    Base::onPressed();
    press();
}


void TabButton::onReleased()
{
    Base::onReleased();
}


void TabButton::onCanceled()
{
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
: _current( static_cast<std::size_t>(-1) )
{
    setContent(_layout);
}


TabBar::~TabBar()
{
    std::vector<TabButton*>::iterator it;
    for(it = _buttons.begin(); it != _buttons.end(); ++it)
    {
        delete *it;
    }
}


bool TabBar::empty() const
{
    return _buttons.empty();
}


std::size_t TabBar::size() const
{
    return _buttons.size();
}


void TabBar::addTab(const Pt::String& title)
{
    TabButton* tb = new TabButton;
    _buttons.push_back(tb);

    tb->setText(title);
    tb->clicked() += Pt::slot(*this, &TabBar::onClicked);

    _layout.addItem(*tb);
}


void TabBar::removeTab(std::size_t n)
{
    if( n >= _buttons.size() )
        return;

    if(_current == n)
        _current = static_cast<std::size_t>(-1);

    delete _buttons.at(n);
    _buttons.erase(_buttons.begin() + n);
}


std::size_t TabBar::current() const
{
    return _current;
}


void TabBar::setCurrent(std::size_t n)
{
    if( n >= _buttons.size() )
        return;

    if(_current == n)
        return;

    if( _current < _buttons.size() )
        _buttons.at(_current)->release();
    
    _buttons.at(n)->press();
    _current = n;
    _currentChanged.send(_current);
}


void TabBar::onClicked()
{
    if( _current < _buttons.size() )
    {
        _buttons.at(_current)->release();
    }

    std::vector<TabButton*>::iterator it;
    for(it = _buttons.begin(); it != _buttons.end(); ++it)
    {
        TabButton* tb = *it;

        if( tb->isPressed() )
        {
            std::size_t n = std::distance(_buttons.begin(), it );
            _current = n;
            _currentChanged.send(_current);
            return;
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
// TabView
//////////////////////////////////////////////////////////////////////////

TabView::TabView()
{
    _tabBar.currentChanged() += Pt::slot(_stack, &StackLayout::setCurrent);
    _stack.widgetRemoved() += Pt::slot(_tabBar, &TabBar::removeTab);

    _layout.addItem(_tabBar, DockingLayout::Top);
    _layout.addItem(_stack, DockingLayout::Fill);

    add(_layout);
}


TabView::~TabView()
{
}


bool TabView::empty() const
{
    return _tabBar.empty();
}


std::size_t TabView::size() const
{
    return _tabBar.size();
}


void TabView::addTab(Widget& w, const Pt::String& title)
{
    _tabBar.addTab(title);
    _stack.addItem(w);
}


void TabView::removeTab(std::size_t n)
{
    _tabBar.removeTab(n);

    Widget* widget = _stack.widgetAt(n);
    if( ! widget )
        return;

    _stack.removeItem(*widget);
}


std::size_t TabView::current() const
{
    return _tabBar.current();
}


void TabView::setCurrent(std::size_t n)
{
    _tabBar.setCurrent(n);
    _stack.setCurrent(n);
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
