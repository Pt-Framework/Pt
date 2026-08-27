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
#include <Pt/Forms/PaintContext.h>
#include <Pt/Forms/Painter.h>

#include <algorithm>
#include <cmath>

namespace Pt {

namespace Forms {

CheckBox::CheckBox()
: _state(Unchecked)
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
    return _styler.background();
}


void CheckBox::setBackground(const Gfx::Brush& b)
{
    _styler.setBackground(b);
    invalidate();
}


const Gfx::Pen& CheckBox::contour() const
{
    return _styler.contour();
}


void CheckBox::setContour(const Gfx::Pen& p)
{
    _styler.setContour(p);
    invalidate();
}


const Gfx::Color& CheckBox::textColor() const
{
    return _styler.textColor();
}


void CheckBox::setTextColor(const Gfx::Color& color)
{
    _styler.setTextColor(color);
    invalidate();
}


Gfx::Font CheckBox::font() const
{
    return _styler.font();
}


void CheckBox::setFont(const Gfx::Font& font)
{
    _styler.setFont(font);
    invalidate();
}


void CheckBox::setFontSize(std::size_t size)
{
    _styler.setFontSize(size);
    invalidate();
}


void CheckBox::setFontWeight(Gfx::Font::Weight weight)
{
    _styler.setFontWeight(weight);
    invalidate();
}


void CheckBox::setFontSlant(Gfx::Font::Slant slant)
{
    _styler.setFontSlant(slant);
    invalidate();
}


void CheckBox::setRenderer(CheckBoxRenderer* renderer)
{
    const Style& style = Application::instance().style();
    const StyleOptions& options = Application::instance().styleOptions();

    _styler.setRenderer(renderer);
    _styler.bind(style, options);

    _boxRect = Gfx::RectF();
    _textRect = Gfx::RectF();
    _textPos = Gfx::PointF();
    _mnemonicRect = Gfx::RectF();

    invalidate();
}


CheckBoxState CheckBox::checkBoxState() const
{
    CheckBoxState s;
    s.setEnabled( isEnabled() );
    s.setHovered( isHovered() );
    s.setFocused( hasFocus() );
    s.setChecked( _state == Checked );
    return s;
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
    Base::onInvalidate();

    const Style& style = Application::instance().style();
    const StyleOptions& options = Application::instance().styleOptions();

    if( _styler.bind(style, options) )
    {
        _boxRect = Gfx::RectF();
        _textRect = Gfx::RectF();
        _textPos = Gfx::PointF();
        _mnemonicRect = Gfx::RectF();
    }

    relayout();
}


Gfx::SizeF CheckBox::onMeasure(const SizePolicy& /*policy*/)
{
    const Painter* painter = _styler.textPainter(surface());
    Gfx::SizeF textSize(0, 0);

    if( painter )
    {
        Gfx::TextMetrics tm = painter->textMetrics( text() );
        Gfx::FontMetrics fm = painter->fontMetrics();
        textSize = Gfx::SizeF(tm.advance(), fm.lineHeight());
    }

    Gfx::SizeF indicatorSize = _styler.measureIndicator(surface());
    Gfx::SizeF contentSize = _styler.measureContent(surface(), indicatorSize, textSize);
    Gfx::SizeF totalSize = _styler.measureFrame(surface(), contentSize);

    return Gfx::SizeF(totalSize.width() + padding().leftRight(),
                      totalSize.height() + padding().topBottom());
}


void CheckBox::onLayout(const Gfx::RectF& rect)
{
    Base::onLayout(rect);

    Gfx::RectF insetRect( Gfx::PointF(padding().left(), padding().top()),
                          Gfx::SizeF(size().width() - padding().leftRight(),
                                     size().height() - padding().topBottom()) );

    Gfx::RectF contentRect = _styler.layoutFrame(surface(), insetRect);

    _boxRect = Gfx::RectF();
    _textRect = Gfx::RectF();
    _textPos = Gfx::PointF();
    _mnemonicRect = Gfx::RectF();

    const Painter* painter = _styler.textPainter(surface());
    if( ! painter )
    {
        _styler.layoutContent(surface(), contentRect, Gfx::SizeF(0, 0),
                                      Gfx::SizeF(0, 0), _boxRect, _textRect);
        repaint();
        return;
    }

    Gfx::FontMetrics fm = painter->fontMetrics();
    Gfx::TextMetrics tm = painter->textMetrics( text() );

    Gfx::SizeF textSize(tm.advance(), fm.lineHeight());
    Gfx::SizeF indicatorSize = _styler.measureIndicator(surface());

    _styler.layoutContent(surface(), contentRect, indicatorSize, textSize,
                          _boxRect, _textRect);

    const Gfx::Scaling& scaling = surface().scaling();
    double textY = scaling.align(_textRect.y() + _textRect.height() / 2.0) + fm.capHeight() / 2.0;

    _textPos = scaling.align( Gfx::PointF(_textRect.x(), textY) );

    String::size_type mnIdx = String::npos;
    const Char* m = mnemonic();
    if( m )
        mnIdx = text().find(*m);

    _mnemonicRect = _styler.layoutMnemonic(surface(), text(), _textPos,
                                           fm, mnIdx);

    repaint();
}


void CheckBox::onPaint(PaintContext& context, const Gfx::RectF& /*rect*/)
{
    Gfx::RectF widgetRect( size() );
    CheckBoxState state = checkBoxState();

    onPaintChrome(context, widgetRect, _boxRect, state);
    onPaintText(context, _textRect, text(), _textPos, state);
    onPaintMnemonic(context, widgetRect, _mnemonicRect, state);
}


void CheckBox::onPaintChrome(PaintContext& context,
                             const Gfx::RectF& rect,
                             const Gfx::RectF& boxRect,
                             const CheckBoxState& state)
{
    _styler.renderChrome(context, rect, boxRect, state);
}


void CheckBox::onPaintText(PaintContext& context,
                           const Gfx::RectF& textRect,
                           const String& text,
                           const Gfx::PointF& pos,
                           const CheckBoxState& state)
{
    _styler.renderText(context, textRect, text, pos, state);
}


void CheckBox::onPaintMnemonic(PaintContext& context,
                              const Gfx::RectF& rect,
                              const Gfx::RectF& mnemonic,
                              const CheckBoxState& state)
{
    _styler.renderMnemonic(context, rect, mnemonic, state);
}

} // namespace

} // namespace
