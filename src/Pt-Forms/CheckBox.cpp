/* Copyright (C) 2016 Marc Boris Duerner 
   Copyright (C) 2016 Laurentiu-Gheorghe Crisan
  
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

#include <Pt/Forms/CheckBox.h>
#include <Pt/Forms/Application.h>
#include <Pt/Forms/Style.h>
#include <Pt/Forms/StyleOptions.h>
#include <Pt/Forms/StyleFlags.h>
#include <Pt/Forms/PaintContext.h>
#include <Pt/Forms/Painter.h>

#include <algorithm>
#include <cmath>

namespace Pt {

namespace Forms {

CheckBox::CheckBox()
: _state(Unchecked)
, _customRenderer(false)
, _styleGeneration(0)
, _overrideFlags(0)
{
}


CheckBox::~CheckBox()
{
}


CheckBox::State CheckBox::state() const
{
    return _state;
}


void CheckBox::setState(State s)
{
    _state = s;
    invalidate();
}


bool CheckBox::isChecked() const
{
    return _state == Checked;
}


const Gfx::Brush& CheckBox::background() const
{
    if( _renderer && _renderer->background() )
        return *_renderer->background();

    return Application::instance().styleOptions().textBackground();
}


void CheckBox::setBackground(const Gfx::Brush& b)
{
    _background.reset( new Gfx::Brush(b) );
    _overrideFlags |= OverrideBackground;

    if( CheckBoxRenderer* renderer = getRenderer() )
        renderer->setBackground(*_background);

    invalidate();
}


const Gfx::Pen& CheckBox::contour() const
{
    if( _renderer && _renderer->contour() )
        return *_renderer->contour();

    return Application::instance().styleOptions().contour();
}


void CheckBox::setContour(const Gfx::Pen& p)
{
    _contour.reset( new Gfx::Pen(p) );
    _overrideFlags |= OverrideContour;

    if( CheckBoxRenderer* renderer = getRenderer() )
        renderer->setContour(*_contour);

    invalidate();
}


const Gfx::Color& CheckBox::textColor() const
{
    if( _renderer )
        return _renderer->textColor();

    return Application::instance().styleOptions().textColor();
}


void CheckBox::setTextColor(const Gfx::Color& color)
{
    _textColor.reset( new Gfx::Color(color) );
    _overrideFlags |= OverrideTextColor;

    if( CheckBoxRenderer* renderer = getRenderer() )
        renderer->setTextColor( Gfx::Pen(*_textColor) );

    invalidate();
}


const Gfx::Font& CheckBox::font() const
{
    if( _renderer )
        return _renderer->font();

    return Application::instance().styleOptions().font();
}


void CheckBox::setFont(const Gfx::Font& font)
{
    _customFont = font;
    _overrideFlags |= OverrideFontAll;

    if( CheckBoxRenderer* renderer = getRenderer() )
        renderer->setFont( getFont() );

    invalidate();
}


Gfx::Font CheckBox::getFont() const
{
    const Gfx::Font& base = Application::instance().styleOptions().font();

    if( ! (_overrideFlags & OverrideFontAny) )
        return base;

    if( _overrideFlags & OverrideFontAll )
        return _customFont;

    std::size_t sz = (_overrideFlags & OverrideFontSize) ? _customFont.size()
                                                        : base.size();
    Gfx::Font::Weight wt = (_overrideFlags & OverrideFontWeight) ? _customFont.weight()
                                                                 : base.weight();
    Gfx::Font::Slant sl = (_overrideFlags & OverrideFontSlant) ? _customFont.slant()
                                                               : base.slant();

    if( base.hasStyleName() )
        return Gfx::Font(base.family(), sz, base.styleName(), wt, sl, base.stretch());

    if( base.category() != Gfx::Font::Category::None )
        return Gfx::Font(base.category(), sz, wt, sl, base.stretch());

    return Gfx::Font(base.family(), sz, wt, sl, base.stretch());
}


void CheckBox::setFontSize(std::size_t size)
{
    _customFont = _customFont.withSize(size);
    _overrideFlags |= OverrideFontSize;

    if( CheckBoxRenderer* renderer = getRenderer() )
        renderer->setFont( getFont() );

    invalidate();
}


void CheckBox::setFontWeight(Gfx::Font::Weight weight)
{
    _customFont = _customFont.withWeight(weight);
    _overrideFlags |= OverrideFontWeight;

    if( CheckBoxRenderer* renderer = getRenderer() )
        renderer->setFont( getFont() );

    invalidate();
}


void CheckBox::setFontSlant(Gfx::Font::Slant slant)
{
    _customFont = _customFont.withSlant(slant);
    _overrideFlags |= OverrideFontSlant;

    if( CheckBoxRenderer* renderer = getRenderer() )
        renderer->setFont( getFont() );

    invalidate();
}


void CheckBox::setRenderer(CheckBoxRenderer* renderer)
{
    _renderer.reset(renderer);
    _customRenderer = renderer != 0;

    if( renderer )
        applyRenderer(renderer);

    invalidate();
}


CheckBoxRenderer* CheckBox::getRenderer()
{
    if( ! _renderer )
    {
        const Style& style = Application::instance().style();
        CheckBoxRenderer* proto = style.get<CheckBoxRenderer>();
        if( ! proto )
            return 0;

        _renderer.reset( proto->create() );
    }

    return _renderer.get();
}


void CheckBox::applyRenderer(CheckBoxRenderer* renderer)
{
    if( _overrideFlags & OverrideBackground )
        renderer->setBackground(*_background);

    if( _overrideFlags & OverrideContour )
        renderer->setContour(*_contour);

    if( _overrideFlags & OverrideTextColor )
        renderer->setTextColor( Gfx::Pen(*_textColor) );

    if( _overrideFlags & OverrideFontAny )
        renderer->setFont( getFont() );
}


CheckBoxStyleFlags CheckBox::checkBoxStyleFlags() const
{
    StyleFlags common;

    if( isEnabled() )
        common.set(StyleFlags::Enabled);
    else
        common.set(StyleFlags::Disabled);

    if( isHighlighted() )
        common.set(StyleFlags::Highlighted);

    if( hasFocus() )
        common.set(StyleFlags::Focused);

    CheckBoxStyleFlags state(common);

    if( _state == Checked )
        state.set(CheckBoxStyleFlags::Checked);

    return state;
}


void CheckBox::onPressed()
{
    Base::onPressed();
}


void CheckBox::onReleased()
{
    Base::onReleased();

    State changedState = _state == Checked ? Unchecked
                                           : Checked;
    setState(changedState);

    clicked().send();
}


void CheckBox::onCanceled()
{
    Base::onCanceled();
}


void CheckBox::onInvalidate()
{
    std::size_t gen = Application::instance().styleOptions().generation();
    if( _styleGeneration != gen )
    {
        _styleGeneration = gen;
        if( ! _customRenderer )
            _renderer.reset();
    }

    if( ! _renderer )
    {
        bool hasOverride = (_overrideFlags != 0);
        if( hasOverride )
        {
            if( CheckBoxRenderer* renderer = getRenderer() )
                applyRenderer(renderer);
        }
        else
        {
            _renderer.reset( Application::instance().style().get<CheckBoxRenderer>() );
        }
    }

    if( ! _renderer )
        return;

    Base::onInvalidate();

    relayout();
}


Gfx::SizeF CheckBox::onMeasure(const SizePolicy& /*policy*/)
{
    if( ! _renderer )
        return Gfx::SizeF(0, 0);

    const Painter& painter = _renderer->textPainter( surface() );
    Gfx::TextMetrics tm = painter.textMetrics( text() );
    Gfx::FontMetrics fm = painter.fontMetrics();

    Gfx::SizeF textSize(tm.advance(), fm.lineHeight());
    Gfx::SizeF indicatorSize = _renderer->measureIndicator(surface());
    Gfx::SizeF contentSize = _renderer->measureContent(surface(), indicatorSize, textSize);
    Gfx::SizeF totalSize = _renderer->measureFrame(surface(), contentSize);

    return Gfx::SizeF(totalSize.width() + padding().leftRight(),
                      totalSize.height() + padding().topBottom());
}


