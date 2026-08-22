/* Copyright (C) 2016 Laurentiu-Gheorghe Crisan
   Copyright (C) 2016 Marc Boris Duerner

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

#include <Pt/Forms/CheckBoxStyle.h>
#include <Pt/Forms/StyleOptions.h>

namespace Pt {

namespace Forms {

CheckBoxState::CheckBoxState()
: _enabled(false)
, _hovered(false)
, _focused(false)
, _checked(false)
{
}


bool CheckBoxState::isEnabled() const
{
    return _enabled;
}


void CheckBoxState::setEnabled(bool value)
{
    _enabled = value;
}


bool CheckBoxState::isHovered() const
{
    return _hovered;
}


void CheckBoxState::setHovered(bool value)
{
    _hovered = value;
}


bool CheckBoxState::isFocused() const
{
    return _focused;
}


void CheckBoxState::setFocused(bool value)
{
    _focused = value;
}


bool CheckBoxState::isChecked() const
{
    return _checked;
}


void CheckBoxState::setChecked(bool value)
{
    _checked = value;
}


CheckBoxRenderer::CheckBoxRenderer(std::size_t refs)
: Style::Facet( typeid(CheckBoxRenderer), refs )
{
}


CheckBoxRenderer::~CheckBoxRenderer()
{
}


CheckBoxRenderer* CheckBoxRenderer::create() const
{
    return onCreate();
}


void CheckBoxRenderer::prepare(const StyleOptions& options,
                               const StyleOptions& checkBoxOptions)
{
    onPrepare(options, checkBoxOptions);
}


void CheckBoxRenderer::onReset(const StyleOptions& options)
{
    StyleOptions empty;
    onPrepare(options, empty);
}


Gfx::SizeF CheckBoxRenderer::measureIndicator(PaintSurface& surface)
{
    return onMeasureIndicator(surface);
}


Gfx::SizeF CheckBoxRenderer::measureContent(PaintSurface& surface,
                                            const Gfx::SizeF& indicatorSize,
                                            const Gfx::SizeF& textSize)
{
    return onMeasureContent(surface, indicatorSize, textSize);
}


Gfx::SizeF CheckBoxRenderer::measureFrame(PaintSurface& surface,
                                          const Gfx::SizeF& contentSize)
{
    return onMeasureFrame(surface, contentSize);
}


Gfx::RectF CheckBoxRenderer::layoutFrame(PaintSurface& surface,
                                         const Gfx::RectF& frameRect)
{
    return onLayoutFrame(surface, frameRect);
}


void CheckBoxRenderer::layoutContent(PaintSurface& surface,
                                     const Gfx::RectF& contentRect,
                                     const Gfx::SizeF& indicatorSize,
                                     const Gfx::SizeF& textSize,
                                     Gfx::RectF& indicatorRect,
                                     Gfx::RectF& textRect)
{
    onLayoutContent(surface, contentRect, indicatorSize, textSize,
                    indicatorRect, textRect);
}


Gfx::RectF CheckBoxRenderer::layoutMnemonic(PaintSurface& surface,
                                            const String& text,
                                            const Gfx::PointF& textPos,
                                            const Gfx::FontMetrics& fontMetrics,
                                            String::size_type mnemonicIndex)
{
    return onLayoutMnemonic(surface, text, textPos, fontMetrics, mnemonicIndex);
}


const Painter& CheckBoxRenderer::textPainter(PaintSurface& surface)
{
    return onGetTextPainter(surface);
}


void CheckBoxRenderer::renderChrome(PaintContext& context,
                                    const Gfx::RectF& rect,
                                    const Gfx::RectF& boxRect,
                                    const CheckBoxState& state)
{
    onRenderChrome(context, rect, boxRect, state);
}


void CheckBoxRenderer::renderText(PaintContext& context,
                                  const Gfx::RectF& textRect,
                                  const String& text,
                                  const Gfx::PointF& pos,
                                  const CheckBoxState& state)
{
    onRenderText(context, textRect, text, pos, state);
}


void CheckBoxRenderer::renderMnemonic(PaintContext& context,
                                      const Gfx::RectF& rect,
                                      const Gfx::RectF& mnemonic,
                                      const CheckBoxState& state)
{
    onRenderMnemonic(context, rect, mnemonic, state);
}


CheckBoxStyle::CheckBoxStyle()
{
}

} // namespace

} // namespace
