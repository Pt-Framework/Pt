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
, _customRenderer(false)
, _fontOverride(0)
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
    _customFont.reset( new Gfx::Font(font) );
    _fontOverride = OverrideAll;

    if( TabViewRenderer* renderer = getRenderer() )
        renderer->setFont( getFont() );

    invalidate();
}


void TabBar::setFontSize(std::size_t size)
{
    _fontOverride |= OverrideSize;

    Gfx::Font f = _customFont ? *_customFont
                               : Application::instance().styleOptions().font();
    _customFont.reset( new Gfx::Font(f.withSize(size)) );

    if( TabViewRenderer* renderer = getRenderer() )
        renderer->setFont( getFont() );

    invalidate();
}


void TabBar::setFontWeight(Gfx::Font::Weight weight)
{
    _fontOverride |= OverrideWeight;

    Gfx::Font f = _customFont ? *_customFont
                               : Application::instance().styleOptions().font();
    _customFont.reset( new Gfx::Font(f.withWeight(weight)) );

    if( TabViewRenderer* renderer = getRenderer() )
        renderer->setFont( getFont() );

    invalidate();
}


void TabBar::setFontSlant(Gfx::Font::Slant slant)
{
    _fontOverride |= OverrideSlant;

    Gfx::Font f = _customFont ? *_customFont
                               : Application::instance().styleOptions().font();
    _customFont.reset( new Gfx::Font(f.withSlant(slant)) );

    if( TabViewRenderer* renderer = getRenderer() )
        renderer->setFont( getFont() );

    invalidate();
}


void TabBar::setTextColor(const Gfx::Color& color)
{
    _textColor.reset( new Gfx::Color(color) );

    if( TabViewRenderer* renderer = getRenderer() )
        renderer->setTextColor( Gfx::Pen(color) );

    invalidate();
}


void TabBar::setRenderer(TabViewRenderer* renderer)
{
    _renderer.reset(renderer);
    _customRenderer = renderer != 0;

    if(renderer)
        applyRenderer(renderer);

    invalidate();
}


TabViewRenderer* TabBar::getRenderer()
{
    if( ! _customRenderer )
    {
        const Style& style = Application::instance().style();
        TabViewRenderer* proto = style.get<TabViewRenderer>();
        if( ! proto )
            return 0;

        _renderer.reset( proto->create() );
        _customRenderer = true;
    }

    return _renderer.get();
}


void TabBar::applyRenderer(TabViewRenderer* renderer)
{
    if( ! renderer )
        return;

    if( _customFont )
        renderer->setFont( getFont() );

    if( _textColor )
        renderer->setTextColor( Gfx::Pen(*_textColor) );
}


Gfx::Font TabBar::getFont() const
{
    const Gfx::Font& base = Application::instance().styleOptions().font();

    if( _fontOverride == 0 )
        return base;

    if( _fontOverride == OverrideAll )
        return *_customFont;

    std::size_t sz = (_fontOverride & OverrideSize) ? _customFont->size()
                                                    : base.size();
    Gfx::Font::Weight wt = (_fontOverride & OverrideWeight) ? _customFont->weight()
                                                            : base.weight();
    Gfx::Font::Slant sl = (_fontOverride & OverrideSlant) ? _customFont->slant()
                                                          : base.slant();

    if( base.hasStyleName() )
        return Gfx::Font(base.family(), sz, base.styleName(), wt, sl, base.stretch());

    if( base.category() != Gfx::Font::Category::None )
        return Gfx::Font(base.category(), sz, wt, sl, base.stretch());

    return Gfx::Font(base.family(), sz, wt, sl, base.stretch());
}


void TabBar::onInvalidate()
{
    if( ! _renderer )
    {
        bool hasOverride = _customFont || _textColor;
        if( hasOverride )
        {
            if( TabViewRenderer* renderer = getRenderer() )
                applyRenderer(renderer);
        }
        else
        {
            _renderer.reset( Application::instance().style().get<TabViewRenderer>() );
        }
    }

    if( ! _renderer )
        return;

    Base::onInvalidate();
    relayout();
}


