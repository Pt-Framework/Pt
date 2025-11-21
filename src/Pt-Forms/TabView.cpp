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
#include <Pt/Gfx/Painter.h>

namespace Pt {

namespace Forms {

//////////////////////////////////////////////////////////////////////////
// TabBar
//////////////////////////////////////////////////////////////////////////

TabBar::TabBar()
: _current( static_cast<std::size_t>(-1) )
, _hasRenderer(false)
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


void TabBar::setRenderer(TabViewRenderer* renderer)
{
    _renderer.reset(renderer);
    _hasRenderer = renderer != 0;

    invalidate();
}


void TabBar::onInvalidate()
{
    Base::onInvalidate();

    const StyleOptions& options = Application::instance().styleOptions();
    const Style& style = Application::instance().style();

    _backgroundBrush = options.background();
    _foregroundBrush = options.foreground();
    _contourPen = options.contour();
    _textPen = options.textColor();
    _font = options.font();

    if( ! _hasRenderer )
        _renderer.reset( style.get<TabViewRenderer>() );
    
    if( ! _renderer )
        return;

    _renderer->prepareTabs(*this, options, 
                           _backgroundBrush, _foregroundBrush, _contourPen, 
                           _font, _textPen);
}


Gfx::SizeF TabBar::onMeasure(const SizePolicy& policy)
{
    if( ! _renderer)
        return Gfx::SizeF();

    return _renderer->measureTabs(surface(), _tabs, _font);
}


void TabBar::onLayout(const Gfx::RectF& rect)
{
    Base::onLayout(rect);

    if( ! _renderer )
        return;

    _renderer->layoutTabs(surface(), _tabs, rect, _font);
}


void TabBar::onPaint(PaintSurface& surface, const Gfx::RectF& rect)
{
    const StyleOptions& options = Application::instance().styleOptions();

    if( ! _renderer)
        return;

    Gfx::Painter painter(surface);
    painter.setClip(rect);

    _renderer->renderTabs(_tabs, options, painter, rect,
                          _backgroundBrush, _foregroundBrush, _contourPen, 
                          _font, _textPen);
}


bool TabBar::onMouseEvent(const MouseEvent& ev)
{
    Base::onMouseEvent(ev);

    if( ! ev.isPress() )
        return true;

    std::vector<TabItem>::iterator it;
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

    std::vector<TabItem>::iterator it;
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
: _hasRenderer(false)
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
    _tabBar.removeTab(n);

    Control* control = _stack.controlAt(n);
    if( ! control )
        return;

    _stack.removeItem(*control);
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


void TabView::setText(std::size_t n, const Pt::String& title)
{
    _tabBar.setText(n, title);
}


void TabView::setRenderer(TabViewRenderer* renderer)
{
    _renderer.reset(renderer);
    _hasRenderer = renderer != 0;

    invalidate();
}


void TabView::onInvalidate()
{
    Base::onInvalidate();

    const StyleOptions& options = Application::instance().styleOptions();
    const Style& style = Application::instance().style();

    _backgroundBrush = options.background();
    _foregroundBrush = options.foreground();
    _contourPen = options.contour();

    if( ! _hasRenderer )
        _renderer.reset( style.get<TabViewRenderer>() );

    if( ! _renderer )
        return;

    _renderer->prepare(*this, options, 
                       _backgroundBrush, _foregroundBrush, _contourPen);
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


void TabView::onPaint(PaintSurface& surface, const Gfx::RectF& rect)
{
    Base::onPaint(surface, rect);

    const StyleOptions& options = Application::instance().styleOptions();

    if( ! _renderer)
        return;

    Gfx::Painter painter(surface);
    painter.setClip(rect);

    _renderer->render(*this, options, painter, rect,
                      _backgroundBrush, _foregroundBrush, _contourPen);
}

} // namespace

} // namespace