void CheckBox::onLayout(const Gfx::RectF& rect)
{
    Base::onLayout(rect);

    if( ! _renderer )
        return;

    Gfx::RectF insetRect( Gfx::PointF(padding().left(), padding().top()),
                          Gfx::SizeF(size().width() - padding().leftRight(),
                                     size().height() - padding().topBottom()) );

    Gfx::RectF contentRect = _renderer->layoutFrame(surface(), insetRect);

    const Painter& painter = _renderer->textPainter(surface());
    Gfx::FontMetrics fm = painter.fontMetrics();
    Gfx::TextMetrics tm = painter.textMetrics( text() );

    Gfx::SizeF textSize(tm.advance(), fm.lineHeight());
    Gfx::SizeF indicatorSize = _renderer->measureIndicator(surface());

    _renderer->layoutContent(surface(), contentRect, indicatorSize, textSize,
                             _boxRect, _textRect);

    const Gfx::Scaling& scaling = surface().scaling();
    double textY = scaling.align(_textRect.y() + _textRect.height() / 2.0) + fm.capHeight() / 2.0;

    _textPos = scaling.align( Gfx::PointF(_textRect.x(), textY) );

    String::size_type mnIdx = String::npos;
    const Char* m = mnemonic();
    if( m )
        mnIdx = text().find(*m);

    _mnemonicRect = _renderer->layoutMnemonic(surface(), text(), _textPos,
                                               fm, mnIdx);

    repaint();
}


void CheckBox::onPaint(PaintContext& context, const Gfx::RectF& /*rect*/)
{
    if( ! _renderer )
        return;

    Gfx::RectF widgetRect( size() );
    CheckBoxStyleFlags state = checkBoxStyleFlags();

    _renderer->renderIndicator(context, widgetRect, _boxRect, state);
    _renderer->renderText(context, _textRect, text(), _textPos, state);
    _renderer->renderMnemonic(context, widgetRect, _mnemonicRect, state);
}

} // namespace

} // namespace