Gfx::SizeF TabBar::onMeasure(const SizePolicy& policy)
{
    if( ! _renderer )
        return Gfx::SizeF();

    // Measure total size by summing individual tab sizes
    Gfx::SizeF total;

    for(std::size_t i = 0; i < _tabs.size(); ++i)
    {
        Gfx::SizeF tabSize = _renderer->measureTab( surface(), _tabs[i].text() );
        total.addWidth( tabSize.width() );

        if( tabSize.height() > total.height() )
            total.setHeight( tabSize.height() );
    }

    return total;
}


void TabBar::onLayout(const Gfx::RectF& rect)
{
    Base::onLayout(rect);

    if( ! _renderer )
        return;

    Gfx::PointF tabPos;

    for(std::size_t i = 0; i < _tabs.size(); ++i)
    {
        Gfx::SizeF tabSize = _renderer->measureTab( surface(), _tabs[i].text() );
        
        Gfx::RectF tabRect;
        tabRect.setOrigin(tabPos);
        tabRect.setWidth( tabSize.width() );
        tabRect.setHeight( rect.height() );

        _tabs[i].setGeometry(tabRect);
        tabPos.addX( tabSize.width() );
    }
}


void TabBar::onPaint(PaintContext& context, const Gfx::RectF& rect)
{
    if( ! _renderer )
        return;

    const Painter& painter = _renderer->textPainter( surface() );

    for(std::size_t i = 0; i < _tabs.size(); ++i)
    {
        const TabItem& tab = _tabs[i];
        Gfx::RectF tabRect = tab.geometry();

        // Compute text position from layout rect
        Gfx::RectF textRect = _renderer->layoutTab( surface(), tabRect );
        Gfx::FontMetrics fontMet = painter.fontMetrics();
        double textY = textRect.y()
                     + (textRect.height() - fontMet.height()) / 2.0
                     + fontMet.ascent();
        Gfx::PointF textPos(textRect.x(), textY);

        // Build per-tab flags
        TabItemStyleFlags itemState;
        if( isEnabled() )
            itemState.set(StyleFlags::Enabled);
        else
            itemState.set(StyleFlags::Disabled);

        if( tab.isPressed() )
            itemState.set(TabItemStyleFlags::Active);

        _renderer->renderTab(context, tabRect, tab.text(), textPos, itemState);
    }
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
: _customRenderer(false)
, _fontOverride(0)
, _hasBackground(false)
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
    _background.reset( new Gfx::Brush(b) );
    _hasBackground = true;

    if( TabViewRenderer* renderer = getRenderer() )
        renderer->setBackground(*_background);

    invalidate();
}


void TabView::setBackground(bool enable)
{
    _hasBackground = enable;
    invalidate();
}


void TabView::setContour(const Gfx::Pen& p)
{
    _contour.reset( new Gfx::Pen(p) );
    _hasFrame = true;

    if( TabViewRenderer* renderer = getRenderer() )
        renderer->setContour(*_contour);

    invalidate();
}


void TabView::setFrame(bool enable)
{
    _hasFrame = enable;
    invalidate();
}


void TabView::setFont(const Gfx::Font& font)
{
    _customFont.reset( new Gfx::Font(font) );
    _fontOverride = OverrideAll;

    _tabBar.setFont(font);
    invalidate();
}


void TabView::setFontSize(std::size_t size)
{
    _fontOverride |= OverrideSize;

    Gfx::Font f = _customFont ? *_customFont
                               : Application::instance().styleOptions().font();
    _customFont.reset( new Gfx::Font(f.withSize(size)) );

    _tabBar.setFontSize(size);
    invalidate();
}


