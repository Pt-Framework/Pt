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

#include <Pt/Forms/TabView.h>
#include <Pt/Forms/Application.h>
#include <Pt/Forms/PaintContext.h>
#include <Pt/Forms/Painter.h>
#include <Pt/Gfx/Painter.h>

namespace Pt {

namespace Forms {

//////////////////////////////////////////////////////////////////////////
// TabBar
//////////////////////////////////////////////////////////////////////////

TabBar::TabBar()
: _current( static_cast<std::size_t>(-1) )
{
}


TabBar::~TabBar()
{
}


bool TabBar::empty() const
{
    return _tabs.empty();
}


std::size_t TabBar::size() const
{
    return _tabs.size();
}


void TabBar::addTab(const Pt::String& title)
{
    TabItem t;
    t.setText(title);
    _tabs.push_back(t);

    relayout();
}


void TabBar::removeTab(std::size_t n)
{
    if( n >= _tabs.size() )
        return;

    if(_current == n)
        _current = static_cast<std::size_t>(-1);
    else if(_current != static_cast<std::size_t>(-1) && n < _current)
        --_current;

    _tabs.erase(_tabs.begin() + n);

    relayout();
}


std::size_t TabBar::current() const
{
    return _current;
}


void TabBar::setCurrent(std::size_t n)
{
    if( n >= _tabs.size() )
        return;

    if(_current == n)
        return;

    if( _current < _tabs.size() )
    {
        _tabs.at(_current).setPressed(false);
    }
    
    _tabs.at(n).setPressed(true);
    _current = n;

    invalidate();
    _currentChanged.send(_current);
}


void TabBar::setText(std::size_t n, const Pt::String& title)
{
    if( n >= _tabs.size() )
        return;

    _tabs.at(n).setText(title);

    relayout();
    repaint();
}


const Gfx::RectF& TabBar::currentTabRect() const
{
    static const Gfx::RectF empty;

    if( _current < _tabs.size() )
        return _tabs[_current].geometry();

    return empty;
}


void TabBar::setFont(const Gfx::Font& font)
{
    _tabBarOptions.setFont(font);
    invalidate();
}


void TabBar::setFontSize(std::size_t size)
{
    _tabBarOptions.setFontSize(size);
    invalidate();
}


void TabBar::setFontWeight(Gfx::Font::Weight weight)
{
    _tabBarOptions.setFontWeight(weight);
    invalidate();
}


void TabBar::setFontSlant(Gfx::Font::Slant slant)
{
    _tabBarOptions.setFontSlant(slant);
    invalidate();
}


void TabBar::setTextColor(const Gfx::Color& color)
{
    _tabBarOptions.setTextColor(color);
    invalidate();
}


void TabBar::setRenderer(TabViewRenderer* renderer)
{
    const StyleOptions& options = Application::instance().styleOptions();
    const Style& style = Application::instance().style();

    if(renderer)
        _tabBarStyle.bind(*renderer, options, _tabBarOptions);
    else
        _tabBarStyle.bind(style, options, _tabBarOptions);

    invalidate();
}


void TabBar::onInvalidate()
{
    const StyleOptions& options = Application::instance().styleOptions();
    const Style& style = Application::instance().style();

    _tabBarStyle.rebind(style, options, _tabBarOptions);

    if( ! _tabBarStyle.renderer() )
        return;

    Base::onInvalidate();
    relayout();
}


Gfx::SizeF TabBar::onMeasure(const SizePolicy& policy)
{
    TabViewRenderer* renderer = _tabBarStyle.renderer();
    if( ! renderer )
        return Gfx::SizeF();

    // Measure total size by summing individual tab sizes
    Gfx::SizeF total;

    for(std::size_t i = 0; i < _tabs.size(); ++i)
    {
        Gfx::SizeF tabSize = renderer->measureTab( surface(), _tabs[i].text() );
        _tabs[i].setMeasuredSize(tabSize);

        total.addWidth( tabSize.width() );

        if( tabSize.height() > total.height() )
            total.setHeight( tabSize.height() );
    }

    return total;
}


void TabBar::onLayout(const Gfx::RectF& rect)
{
    Base::onLayout(rect);

    TabViewRenderer* renderer = _tabBarStyle.renderer();
    if( ! renderer )
        return;

    const Painter& painter = renderer->textPainter( surface() );
    Gfx::FontMetrics fontMet = painter.fontMetrics();

    Gfx::PointF tabPos;

    for(std::size_t i = 0; i < _tabs.size(); ++i)
    {
        const Gfx::SizeF& tabSize = _tabs[i].measuredSize();

        Gfx::RectF tabRect;
        tabRect.setOrigin(tabPos);
        tabRect.setWidth( tabSize.width() );
        tabRect.setHeight( rect.height() );

        _tabs[i].setGeometry(tabRect);

        Gfx::RectF textRect = renderer->layoutTab( surface(), tabRect );
        double textY = textRect.y()
                     + (textRect.height() - fontMet.height()) / 2.0
                     + fontMet.ascent();
        _tabs[i].setTextPos( Gfx::PointF(textRect.x(), textY) );

        tabPos.addX( tabSize.width() );
    }
}


void TabBar::onPaint(PaintContext& context, const Gfx::RectF& rect)
{
    TabViewRenderer* renderer = _tabBarStyle.renderer();
    if( ! renderer )
        return;

    for(std::size_t i = 0; i < _tabs.size(); ++i)
    {
        const TabItem& tab = _tabs[i];

        TabItemState itemState;
        itemState.setEnabled( isEnabled() );
        itemState.setActive( tab.isPressed() );

        onPaintTab(context, tab.geometry(), tab.text(), tab.textPos(), itemState);
    }
}


void TabBar::onPaintTab(PaintContext& context,
                        const Gfx::RectF& tabRect,
                        const Pt::String& text,
                        const Gfx::PointF& textPos,
                        const TabItemState& state)
{
    TabViewRenderer* renderer = _tabBarStyle.renderer();
    if( ! renderer )
        return;

    renderer->renderTab(context, tabRect, text, textPos, state);
}


bool TabBar::onMouseEvent(const MouseEvent& ev)
{
    Base::onMouseEvent(ev);

    if( ! ev.isPress() )
        return true;

    for(std::size_t n = 0; n != _tabs.size(); ++n)
    {
        TabItem& t = _tabs[n];
        
        if( t.geometry().contains( ev.position() ) )
        {
            if(n != _current)
            {
                if(_current < _tabs.size() )
                    _tabs.at(_current).setPressed(false);
                
                t.setPressed(true);

                _current = n;
                invalidate();

                _currentChanged.send(_current);
            }

            break;
        }
    }

    return true;
}


bool TabBar::onTouchEvent(const TouchEvent& ev)
{
    Base::onTouchEvent(ev);

    if( ! ev.isPress() )
        return true;

    for(std::size_t n = 0; n != _tabs.size(); ++n)
    {
        TabItem& t = _tabs[n];
        
        if( t.geometry().contains( ev.position() ) )
        {
            if(n != _current)
            {
                if(_current < _tabs.size() )
                    _tabs.at(_current).setPressed(false);
                
                t.setPressed(true);

                _current = n;
                invalidate();

                _currentChanged.send(_current);
            }

            break;
        }
    }

    return true;
}

//////////////////////////////////////////////////////////////////////////
// TabView
//////////////////////////////////////////////////////////////////////////

TabView::TabView()
: _hasBackground(false)
, _hasFrame(false)
{
    _tabBar.currentChanged() += Pt::slot(_stack, &StackLayout::setCurrent);
    _stack.controlRemoved() += Pt::slot(_tabBar, &TabBar::removeTab);

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


void TabView::addTab(Control& control, const Pt::String& title)
{
    _tabBar.addTab(title);
    _stack.addItem(control);
}


void TabView::removeTab(std::size_t n)
{
    Control* control = _stack.controlAt(n);
    if( ! control )
        return;

    // _stack.controlRemoved is wired to _tabBar.removeTab,
    // so removing from the stack automatically removes the tab title
    _stack.removeItem(*control);
}


std::size_t TabView::current() const
{
    return _tabBar.current();
}


void TabView::setCurrent(std::size_t n)
{
    // TabBar::currentChanged is wired to StackLayout::setCurrent,
    // so only drive the tab bar; the signal handles the stack.
    _tabBar.setCurrent(n);
}


void TabView::setText(std::size_t n, const Pt::String& title)
{
    _tabBar.setText(n, title);
}


void TabView::setBackground(const Gfx::Brush& b)
{
    _tabViewOptions.setBackground(b);
    _hasBackground = true;
    invalidate();
}


void TabView::setBackground(bool enable)
{
    _hasBackground = enable;
    invalidate();
}


void TabView::setContour(const Gfx::Pen& p)
{
    _tabViewOptions.setContour(p);
    _hasFrame = true;
    invalidate();
}


void TabView::setFrame(bool enable)
{
    _hasFrame = enable;
    invalidate();
}


void TabView::setFont(const Gfx::Font& font)
{
    _tabViewOptions.setFont(font);
    _tabBar.setFont(font);
    invalidate();
}


void TabView::setFontSize(std::size_t size)
{
    _tabViewOptions.setFontSize(size);
    _tabBar.setFontSize(size);
    invalidate();
}


void TabView::setFontWeight(Gfx::Font::Weight weight)
{
    _tabViewOptions.setFontWeight(weight);
    _tabBar.setFontWeight(weight);
    invalidate();
}


void TabView::setFontSlant(Gfx::Font::Slant slant)
{
    _tabViewOptions.setFontSlant(slant);
    _tabBar.setFontSlant(slant);
    invalidate();
}


void TabView::setTextColor(const Gfx::Color& color)
{
    _tabViewOptions.setTextColor(color);
    _tabBar.setTextColor(color);
    invalidate();
}


void TabView::setRenderer(TabViewRenderer* renderer)
{
    const StyleOptions& options = Application::instance().styleOptions();

    if(renderer)
        _tabViewStyle.bind(*renderer, options, _tabViewOptions);
    else
        _tabViewStyle.bind(Application::instance().style(), options, _tabViewOptions);

    _tabBar.setRenderer(renderer);
    invalidate();
}


void TabView::onInvalidate()
{
    const StyleOptions& options = Application::instance().styleOptions();
    _tabViewStyle.rebind(Application::instance().style(), options, _tabViewOptions);

    if( ! _tabViewStyle.renderer() )
        return;

    Base::onInvalidate();
    relayout();
}


Gfx::SizeF TabView::onMeasure(const SizePolicy& policy)
{
    Base::onMeasure(policy);

    _layout.measure(policy);
    return _layout.preferredSize();
}


void TabView::onLayout(const Gfx::RectF& rect)
{
    Base::onLayout(rect);

    Gfx::PointF pos(padding().left() + _layout.margin().left(), 
                    padding().top()  + _layout.margin().top());

    double hspace = padding().leftRight() + _layout.margin().leftRight();
    double vspace = padding().topBottom() + _layout.margin().topBottom();

    Gfx::SizeF size;
    size.setWidth( rect.width() - hspace );
    size.setHeight( rect.height() - vspace );

    _layout.move(pos);
    _layout.resize(size);
}


void TabView::onPaint(PaintContext& context, const Gfx::RectF& rect)
{
    Base::onPaint(context, rect);

    TabViewRenderer* renderer = _tabViewStyle.renderer();
    if( ! renderer )
        return;

    TabViewState state;
    state.setEnabled( isEnabled() );
    state.setFocused( hasFocus() );

    // Content panel rect (below tab bar)
    Gfx::RectF contentRect = _stack.geometry();

    if( _hasBackground )
        onPaintBackground(context, contentRect, state);

    if( _hasFrame )
        onPaintChrome(context, contentRect, _tabBar.currentTabRect(), state);
}


void TabView::onPaintBackground(PaintContext& context,
                                const Gfx::RectF& contentRect,
                                const TabViewState& state)
{
    TabViewRenderer* renderer = _tabViewStyle.renderer();
    if( ! renderer )
        return;

    renderer->renderBackground(context, contentRect, state);
}


void TabView::onPaintChrome(PaintContext& context,
                            const Gfx::RectF& contentRect,
                            const Gfx::RectF& activeTabRect,
                            const TabViewState& state)
{
    TabViewRenderer* renderer = _tabViewStyle.renderer();
    if( ! renderer )
        return;

    renderer->renderChrome(context, contentRect, activeTabRect, state);
}

} // namespace

} // namespace
