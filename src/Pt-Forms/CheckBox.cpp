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
, _fontOverride(0)
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
    _fontOverride = OverrideAll;

    if( CheckBoxRenderer* renderer = getRenderer() )
        renderer->setFont( getFont() );

    invalidate();
}


Gfx::Font CheckBox::getFont() const
{
    const Gfx::Font& base = Application::instance().styleOptions().font();

    if( _fontOverride == 0 )
        return base;

    if( _fontOverride == OverrideAll )
        return _customFont;

    std::size_t sz = (_fontOverride & OverrideSize) ? _customFont.size()
                                                    : base.size();
    Gfx::Font::Weight wt = (_fontOverride & OverrideWeight) ? _customFont.weight()
                                                            : base.weight();
    Gfx::Font::Slant sl = (_fontOverride & OverrideSlant) ? _customFont.slant()
                                                          : base.slant();

    if( base.hasStyleName() )
        return Gfx::Font(base.family(), sz, base.styleName(), wt, sl, base.stretch());

    if( base.category() != Gfx::Font::Category::None )
        return Gfx::Font(base.category(), sz, wt, sl, base.stretch());

    return Gfx::Font(base.family(), sz, wt, sl, base.stretch());
}


void CheckBox::setFontSize(std::size_t size)
{
    _customFont = Gfx::Font(_customFont.family(), size,
                            _customFont.weight(), _customFont.slant(),
                            _customFont.stretch());
    _fontOverride |= OverrideSize;

    if( CheckBoxRenderer* renderer = getRenderer() )
        renderer->setFont( getFont() );

    invalidate();
}


void CheckBox::setFontWeight(Gfx::Font::Weight weight)
{
    _customFont = Gfx::Font(_customFont.family(), _customFont.size(),
                            weight, _customFont.slant(),
                            _customFont.stretch());
    _fontOverride |= OverrideWeight;

    if( CheckBoxRenderer* renderer = getRenderer() )
        renderer->setFont( getFont() );

    invalidate();
}


void CheckBox::setFontSlant(Gfx::Font::Slant slant)
{
    _customFont = Gfx::Font(_customFont.family(), _customFont.size(),
                            _customFont.weight(), slant,
                            _customFont.stretch());
    _fontOverride |= OverrideSlant;

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
    if( ! _customRenderer )
    {
        const Style& style = Application::instance().style();
        CheckBoxRenderer* proto = style.get<CheckBoxRenderer>();
        if( ! proto )
            return 0;

        _renderer.reset( proto->create() );
        _customRenderer = true;
    }

    return _renderer.get();
}


void CheckBox::applyRenderer(CheckBoxRenderer* renderer)
{
    if( _background )
        renderer->setBackground(*_background);

    if( _contour )
        renderer->setContour(*_contour);

    if( _textColor )
        renderer->setTextColor( Gfx::Pen(*_textColor) );

    if( _fontOverride )
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
    if( ! _renderer )
    {
        bool hasOverride = _background || _contour || _textColor || _fontOverride;
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
    _textMetrics = painter.textMetrics( text() );
    _fontMetrics = painter.fontMetrics();

    _boxSize = _renderer->measureBox( surface() );

    double space = std::min<double>(_boxSize.width() / 2, _fontMetrics.height() / 2);
    double itemsWidth = space + _boxSize.width() + space + _textMetrics.advance();
    double itemsHeight = std::max<double>(_fontMetrics.lineHeight(), _boxSize.height());

    return Gfx::SizeF(itemsWidth + padding().leftRight(),
                      itemsHeight + padding().topBottom());
}


void CheckBox::onLayout(const Gfx::RectF& rect)
{
    Base::onLayout(rect);

    if( ! _renderer )
        return;

    const Gfx::Scaling& scaling = surface().scaling();

    double space = std::min<double>(_boxSize.width() / 2,
                                    _fontMetrics.height() / 2);

    double boxX = space;
    double boxY = size().height() / 2.0 - _boxSize.height() / 2.0;
    _boxRect.set( Gfx::PointF(boxX, boxY), _boxSize );

    double textX = space + _boxSize.width() + space;
    double textY = scaling.align(size().height() / 2.0)
                   + _fontMetrics.capHeight() / 2.0;

    _textPos = scaling.align( Gfx::PointF(textX, textY) );

    String::size_type mnIdx = String::npos;
    const Char* m = mnemonic();
    if( m )
        mnIdx = text().find(*m);

    _mnemonicRect = _renderer->layoutMnemonic(surface(), text(), _textPos,
                                               _fontMetrics, mnIdx);

    repaint();
}


void CheckBox::onPaint(PaintContext& context, const Gfx::RectF& /*rect*/)
{
    if( ! _renderer )
        return;

    Gfx::RectF widgetRect( size() );
    CheckBoxStyleFlags state = checkBoxStyleFlags();

    _renderer->renderBox(context, widgetRect, _boxRect, state);
    _renderer->renderText(context, widgetRect, text(), _textPos, state);
    _renderer->renderMnemonic(context, widgetRect, _mnemonicRect, state);
}

} // namespace

} // namespace