void TabView::setFontWeight(Gfx::Font::Weight weight)
{
    _fontOverride |= OverrideWeight;

    Gfx::Font f = _customFont ? *_customFont
                               : Application::instance().styleOptions().font();
    _customFont.reset( new Gfx::Font(f.withWeight(weight)) );

    _tabBar.setFontWeight(weight);
    invalidate();
}


void TabView::setFontSlant(Gfx::Font::Slant slant)
{
    _fontOverride |= OverrideSlant;

    Gfx::Font f = _customFont ? *_customFont
                               : Application::instance().styleOptions().font();
    _customFont.reset( new Gfx::Font(f.withSlant(slant)) );

    _tabBar.setFontSlant(slant);
    invalidate();
}


void TabView::setTextColor(const Gfx::Color& color)
{
    _textColor.reset( new Gfx::Color(color) );

    _tabBar.setTextColor(color);
    invalidate();
}


void TabView::setRenderer(TabViewRenderer* renderer)
{
    _renderer.reset(renderer);
    _customRenderer = renderer != 0;

    if(renderer)
        applyRenderer(renderer);

    _tabBar.setRenderer(renderer);
    invalidate();
}


TabViewRenderer* TabView::getRenderer()
{
    if( ! _customRenderer )
    {
        const Style& style = Application::instance().style();
        TabViewRenderer* proto = style.get<TabViewRenderer>();
        if( ! proto )
            return 0;

        _renderer.reset( proto->create() );
        _customRenderer = true;
    }

    return _renderer.get();
}


void TabView::applyRenderer(TabViewRenderer* renderer)
{
    if( ! renderer )
        return;

    if( _background )
        renderer->setBackground(*_background);

    if( _contour )
        renderer->setContour(*_contour);

    if( _customFont )
        renderer->setFont( getFont() );

    if( _textColor )
        renderer->setTextColor( Gfx::Pen(*_textColor) );
}


Gfx::Font TabView::getFont() const
{
    const Gfx::Font& base = Application::instance().styleOptions().font();

    if( _fontOverride == 0 )
        return base;

    if( _fontOverride == OverrideAll )
        return *_customFont;

    std::size_t sz = (_fontOverride & OverrideSize) ? _customFont->size()
                                                    : base.size();
    Gfx::Font::Weight wt = (_fontOverride & OverrideWeight) ? _customFont->weight()
                                                            : base.weight();
    Gfx::Font::Slant sl = (_fontOverride & OverrideSlant) ? _customFont->slant()
                                                          : base.slant();

    if( base.hasStyleName() )
        return Gfx::Font(base.family(), sz, base.styleName(), wt, sl, base.stretch());

    if( base.category() != Gfx::Font::Category::None )
        return Gfx::Font(base.category(), sz, wt, sl, base.stretch());

    return Gfx::Font(base.family(), sz, wt, sl, base.stretch());
}


TabViewStyleFlags TabView::tabViewStyleFlags() const
{
    StyleFlags base;

    if( isEnabled() )
        base.set(StyleFlags::Enabled);
    else
        base.set(StyleFlags::Disabled);

    if( hasFocus() )
        base.set(StyleFlags::Focused);

    return TabViewStyleFlags(base);
}


void TabView::onInvalidate()
{
    if( ! _renderer )
    {
        bool hasOverride = _background || _contour || _customFont || _textColor;
        if( hasOverride )
        {
            if( TabViewRenderer* renderer = getRenderer() )
                applyRenderer(renderer);
        }
        else
        {
            _renderer.reset( Application::instance().style().get<TabViewRenderer>() );
        }
    }

    if( ! _renderer )
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

    if( ! _renderer )
        return;

    TabViewStyleFlags state = tabViewStyleFlags();

    // Content panel rect (below tab bar)
    Gfx::RectF contentRect = _stack.geometry();

    if( _hasBackground )
        _renderer->renderBackground(context, contentRect, state);

    if( _hasFrame )
        _renderer->renderFrame(context, contentRect, _tabBar.currentTabRect(), state);
}

} // namespace

} // namespace
