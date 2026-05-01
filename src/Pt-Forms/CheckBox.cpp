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
#include <Pt/Forms/PixmapSurface.h>
#include <Pt/Forms/PaintContext.h>
#include <Pt/Forms/Painter.h>
#include <Pt/Gfx/Painter.h>

namespace Pt {

namespace Forms {

CheckBox::CheckBox()
: _state(Unchecked)
, _hasRenderer(false)
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
    return _background ? *_background
                       : Application::instance().styleOptions().textBackground();
}


void CheckBox::setBackground(const Gfx::Brush& b)
{
    _background.reset( new Gfx::Brush(b) );
    invalidate();
}


const Gfx::Pen& CheckBox::contour() const
{
    return _contour ? *_contour
                    : Application::instance().styleOptions().contour();
}


void CheckBox::setContour(const Gfx::Pen& p)
{
    _contour.reset( new Gfx::Pen(p) );
    invalidate();
}


const Gfx::Color& CheckBox::textColor() const
{
    return _textColor ? *_textColor
                      : Application::instance().styleOptions().textColor();
}


void CheckBox::setTextColor(const Gfx::Color& color)
{
    _textColor.reset( new Gfx::Color(color) );
    invalidate();
}


const Gfx::Font& CheckBox::font() const
{
    return _font;
}


void CheckBox::setFont(const Gfx::Font& font)
{
    _customFont = font;
    _fontOverride = OverrideAll;

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

    invalidate();
}


void CheckBox::setFontWeight(Gfx::Font::Weight weight)
{
    _customFont = Gfx::Font(_customFont.family(), _customFont.size(),
                            weight, _customFont.slant(),
                            _customFont.stretch());
    _fontOverride |= OverrideWeight;

    invalidate();
}


void CheckBox::setFontSlant(Gfx::Font::Slant slant)
{
    _customFont = Gfx::Font(_customFont.family(), _customFont.size(),
                            _customFont.weight(), slant,
                            _customFont.stretch());
    _fontOverride |= OverrideSlant;

    invalidate();
}


void CheckBox::setRenderer(CheckBoxRenderer* renderer)
{
    _renderer.reset(renderer);
    _hasRenderer = renderer != 0;

    invalidate();
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


Gfx::SizeF CheckBox::onMeasure(PaintContext& ctx, const SizePolicy& policy)
{
    //Gfx::TextMetrics tm = PixmapSurface::textMetrics( _font, text() );

    Painter painter( ctx );
    painter.setFont(_font);
    Gfx::TextMetrics tm = painter.textMetrics( text() );
    Gfx::FontMetrics fm = painter.fontMetrics();

    double space = std::min<double>(_boxSize.width() / 2, _font.size() / 2);
    double boxWidth = _boxSize.width();
    double boxHeight = _boxSize.height();

    double itemsWidth = space + boxWidth + space + tm.advance();
    double itemsHeight = std::max<double>(fm.lineHeight(), boxHeight);

    return Gfx::SizeF( itemsWidth + padding().leftRight(), 
                       itemsHeight + padding().topBottom() );
}


void CheckBox::onInvalidate()
{
    Base::onInvalidate();

    const StyleOptions& options = Application::instance().styleOptions();
    const Style& style = Application::instance().style();

    _brush = background();
    _pen = contour();
    _textPen = textColor();
    _font = getFont();

    if( ! _hasRenderer )
        _renderer.reset( style.get<CheckBoxRenderer>() );
    
    if( ! _renderer )
        return;

    _renderer->prepare(*this, options, _brush, _pen, _font, _textPen, _boxSize);
}


void CheckBox::onPaint(PaintContext& context, const Gfx::RectF& rect)
{
    const StyleOptions& options = Application::instance().styleOptions();

    if( ! _renderer )
        return;

    Painter painter(context);
    painter.setClip(rect);

    const Gfx::Scaling& scaling = this->scaling();

    double space = std::min<double>(_boxSize.width() / 2, _font.size() / 1.5);

    double boxX = space;
    double boxY = size().height() / 2.0 - _boxSize.height() / 2.0;
    Gfx::RectF boxRect(Gfx::PointF(boxX, boxY), _boxSize);

    _renderer->renderBox(*this, options, painter, rect, 
                          boxRect, _brush, _pen);

    painter.setFont(_font);
    Gfx::TextMetrics tm = painter.textMetrics( text() );
    Gfx::FontMetrics fontMet = painter.fontMetrics();

    double textX = space + _boxSize.width() + space;
    //double textY = size().height() / 2.0 - tm.height() / 2.0 + tm.ascent();
    double textY = scaling.align(size().height() / 2.0) + fontMet.capHeight() / 2.0;

    // NOTE: textY needs to be aligned to match the alignment of the box rect

    Gfx::PointF textPos(textX, textY);
    textPos = scaling.align(textPos);

    Gfx::RectF mnemonicRect;

    const Char* m = mnemonic();
    if(m)
    {
        String::size_type n = text().find(*m);
        if(n != String::npos)
        {
            Pt::String mnemonicText(text(), 0, n);
            Gfx::TextMetrics fmLeft = painter.textMetrics(mnemonicText);

            mnemonicText = *m;
            Gfx::TextMetrics fmChar = painter.textMetrics(mnemonicText);

            mnemonicRect.set( Gfx::PointF(textPos.x() + fmLeft.advance(), 
                                          textPos.y() - fontMet.ascent()),
                              Gfx::SizeF(fmChar.advance(), 
                                         fontMet.height()) );
        }
    }

    _renderer->renderText(*this, options, painter, rect,
                          text(), textPos, tm, _font, _textPen,
                          mnemonicRect);
}

} // namespace

} // namespace
