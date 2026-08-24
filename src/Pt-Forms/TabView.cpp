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
// TabView
//////////////////////////////////////////////////////////////////////////

TabView::TabView()
: _current(StackLayout::NoIndex)
, _hasBackground(false)
, _hasFrame(false)
{
    _stack.controlRemoved() += Pt::slot(*this, &TabView::onControlRemoved);

    add(_stack);
}


TabView::~TabView()
{
}


std::size_t TabView::hitTab(const Gfx::PointF& pos) const
{
    if( ! _tabViewStyle.renderer() )
        return StackLayout::NoIndex;

    if( ! _tabBarRect.contains(pos) )
        return StackLayout::NoIndex;

    for(std::size_t n = 0; n != _tabs.size(); ++n)
    {
        if( _tabs[n].geometry().contains(pos) )
            return n;
    }

    return StackLayout::NoIndex;
}


Gfx::SizeF TabView::measureTabs(PaintSurface& surface,
                                TabViewRenderer& renderer)
{
    Gfx::SizeF total;

    for(std::size_t i = 0; i < _tabs.size(); ++i)
    {
        TabViewItem& tab = _tabs.at(i);
        Gfx::SizeF tabSize = renderer.measureTab(surface, tab.text());
        tab.setMeasuredSize(tabSize);

        total.addWidth( tabSize.width() );

        if( tabSize.height() > total.height() )
            total.setHeight( tabSize.height() );
    }

    return total;
}


void TabView::layoutTabs(PaintSurface& surface,
                         TabViewRenderer& renderer,
                         const Gfx::RectF& rect)
{
    _tabBarRect = rect;

    const Painter& painter = renderer.textPainter(surface);
    Gfx::FontMetrics fontMet = painter.fontMetrics();

    Gfx::PointF tabPos = rect.topLeft();

    for(std::size_t i = 0; i < _tabs.size(); ++i)
    {
        TabViewItem& tab = _tabs.at(i);
        const Gfx::SizeF& tabSize = tab.measuredSize();

        Gfx::RectF tabRect;
        tabRect.setOrigin(tabPos);
        tabRect.setWidth( tabSize.width() );
        tabRect.setHeight( rect.height() );

        tab.setGeometry(tabRect);

        Gfx::RectF textRect = renderer.layoutTab(surface, tabRect);
        double textY = textRect.y()
                     + (textRect.height() - fontMet.height()) / 2.0
                     + fontMet.ascent();
        tab.setTextPos( Gfx::PointF(textRect.x(), textY) );

        tabPos.addX( tabSize.width() );
    }
}


void TabView::renderTabs(PaintContext& context,
                         TabViewRenderer& renderer,
                         bool enabled)
{
    for(std::size_t i = 0; i < _tabs.size(); ++i)
    {
        const TabViewItem& tab = _tabs.at(i);

        TabViewItemState itemState;
        itemState.setEnabled(enabled);
        itemState.setActive( tab.isPressed() );

        renderTab(context, renderer, tab.geometry(), tab.text(), tab.textPos(), itemState);
    }
}


void TabView::renderTab(PaintContext& context,
                        TabViewRenderer& renderer,
                        const Gfx::RectF& tabRect,
                        const Pt::String& text,
                        const Gfx::PointF& textPos,
                        const TabViewItemState& state)
{
    renderer.renderTab(context, tabRect, text, textPos, state);
}


const Gfx::RectF& TabView::currentTabRect() const
{
    static const Gfx::RectF empty;

    if( _current < _tabs.size() )
        return _tabs.at(_current).geometry();

    return empty;
}


void TabView::onProcessMouseEvent(const MouseEvent& ev)
{
    if( ! acceptsInput() )
        return;

    Gfx::PointF pos = fromGlobal( ev.position() );
    if( ev.isPress() )
    {
        std::size_t current = hitTab(pos);
        if(current != StackLayout::NoIndex)
        {
            setCurrent(current);
            return;
        }
    }

    Base::onProcessMouseEvent(ev);
}


void TabView::onProcessTouchEvent(const TouchEvent& ev)
{
    if( ! acceptsInput() )
        return;

    Gfx::PointF pos = fromGlobal( ev.position() );
    if( ev.isPress() )
    {
        std::size_t current = hitTab(pos);
        if(current != StackLayout::NoIndex)
        {
            setCurrent(current);
            return;
        }
    }

    Base::onProcessTouchEvent(ev);
}


bool TabView::empty() const
{
    return _tabs.empty();
}


std::size_t TabView::size() const
{
    return _tabs.size();
}


void TabView::addTab(Control& control, const Pt::String& title)
{
    TabViewItem tab;
    tab.setText(title);
    _tabs.push_back(tab);

    _stack.addItem(control);
    relayout();
}


void TabView::removeTab(std::size_t n)
{
    Control* control = _stack.controlAt(n);
    if( ! control )
        return;

    _stack.removeItem(*control);
}


std::size_t TabView::current() const
{
    return _current;
}


void TabView::setCurrent(std::size_t n)
{
    if( n >= _tabs.size() )
        return;

    if(_current == n)
        return;

    if( _current < _tabs.size() )
        _tabs.at(_current).setPressed(false);

    _tabs.at(n).setPressed(true);
    _current = n;

    repaint();
    _stack.setCurrent(n);
}


