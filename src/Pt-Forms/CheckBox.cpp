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
    const Gfx::Brush* b = _checkBoxOptions.background();
    if( b )
        return *b;

    return Application::instance().styleOptions().textBackground();
}


void CheckBox::setBackground(const Gfx::Brush& b)
{
    _checkBoxOptions.setBackground(b);
    invalidate();
}


const Gfx::Pen& CheckBox::contour() const
{
    const Gfx::Pen* p = _checkBoxOptions.contour();
    if( p )
        return *p;

    return Application::instance().styleOptions().contour();
}


void CheckBox::setContour(const Gfx::Pen& p)
{
    _checkBoxOptions.setContour(p);
    invalidate();
}


const Gfx::Color& CheckBox::textColor() const
{
    const Gfx::Color* c = _checkBoxOptions.textColor();
    if( c )
        return *c;

    return Application::instance().styleOptions().textColor();
}


void CheckBox::setTextColor(const Gfx::Color& color)
{
    _checkBoxOptions.setTextColor(color);
    invalidate();
}


const Gfx::Font& CheckBox::font() const
{
    const Gfx::Font* f = _checkBoxOptions.font();
    if( f )
        return *f;

    return Application::instance().styleOptions().font();
}


void CheckBox::setFont(const Gfx::Font& font)
{
    _checkBoxOptions.setFont(font);
    invalidate();
}


void CheckBox::setFontSize(std::size_t size)
{
    _checkBoxOptions.setFontSize(size);
    invalidate();
}


void CheckBox::setFontWeight(Gfx::Font::Weight weight)
{
    _checkBoxOptions.setFontWeight(weight);
    invalidate();
}


void CheckBox::setFontSlant(Gfx::Font::Slant slant)
{
    _checkBoxOptions.setFontSlant(slant);
    invalidate();
}


void CheckBox::setRenderer(CheckBoxRenderer* renderer)
{
    const Style& style = Application::instance().style();
    const StyleOptions& options = Application::instance().styleOptions();

    if( renderer )
        _checkBoxStyle.bind(*renderer, options, _checkBoxOptions);
    else
        _checkBoxStyle.bind(style, options, _checkBoxOptions);

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

    _checkBoxStyle.rebind(style, options, _checkBoxOptions);

    if( ! _checkBoxStyle.renderer() )
        return;

    relayout();
}


Gfx::SizeF CheckBox::onMeasure(const SizePolicy& /*policy*/)
{
    CheckBoxRenderer* renderer = _checkBoxStyle.renderer();
    if( ! renderer )
        return Gfx::SizeF(0, 0);

    const Painter& painter = renderer->textPainter( surface() );
    Gfx::TextMetrics tm = painter.textMetrics( text() );
    Gfx::FontMetrics fm = painter.fontMetrics();

    Gfx::SizeF textSize(tm.advance(), fm.lineHeight());
    Gfx::SizeF indicatorSize = renderer->measureIndicator(surface());
    Gfx::SizeF contentSize = renderer->measureContent(surface(), indicatorSize, textSize);
    Gfx::SizeF totalSize = renderer->measureFrame(surface(), contentSize);

    return Gfx::SizeF(totalSize.width() + padding().leftRight(),
                      totalSize.height() + padding().topBottom());
}


void CheckBox::onLayout(const Gfx::RectF& rect)
{
    Base::onLayout(rect);

    CheckBoxRenderer* renderer = _checkBoxStyle.renderer();
    if( ! renderer )
        return;

    Gfx::RectF insetRect( Gfx::PointF(padding().left(), padding().top()),
                          Gfx::SizeF(size().width() - padding().leftRight(),
                                     size().height() - padding().topBottom()) );

    Gfx::RectF contentRect = renderer->layoutFrame(surface(), insetRect);

    const Painter& painter = renderer->textPainter(surface());
    Gfx::FontMetrics fm = painter.fontMetrics();
    Gfx::TextMetrics tm = painter.textMetrics( text() );

    Gfx::SizeF textSize(tm.advance(), fm.lineHeight());
    Gfx::SizeF indicatorSize = renderer->measureIndicator(surface());

    renderer->layoutContent(surface(), contentRect, indicatorSize, textSize,
                            _boxRect, _textRect);

    const Gfx::Scaling& scaling = surface().scaling();
    double textY = scaling.align(_textRect.y() + _textRect.height() / 2.0) + fm.capHeight() / 2.0;

    _textPos = scaling.align( Gfx::PointF(_textRect.x(), textY) );

    String::size_type mnIdx = String::npos;
    const Char* m = mnemonic();
    if( m )
        mnIdx = text().find(*m);

    _mnemonicRect = renderer->layoutMnemonic(surface(), text(), _textPos,
                                             fm, mnIdx);

    repaint();
}


void CheckBox::onPaint(PaintContext& context, const Gfx::RectF& /*rect*/)
{
    if( ! _checkBoxStyle.renderer() )
        return;

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
    CheckBoxRenderer* renderer = _checkBoxStyle.renderer();
    if( ! renderer )
        return;

    renderer->renderChrome(context, rect, boxRect, state);
}


void CheckBox::onPaintText(PaintContext& context,
                           const Gfx::RectF& textRect,
                           const String& text,
                           const Gfx::PointF& pos,
                           const CheckBoxState& state)
{
    CheckBoxRenderer* renderer = _checkBoxStyle.renderer();
    if( ! renderer )
        return;

    renderer->renderText(context, textRect, text, pos, state);
}


void CheckBox::onPaintMnemonic(PaintContext& context,
                              const Gfx::RectF& rect,
                              const Gfx::RectF& mnemonic,
                              const CheckBoxState& state)
{
    CheckBoxRenderer* renderer = _checkBoxStyle.renderer();
    if( ! renderer )
        return;

    renderer->renderMnemonic(context, rect, mnemonic, state);
}

} // namespace

} // namespace