void TabView::setText(std::size_t n, const Pt::String& title)
{
    if( n >= _tabs.size() )
        return;

    _tabs.at(n).setText(title);
    relayout();
    repaint();
}


void TabView::onControlRemoved(std::size_t n)
{
    if( n >= _tabs.size() )
        return;

    if(_current == n)
        _current = StackLayout::NoIndex;
    else if(_current != StackLayout::NoIndex && n < _current)
        --_current;

    _tabs.erase(_tabs.begin() + n);
    relayout();
}


void TabView::setBackground(const Gfx::Brush& b)
{
    BackgroundOption background(b);
    _tabViewOptions.set(background);
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
    ContourOption contour(p);
    _tabViewOptions.set(contour);
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
    FontOption fontOption;
    fontOption.setFont(font);
    _tabViewOptions.set(fontOption);
    invalidate();
}


void TabView::setFontSize(std::size_t size)
{
    const FontOption* localFont = _tabViewOptions.findLocal<FontOption>();
    FontOption font = localFont ? *localFont : FontOption();
    font.setSize(size);
    _tabViewOptions.set(font);
    invalidate();
}


void TabView::setFontWeight(Gfx::Font::Weight weight)
{
    const FontOption* localFont = _tabViewOptions.findLocal<FontOption>();
    FontOption font = localFont ? *localFont : FontOption();
    font.setWeight(weight);
    _tabViewOptions.set(font);
    invalidate();
}


void TabView::setFontSlant(Gfx::Font::Slant slant)
{
    const FontOption* localFont = _tabViewOptions.findLocal<FontOption>();
    FontOption font = localFont ? *localFont : FontOption();
    font.setSlant(slant);
    _tabViewOptions.set(font);
    invalidate();
}


void TabView::setTextColor(const Gfx::Color& color)
{
    TextColorOption textColor(color);
    _tabViewOptions.set(textColor);
    invalidate();
}


void TabView::setAccentColor(const Gfx::Color& color)
{
    AccentColorOption accentColor(color);
    _tabViewOptions.set(accentColor);
    invalidate();
}


void TabView::setRenderer(TabViewRenderer* renderer)
{
    const StyleOptions& options = Application::instance().styleOptions();

    if(renderer)
        _tabViewStyle.bind(*renderer, options, _tabViewOptions);
    else
        _tabViewStyle.bind(Application::instance().style(), options, _tabViewOptions);

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

    TabViewRenderer* renderer = _tabViewStyle.renderer();
    Gfx::SizeF tabBarSize;
    if( renderer )
        tabBarSize = measureTabs(surface(), *renderer);

    SizePolicy stackPolicy(policy.horizontal(), policy.vertical());
    double stackWidth = policy.size().width() - padding().leftRight() - _stack.margin().leftRight();
    double stackHeight = policy.size().height() - padding().topBottom() - tabBarSize.height() - _stack.margin().topBottom();
    if( stackWidth < 0 )
        stackWidth = 0;
    if( stackHeight < 0 )
        stackHeight = 0;

    stackPolicy.setWidth(stackWidth);
    stackPolicy.setHeight(stackHeight);

    _stack.measure(stackPolicy);
    Gfx::SizeF stackSize = _stack.preferredSize();

    Gfx::SizeF preferred;
    preferred.setWidth(stackSize.width() + _stack.margin().leftRight());
    if( tabBarSize.width() > preferred.width() )
        preferred.setWidth(tabBarSize.width());

    preferred.setHeight(tabBarSize.height() + stackSize.height() + _stack.margin().topBottom());
    preferred.addWidth(padding().leftRight());
    preferred.addHeight(padding().topBottom());
    return preferred;
}


void TabView::onLayout(const Gfx::RectF& rect)
{
    Base::onLayout(rect);

    Gfx::PointF pos(padding().left(), padding().top());
    Gfx::SizeF size;
    size.setWidth( rect.width() - padding().leftRight() );
    size.setHeight( rect.height() - padding().topBottom() );

    if( size.width() < 0 )
        size.setWidth(0);
    if( size.height() < 0 )
        size.setHeight(0);

    TabViewRenderer* renderer = _tabViewStyle.renderer();
    double tabBarHeight = 0;
    _tabBarRect.clear();
    if( renderer )
    {
        Gfx::SizeF tabBarSize = measureTabs(surface(), *renderer);
        tabBarHeight = tabBarSize.height();

        Gfx::RectF tabBarRect(pos, Gfx::SizeF(size.width(), tabBarHeight));
        layoutTabs(surface(), *renderer, tabBarRect);
    }

    Gfx::PointF stackPos(pos.x() + _stack.margin().left(),
                         pos.y() + tabBarHeight + _stack.margin().top());
    Gfx::SizeF stackSize(size.width() - _stack.margin().leftRight(),
                         size.height() - tabBarHeight - _stack.margin().topBottom());
    if( stackSize.width() < 0 )
        stackSize.setWidth(0);
    if( stackSize.height() < 0 )
        stackSize.setHeight(0);

    _stack.move(stackPos);
    _stack.resize(stackSize);
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
        onPaintChrome(context, contentRect, currentTabRect(), state);

    renderTabs(context, *renderer, isEnabled());
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
