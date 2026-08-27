/* Copyright (C) 2013 Marc Boris Duerner
   Copyright (C) 2013 Laurentiu-Gheorghe Crisan

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
   MA  02110-1301  USA
*/

#include <Pt/Forms/PlatinumStyle.h>
#include <Pt/Forms/StyleOptions.h>
#include <Pt/Forms/Application.h>
#include <Pt/Forms/PixmapSurface.h>
#include <Pt/Forms/Panel.h>
#include <Pt/Forms/Label.h>
#include <Pt/Forms/PushButton.h>
#include <Pt/Forms/MenuBar.h>
#include <Pt/Forms/Menu.h>
#include <Pt/Forms/MenuItem.h>
#include <Pt/Forms/ScrollBar.h>
#include <Pt/Forms/ProgressBar.h>
#include <Pt/Forms/ListBox.h>
#include <Pt/Forms/ComboBox.h>
#include <Pt/Forms/SpinBox.h>
#include <Pt/Forms/TabView.h>
#include <Pt/Forms/PaintContext.h>
#include <Pt/Forms/Painter.h>

#include <Pt/Gfx/Painter.h>
#include <Pt/Gfx/PaintSurface.h>
#include <Pt/Gfx/Image.h>

#include <cmath>

namespace {

const double platinumPanelCornerRadius = 2.0;

Pt::Gfx::Color brighten(const Pt::Gfx::Color& c, float factor)
{
    float r = c.red() * factor;
    float g = c.green() * factor;
    float b = c.blue() * factor;

    Pt::uint8_t r8 = r > 255 ? 255 : static_cast<Pt::uint8_t>(r);
    Pt::uint8_t g8 = g > 255 ? 255 : static_cast<Pt::uint8_t>(g);
    Pt::uint8_t b8 = b > 255 ? 255 : static_cast<Pt::uint8_t>(b);

    return Pt::Gfx::Color(c.alpha(), r8, g8, b8);
}


Pt::Gfx::RectF layoutMnemonicImpl(Pt::Forms::Painter& painter,
                                  Pt::Forms::PaintSurface& surface,
                                  const Pt::String& text,
                                  const Pt::Gfx::PointF& textPos,
                                  const Pt::Gfx::FontMetrics& fontMet,
                                  Pt::String::size_type mnemonicIndex)
{
    if( mnemonicIndex == Pt::String::npos || mnemonicIndex >= text.size() )
        return Pt::Gfx::RectF();

    painter.begin(surface);

    Pt::String leftText(text, 0, mnemonicIndex);
    Pt::Gfx::TextMetrics fmLeft = painter.textMetrics(leftText);

    Pt::String charText(1, text[mnemonicIndex]);
    Pt::Gfx::TextMetrics fmChar = painter.textMetrics(charText);

    return Pt::Gfx::RectF(
        Pt::Gfx::PointF(textPos.x() + fmLeft.advance(), textPos.y()),
        Pt::Gfx::SizeF(fmChar.advance(), fontMet.descent()) );
}


void renderMnemonicImpl(Pt::Forms::Painter& painter,
                        Pt::Forms::PaintContext& context,
                        const Pt::Gfx::RectF& rect,
                        const Pt::Gfx::RectF& mnemonic)
{
    if( mnemonic.isEmpty() )
        return;

    painter.begin(context);
    painter.setClip(rect);

    double mnemonicY = mnemonic.top() + 1;
    painter.drawLine( Pt::Gfx::PointF(mnemonic.left(), mnemonicY),
                      Pt::Gfx::PointF(mnemonic.right(), mnemonicY) );
}

} // namespace

namespace Pt {

namespace Forms {

///////////////////////////////////////////////////////////////////////////////
// PlatinumRendererBase
///////////////////////////////////////////////////////////////////////////////

PlatinumRendererBase::PlatinumRendererBase(std::size_t refs)
: Style::Facet( typeid(PlatinumRendererBase), refs )
{
}


PlatinumRendererBase::~PlatinumRendererBase()
{
}


void PlatinumRendererBase::renderChrome(Painter& painter,
                                        const Gfx::RectF& rect,
                                        const Gfx::Pen& pen,
                                        double corner) const
{
    if( rect.width() <= 0 || rect.height() <= 0 )
        return;

    double inset = painter.scaling().alignContour( pen.size() ) / 2;

    Gfx::Polygon polygon = getPolygon(rect, inset, corner);

    painter.setPen(pen);
    painter.drawPolyline(&polygon[0], polygon.size());
}


void PlatinumRendererBase::renderChrome(Painter& painter,
                                        const Gfx::RectF& rect,
                                        double penSize,
                                        double corner) const
{
    if( rect.width() <= 0 || rect.height() <= 0 )
        return;

    double inset = painter.scaling().alignContour(penSize) / 2;

    Gfx::Polygon polygon = getPolygon(rect, inset, corner);

    painter.drawPolyline(&polygon[0], polygon.size());
}


void PlatinumRendererBase::renderPlane(Painter& painter,
                                       const Gfx::RectF& rect,
                                       const Gfx::Brush& brush,
                                       double corner) const

{
    if( rect.width() <= 0 || rect.height() <= 0 )
        return;

    double inset = painter.scaling().toLogical(0.5);

    Gfx::Polygon polygon = getPolygon(rect, inset, corner);

    painter.setBrush(brush);
    painter.fillPolygon(&polygon[0], polygon.size());
}


void PlatinumRendererBase::renderPlane(Painter& painter,
                                       const Gfx::RectF& rect,
                                       double corner) const
{
    if( rect.width() <= 0 || rect.height() <= 0 )
        return;

    double inset = painter.scaling().toLogical(0.5);

    Gfx::Polygon polygon = getPolygon(rect, inset, corner);

    painter.fillPolygon(&polygon[0], polygon.size());
}


Gfx::Polygon PlatinumRendererBase::getPolygon(const Gfx::RectF& rect,
                                             double inset, double corner)
{
    // Clamp inset so polygon coordinates never invert
    double maxInset = std::min(rect.width(), rect.height()) / 2.0;
    if( inset > maxInset )
        inset = maxInset;

    // Clamp corner against remaining available space
    double maxCorner = std::min(rect.width(), rect.height()) / 2.0 - inset;
    if( maxCorner < 0 )
        maxCorner = 0;
    if( corner > maxCorner )
        corner = maxCorner;

    Gfx::Polygon polygon;
    Gfx::PointF outline[9] = {};

    // top left
    outline[0].setX(inset);
    outline[0].setY(corner + inset);

    outline[1].setX(corner + inset);
    outline[1].setY(inset);

    // top right
    outline[2].setX(rect.width() - corner - inset);
    outline[2].setY(inset);

    outline[3].setX(rect.width() - inset);
    outline[3].setY(corner + inset);

    // bottom right
    outline[4].setX(rect.width() - inset);
    outline[4].setY(rect.height() - corner - inset);

    outline[5].setX(rect.width() - corner - inset);
    outline[5].setY(rect.height() - inset);

    // bottom left
    outline[6].setX(corner + inset);
    outline[6].setY(rect.height() - inset);

    outline[7].setX(inset);
    outline[7].setY(rect.height() - corner - inset);

    outline[8] = outline[0];

    for (size_t i = 0; i < 9; ++i)
        polygon.push_back(outline[i]);

    return polygon;
}

///////////////////////////////////////////////////////////////////////////////
// PlatinumPanelRenderer
///////////////////////////////////////////////////////////////////////////////

PlatinumPanelRenderer::PlatinumPanelRenderer(FacetPtr<PlatinumRendererBase> base,
                                             std::size_t refs)
: PanelRenderer(refs)
, _base(base)
, _cornerRadius(0.0)
{
}


PlatinumPanelRenderer::~PlatinumPanelRenderer()
{
}


PanelRenderer* PlatinumPanelRenderer::onCreate() const
{
    return new PlatinumPanelRenderer(_base);
}


void PlatinumPanelRenderer::onPrepare(const StyleOptions& options)
{
    _cornerRadius = platinumPanelCornerRadius;

    _bgPainter.setBrush( options.get<BackgroundOption>().value() );

    Gfx::Pen framePen( options.get<ContourOption>().value() );
    framePen.setJoinStyle(Gfx::Pen::BevelJoin);
    _framePainter.setPen(framePen);

    _textPainter.setFont( options.get<FontOption>().value() );
    _textPainter.setPen( Gfx::Pen( options.get<TextColorOption>().value() ) );
}


Gfx::SizeF PlatinumPanelRenderer::onMeasureFrame(PaintSurface& /*surface*/,
                                                 const Gfx::SizeF& contentSize)
{
    return contentSize;
}


Gfx::RectF PlatinumPanelRenderer::onLayoutFrame(PaintSurface& /*surface*/,
                                                const Gfx::RectF& frameRect)
{
    return frameRect;
}


const Painter& PlatinumPanelRenderer::onGetTextPainter(PaintSurface& surface)
{
    _textPainter.begin(surface);
    return _textPainter;
}


void PlatinumPanelRenderer::onRenderBackground(PaintContext& context,
                                               const Gfx::RectF& rect,
                                               const PanelState& /*state*/)
{
    if( rect.width() <= 0 || rect.height() <= 0 )
        return;

    _bgPainter.begin(context);
    _base->renderPlane(_bgPainter, rect, _cornerRadius);
}


void PlatinumPanelRenderer::onRenderFrame(PaintContext& context,
                                          const Gfx::RectF& rect,
                                          const PanelState& /*state*/)
{
    if( rect.width() <= 0 || rect.height() <= 0 )
        return;

    _framePainter.begin(context);
    _base->renderChrome(_framePainter, rect,
                       _framePainter.pen().size(),
                       _cornerRadius);
}


void PlatinumPanelRenderer::onRenderText(PaintContext& context,
                                         const Gfx::RectF& rect,
                                         const String& text,
                                         const Gfx::PointF& pos,
                                         const PanelState& /*state*/)
{
    if( rect.width() <= 0 || rect.height() <= 0 )
        return;

    _textPainter.begin(context);
    _textPainter.setClip(rect);
    _textPainter.drawText(pos, text);
}


void PlatinumPanelRenderer::onRenderIcon(PaintContext& context,
                                         const Gfx::RectF& rect,
                                         const Pixmap& picture,
                                         const Gfx::PointF& pos,
                                         const PanelState& /*state*/)
{
    if( rect.width() <= 0 || rect.height() <= 0 )
        return;

    _iconPainter.begin(context);
    _iconPainter.setClip(rect);
    _iconPainter.setCompositionMode(Gfx::CompositionMode::SourceOver);
    _iconPainter.drawPixmap(pos, picture);
}

///////////////////////////////////////////////////////////////////////////////
// PlatinumButtonRenderer
///////////////////////////////////////////////////////////////////////////////

PlatinumButtonRenderer::PlatinumButtonRenderer(FacetPtr<PlatinumRendererBase> base,
                                               std::size_t refs)
: ButtonRenderer(refs)
, _base(base)
{
}


PlatinumButtonRenderer::~PlatinumButtonRenderer()
{
}


ButtonRenderer* PlatinumButtonRenderer::onCreate() const
{
    return new PlatinumButtonRenderer(_base);
}


void PlatinumButtonRenderer::onPrepare(const StyleOptions& options)
{
    const ContourOption& contour = options.get<ContourOption>();
    Gfx::Pen cPen = contour.value();
    cPen.setJoinStyle(Gfx::Pen::BevelJoin);

    const AccentColorOption& accentColor = options.get<AccentColorOption>();
    _accentColor = accentColor.value();

    const TextColorOption& textColor = options.get<TextColorOption>();
    _textColor = textColor.value();

    const ForegroundOption& foreground = options.get<ForegroundOption>();
    _normalPainter.setBrush( foreground.value() );
    _normalPainter.setPen( cPen );

    Gfx::Brush pressedBrush(_accentColor);
    _pressedPainter.setBrush( pressedBrush );
    _pressedPainter.setPen( cPen );

    const HighlightColorOption& highlightColor = options.get<HighlightColorOption>();
    Gfx::Brush highlightBrush( highlightColor.value() );
    _highlightPainter.setBrush( highlightBrush );
    _highlightPainter.setPen( cPen );

    _textPainter.setFont( options.get<FontOption>().value() );
    _textPainter.setPen( Gfx::Pen(_textColor) );
}


Gfx::SizeF PlatinumButtonRenderer::onMeasureFrame(PaintSurface& /*surface*/,
                                                   const Gfx::SizeF& contentSize)
{
    Spacing ins(3);
    return Gfx::SizeF(contentSize.width() + ins.leftRight(),
                      contentSize.height() + ins.topBottom());
}


Gfx::SizeF PlatinumButtonRenderer::onMeasureContent(PaintSurface& /*surface*/,
                                                     Direction direction,
                                                     const Gfx::SizeF& iconSize,
                                                     const Gfx::SizeF& textSize)
{
    bool hasIcon = iconSize.width() > 0 && iconSize.height() > 0;
    bool hasText = textSize.width() > 0;

    if( hasIcon && hasText )
    {
        Gfx::FontMetrics fm = _textPainter.fontMetrics();
        double spacing = std::ceil(fm.height() / 2.0);

        bool horizontal = (direction == Direction::Left || direction == Direction::Right);

        if( horizontal )
        {
            double w = iconSize.width() + spacing + textSize.width();
            double h = std::max(iconSize.height(), textSize.height());
            return Gfx::SizeF(w, h);
        }
        else
        {
            double w = std::max(iconSize.width(), textSize.width());
            double h = iconSize.height() + spacing + textSize.height();
            return Gfx::SizeF(w, h);
        }
    }
    else if( hasIcon )
    {
        return iconSize;
    }
    else
    {
        return textSize;
    }
}


Gfx::RectF PlatinumButtonRenderer::onLayoutFrame(PaintSurface& /*surface*/,
                                                   const Gfx::RectF& frameRect)
{
    Spacing ins(3);
    double w = frameRect.width() - ins.leftRight();
    double h = frameRect.height() - ins.topBottom();
    if( w < 0 ) w = 0;
    if( h < 0 ) h = 0;
    return Gfx::RectF(Gfx::PointF(ins.left(), ins.top()),
                       Gfx::SizeF(w, h));
}


Gfx::RectF PlatinumButtonRenderer::onLayoutMnemonic(PaintSurface& surface,
                                                    const String& text,
                                                    const Gfx::PointF& textPos,
                                                    const Gfx::FontMetrics& fontMet,
                                                    String::size_type mnemonicIndex)
{
    return layoutMnemonicImpl(_textPainter, surface, text, textPos, fontMet, mnemonicIndex);
}


const Painter& PlatinumButtonRenderer::onGetTextPainter(PaintSurface& surface)
{
    _textPainter.begin(surface);
    return _textPainter;
}


void PlatinumButtonRenderer::onLayoutContent(PaintSurface& /*surface*/,
                                             const Gfx::RectF& contentRect,
                                             Direction direction,
                                             const Gfx::SizeF& iconSize,
                                             const Gfx::SizeF& textSize,
                                             Gfx::RectF& iconRect,
                                             Gfx::RectF& textRect)
{
    bool hasIcon = iconSize.width() > 0 && iconSize.height() > 0;
    bool hasText = textSize.width() > 0;

    if( hasIcon && hasText )
    {
        Gfx::FontMetrics fm = _textPainter.fontMetrics();
        double spacing = std::ceil(fm.height() / 2.0);

        bool horizontal = (direction == Direction::Left || direction == Direction::Right);
        bool iconFirst  = (direction == Direction::Left || direction == Direction::Top);

        if( horizontal )
        {
            double groupW = iconSize.width() + spacing + textSize.width();
            double gx = contentRect.x() + (contentRect.width() - groupW) / 2;

            double iconX = iconFirst ? gx : gx + textSize.width() + spacing;
            double textX = iconFirst ? gx + iconSize.width() + spacing : gx;

            iconRect.set( Gfx::PointF(iconX, contentRect.y()),
                          Gfx::SizeF(iconSize.width(), contentRect.height()) );
            textRect.set( Gfx::PointF(textX, contentRect.y()),
                          Gfx::SizeF(textSize.width(), contentRect.height()) );
        }
        else
        {
            double groupH = iconSize.height() + spacing + textSize.height();
            double gy = contentRect.y() + (contentRect.height() - groupH) / 2;

            double iconY = iconFirst ? gy : gy + textSize.height() + spacing;
            double textY = iconFirst ? gy + iconSize.height() + spacing : gy;

            iconRect.set( Gfx::PointF(contentRect.x(), iconY),
                          Gfx::SizeF(contentRect.width(), iconSize.height()) );
            textRect.set( Gfx::PointF(contentRect.x(), textY),
                          Gfx::SizeF(contentRect.width(), textSize.height()) );
        }
    }
    else if( hasIcon )
    {
        iconRect = contentRect;
        textRect = Gfx::RectF();
    }
    else
    {
        iconRect = Gfx::RectF();
        textRect = contentRect;
    }
}


void PlatinumButtonRenderer::onRenderBackground(PaintContext& /*context*/,
                                                const Gfx::RectF& /*rect*/,
                                                const ButtonState& /*state*/)
{
}


void PlatinumButtonRenderer::onPrepareIcon(const Gfx::Image& icon,
                                           Pixmap& picture,
                                           const ButtonState& state) const
{
    if( state.isPressed() && state.isFlat() )
    {
        Gfx::Image tintedIcon = icon;
        Gfx::PixelView pixelView(tintedIcon);

        for(Gfx::PixelView::Iterator it = pixelView.begin(); it != pixelView.end(); ++it)
        {
            Gfx::Color color = it->getColor();

            color.setRed( _accentColor.red() );
            color.setGreen( _accentColor.green() );
            color.setBlue( _accentColor.blue() );

            (*it) = color;
        }

        picture.reset(tintedIcon);
    }
    else
    {
        picture.reset(icon);
    }
}


void PlatinumButtonRenderer::onRenderChrome(PaintContext& context,
                                           const Gfx::RectF& rect,
                                           const ButtonState& state)
{
    if( rect.width() <= 0 || rect.height() <= 0 )
        return;

    Painter* painter = 0;

    if( state.isPressed() )
        painter = &_pressedPainter;
    else if( state.isHovered() )
        painter = &_highlightPainter;
    else
        painter = &_normalPainter;

    painter->resetClip();
    painter->begin(context);

    const Gfx::Scaling& scaling = painter->scaling();
    double corner = scaling.align(platinumPanelCornerRadius);

    _base->renderPlane(*painter, rect, painter->brush(), corner);

    _base->renderChrome(*painter, rect, painter->pen(), corner);

    if( state.isFocused() )
    {
        double inset = scaling.toLogical(0.5);
        double focusOffset = scaling.align(2.0) + inset;

        double fw = rect.width() - 2 * focusOffset;
        double fh = rect.height() - 2 * focusOffset;

        if( fw > 0 && fh > 0 )
        {
            Gfx::RectF focusRect(
                Gfx::PointF(rect.x() + focusOffset, rect.y() + focusOffset),
                Gfx::SizeF(fw, fh));

            Gfx::Pen savedPen = painter->pen();
            Gfx::Pen focusPen( savedPen.color(), 1, Gfx::Pen::Dash );
            painter->setPen(focusPen);
            painter->drawRect(focusRect);
            painter->setPen(savedPen);
        }
    }
}


void PlatinumButtonRenderer::onRenderText(PaintContext& context,
                                          const Gfx::RectF& rect,
                                          const String& text,
                                          const Gfx::PointF& pos,
                                          const ButtonState& state)
{
    if( rect.width() <= 0 || rect.height() <= 0 )
        return;

    if( state.isPressed() && state.isFlat() )
        _textPainter.setPen( Gfx::Pen(_accentColor) );
    else
        _textPainter.setPen( Gfx::Pen(_textColor) );

    _textPainter.begin(context);
    _textPainter.setClip(rect);
    _textPainter.drawText(pos, text);
}


void PlatinumButtonRenderer::onRenderMnemonic(PaintContext& context,
                                              const Gfx::RectF& rect,
                                              const Gfx::RectF& mnemonic,
                                              const ButtonState& /*state*/)
{
    renderMnemonicImpl(_textPainter, context, rect, mnemonic);
}


void PlatinumButtonRenderer::onRenderIcon(PaintContext& context,
                                          const Gfx::RectF& rect,
                                          const Pixmap& picture,
                                          const Gfx::PointF& pos,
                                          const ButtonState& /*state*/)
{
    if( rect.width() <= 0 || rect.height() <= 0 )
        return;

    _iconPainter.begin(context);
    _iconPainter.setClip(rect);
    _iconPainter.setCompositionMode(Gfx::CompositionMode::SourceOver);
    _iconPainter.drawPixmap(pos, picture);
}

///////////////////////////////////////////////////////////////////////////////
// PlatinumCheckBoxRenderer
///////////////////////////////////////////////////////////////////////////////

PlatinumCheckBoxRenderer::PlatinumCheckBoxRenderer(std::size_t refs)
: CheckBoxRenderer(refs)
{
}


PlatinumCheckBoxRenderer::~PlatinumCheckBoxRenderer()
{
}


CheckBoxRenderer* PlatinumCheckBoxRenderer::onCreate() const
{
    return new PlatinumCheckBoxRenderer();
}


void PlatinumCheckBoxRenderer::onPrepare(const StyleOptions& options)
{
    Gfx::Brush bg = options.get<TextBackgroundOption>().value();

    Gfx::Pen cPen = options.get<ContourOption>().value();
    cPen.setJoinStyle(Gfx::Pen::BevelJoin);

    _boxPainter.setBrush( bg );
    _boxPainter.setPen( cPen );

    Gfx::Font f = options.get<FontOption>().value();
    Gfx::Color tc = options.get<TextColorOption>().value();

    _textPainter.setFont( f );
    _textPainter.setPen( tc );

    _checkColor = tc;
}


Gfx::SizeF PlatinumCheckBoxRenderer::onMeasureIndicator(PaintSurface& /*surface*/)
{
    double sz = _textPainter.font().size() * 1.2;
    return Gfx::SizeF(sz, sz);
}


Gfx::SizeF PlatinumCheckBoxRenderer::onMeasureContent(PaintSurface& /*surface*/,
                                                      const Gfx::SizeF& indicatorSize,
                                                      const Gfx::SizeF& textSize)
{
    double space = std::min<double>(indicatorSize.width() / 2, textSize.height() / 2);
    double focusPad = 2.0;
    double strokeInset = 1.0;

    double extraWidth = 2 * (focusPad + strokeInset);
    double w = indicatorSize.width() + space + textSize.width() + extraWidth;

    double h = std::max<double>(indicatorSize.height(), textSize.height());
    h += 2 * strokeInset;

    return Gfx::SizeF(w, h);
}


Gfx::SizeF PlatinumCheckBoxRenderer::onMeasureFrame(PaintSurface& /*surface*/,
                                                     const Gfx::SizeF& contentSize)
{
    return contentSize;
}


Gfx::RectF PlatinumCheckBoxRenderer::onLayoutFrame(PaintSurface& /*surface*/,
                                                    const Gfx::RectF& frameRect)
{
    return frameRect;
}


void PlatinumCheckBoxRenderer::onLayoutContent(PaintSurface& /*surface*/,
                                               const Gfx::RectF& contentRect,
                                               const Gfx::SizeF& indicatorSize,
                                               const Gfx::SizeF& textSize,
                                               Gfx::RectF& indicatorRect,
                                               Gfx::RectF& textRect)
{
    double indW = indicatorSize.width();
    if( indW > contentRect.width() )
        indW = contentRect.width();

    double indH = indicatorSize.height();
    if( indH > contentRect.height() )
        indH = contentRect.height();

    double space = std::min<double>(indW / 2, textSize.height() / 2);

    double boxX = contentRect.x();
    double boxY = contentRect.y() + (contentRect.height() - indH) / 2.0;
    indicatorRect.set( Gfx::PointF(boxX, boxY), Gfx::SizeF(indW, indH) );

    double textX = contentRect.x() + indW + space;
    double textW = contentRect.width() - indW - space;
    if( textW < 0 )
        textW = 0;

    textRect.set( Gfx::PointF(textX, contentRect.y()),
                  Gfx::SizeF(textW, contentRect.height()) );
}


Gfx::RectF PlatinumCheckBoxRenderer::onLayoutMnemonic(PaintSurface& surface,
                                                      const String& text,
                                                      const Gfx::PointF& textPos,
                                                      const Gfx::FontMetrics& fontMetrics,
                                                      String::size_type mnemonicIndex)
{
    return layoutMnemonicImpl(_textPainter, surface, text, textPos, fontMetrics, mnemonicIndex);
}


const Painter& PlatinumCheckBoxRenderer::onGetTextPainter(PaintSurface& surface)
{
    _textPainter.begin(surface);
    return _textPainter;
}


void PlatinumCheckBoxRenderer::onRenderChrome(PaintContext& context,
                                                 const Gfx::RectF& rect,
                                                 const Gfx::RectF& box,
                                                 const CheckBoxState& state)
{
    if( rect.width() <= 0 || rect.height() <= 0 )
        return;

    if( box.width() <= 0 || box.height() <= 0 )
        return;

    _boxPainter.begin(context);
    _boxPainter.setClip(rect);

    const Gfx::Scaling& scaling = _boxPainter.scaling();

    Gfx::RectF boxRect = scaling.align(box);
    double inset = scaling.toLogical(0.5);
    double checkOffset = scaling.alignContour(platinumPanelCornerRadius);

    double cw = boxRect.width() - 2 * checkOffset;
    double ch = boxRect.height() - 2 * checkOffset;
    double bw = boxRect.width() - 2 * inset;
    double bh = boxRect.height() - 2 * inset;

    _boxPainter.fillRect(boxRect);

    if( bw > 0 && bh > 0 )
    {
        Gfx::RectF borderRect(Gfx::PointF(boxRect.x() + inset, boxRect.y() + inset),
                              Gfx::SizeF(bw, bh));
        _boxPainter.drawRect(borderRect);
    }

    if( state.isChecked() && cw > 0 && ch > 0 )
    {
        Gfx::RectF checkRect(Gfx::PointF(boxRect.x() + checkOffset, boxRect.y() + checkOffset),
                             Gfx::SizeF(cw, ch));

        Gfx::Brush savedBrush = _boxPainter.brush();
        _boxPainter.setBrush( _checkColor );
        _boxPainter.fillRect(checkRect);
        _boxPainter.setBrush( savedBrush );
    }
}


void PlatinumCheckBoxRenderer::onRenderText(PaintContext& context,
                                            const Gfx::RectF& textRect,
                                            const String& text,
                                            const Gfx::PointF& pos,
                                            const CheckBoxState& state)
{
    if( textRect.width() <= 0 || textRect.height() <= 0 )
        return;

    double focusPad = 2.0;
    double strokeInset = 1.0;

    _textPainter.begin(context);
    _textPainter.setClip(textRect);

    Gfx::PointF renderPos(pos.x() + focusPad + strokeInset, pos.y());
    _textPainter.drawText(renderPos, text);

    if( state.isFocused() )
    {
        Gfx::FontMetrics fm = _textPainter.fontMetrics();
        Gfx::TextMetrics tm = _textPainter.textMetrics(text);

        Gfx::RectF focusRect( Gfx::PointF(renderPos.x() - focusPad, renderPos.y() - fm.ascent()),
                              Gfx::SizeF(tm.advance() + 2 * focusPad, fm.height()) );

        Gfx::Pen savedPen = _textPainter.pen();
        Gfx::Pen focusPen(savedPen.color(), 1, Gfx::Pen::Dash);
        _textPainter.setPen(focusPen);
        _textPainter.drawRect(focusRect);
        _textPainter.setPen(savedPen);
    }
}


void PlatinumCheckBoxRenderer::onRenderMnemonic(PaintContext& context,
                                                const Gfx::RectF& rect,
                                                const Gfx::RectF& mnemonic,
                                                const CheckBoxState& /*state*/)
{
    renderMnemonicImpl(_textPainter, context, rect, mnemonic);
}



///////////////////////////////////////////////////////////////////////////////
// PlatinumSpinBoxRenderer
///////////////////////////////////////////////////////////////////////////////

PlatinumSpinBoxRenderer::PlatinumSpinBoxRenderer(std::size_t refs)
: SpinBoxRenderer(refs)
, _inset(5)
{
}


PlatinumSpinBoxRenderer::~PlatinumSpinBoxRenderer()
{
}


SpinBoxRenderer* PlatinumSpinBoxRenderer::onCreate() const
{
    return new PlatinumSpinBoxRenderer();
}


void PlatinumSpinBoxRenderer::onPrepare(const StyleOptions& options,
                                        const StyleOptions& spinBoxOptions)
{
    const TextBackgroundOption* localBg = spinBoxOptions.find<TextBackgroundOption>();
    _background = localBg ? localBg->value()
                          : options.get<TextBackgroundOption>().value();

    const ContourOption* localContour = spinBoxOptions.find<ContourOption>();
    _contour = localContour ? localContour->value()
                            : options.get<ContourOption>().value();
    _contour.setJoinStyle(Gfx::Pen::BevelJoin);

    const ForegroundOption* localFg = spinBoxOptions.find<ForegroundOption>();
    _foreground = localFg ? localFg->value()
                          : options.get<ForegroundOption>().value();

    const Gfx::Font& baseFont = options.get<FontOption>().value();
    const FontOption* localFont = spinBoxOptions.find<FontOption>();
    _font = localFont ? localFont->getFont(baseFont) : baseFont;

    const TextColorOption* localText = spinBoxOptions.find<TextColorOption>();
    _textColor = localText ? localText->value()
                           : options.get<TextColorOption>().value();

    _accentColor = options.get<AccentColorOption>().value();

    _textPainter.setFont(_font);
    _textPainter.setPen(_textColor);
}


Gfx::SizeF PlatinumSpinBoxRenderer::onMeasureFrame(PaintSurface& surface,
                                                   const Gfx::SizeF& contentSize)
{
    _textPainter.begin(surface);
    _textPainter.setFont(_font);

    Gfx::FontMetrics fm = _textPainter.fontMetrics();
    double textHeight = fm.ascent() + fm.descent();
    double entryHeight = textHeight + 2 * _inset;
    double buttonHeight = entryHeight;

    double entryWidth = contentSize.width() + 2 * _inset;

    double height = entryHeight;

    if(buttonHeight > height)
        height = buttonHeight;

    double width = entryWidth + 2 * height;

    return Gfx::SizeF(width, height);
}


Gfx::SizeF PlatinumSpinBoxRenderer::onMeasureEntry(PaintSurface& surface,
                                                    const Gfx::SizeF& contentSize)
{
    _textPainter.begin(surface);
    _textPainter.setFont(_font);

    Gfx::FontMetrics fm = _textPainter.fontMetrics();
    double textHeight = fm.ascent() + fm.descent();
    double entryHeight = textHeight + 2 * _inset;
    double entryWidth = contentSize.width() + 2 * _inset;

    return Gfx::SizeF(entryWidth, entryHeight);
}


Gfx::SizeF PlatinumSpinBoxRenderer::onMeasureIndicator(PaintSurface& surface)
{
    _textPainter.begin(surface);
    _textPainter.setFont(_font);

    Gfx::FontMetrics fm = _textPainter.fontMetrics();
    double textHeight = fm.ascent() + fm.descent();
    double h = textHeight + 2 * _inset;
    return Gfx::SizeF(h, h);
}


void PlatinumSpinBoxRenderer::onLayoutChrome(PaintSurface& /*surface*/,
                                               const Gfx::RectF& rect,
                                               Gfx::RectF& entryRect,
                                               Gfx::RectF& upButtonRect,
                                               Gfx::RectF& downButtonRect,
                                               Gfx::RectF& textRect)
{
    double buttonWidth = rect.height();
    if( 2 * buttonWidth > rect.width() )
        buttonWidth = rect.width() / 2.0;

    double entryW = rect.width() - 2 * buttonWidth;
    if( entryW < 0 )
        entryW = 0;

    entryRect.setOrigin( Gfx::PointF(rect.x() + buttonWidth, rect.y()) );
    entryRect.setSize( Gfx::SizeF(entryW, rect.height()) );

    downButtonRect.setOrigin( Gfx::PointF(rect.x(), rect.y()) );
    downButtonRect.setSize( Gfx::SizeF(buttonWidth, rect.height()) );

    upButtonRect.setOrigin( Gfx::PointF(entryRect.x() + entryW, rect.y()) );
    upButtonRect.setSize( Gfx::SizeF(buttonWidth, rect.height()) );

    double textW = entryW - 2 * _inset;
    if( textW < 0 )
        textW = 0;

    textRect.setOrigin( Gfx::PointF(entryRect.x() + _inset, entryRect.y()) );
    textRect.setSize( Gfx::SizeF(textW, entryRect.height()) );
}


Gfx::RectF PlatinumSpinBoxRenderer::onLayoutEntry(PaintSurface& /*surface*/,
                                                   const Gfx::RectF& entryRect)
{
    double textW = entryRect.width() - 2 * _inset;
    if( textW < 0 )
        textW = 0;

    Gfx::RectF textRect;
    textRect.setOrigin( Gfx::PointF(entryRect.x() + _inset, entryRect.y()) );
    textRect.setSize( Gfx::SizeF(textW, entryRect.height()) );
    return textRect;
}


const Painter& PlatinumSpinBoxRenderer::onGetTextPainter(PaintSurface& surface)
{
    _textPainter.begin(surface);
    _textPainter.setFont(_font);
    return _textPainter;
}


void PlatinumSpinBoxRenderer::onRenderChrome(PaintContext& context,
                                              const Gfx::RectF& /*rect*/,
                                              const Gfx::RectF& entryRect,
                                              const Gfx::RectF& upButtonRect,
                                              const Gfx::RectF& downButtonRect,
                                              const SpinBoxState& state)
{
    if( entryRect.width() <= 0 || entryRect.height() <= 0 )
        return;

    _bgPainter.begin(context);
    _bgPainter.setBrush(_background);
    _bgPainter.fillRect(entryRect);

    Gfx::Pen cPen = _contour;

    if( state.isEnabled() )
    {
        if( state.isHovered() || state.isFocused() )
        {
            cPen = Gfx::Pen( _accentColor,
                             cPen.size(), cPen.style(),
                             cPen.capStyle(), cPen.joinStyle() );
        }
    }

    _bgPainter.setPen(cPen);

    const double inset = _bgPainter.scaling().alignContour( cPen.size() ) / 2;

    double bw = entryRect.width() - 2 * inset;
    double bh = entryRect.height() - 2 * inset;
    if( bw > 0 && bh > 0 )
    {
        Gfx::RectF borderRect( Gfx::PointF(entryRect.x() + inset, entryRect.y() + inset),
                               Gfx::SizeF(bw, bh) );
        _bgPainter.drawRect(borderRect);
    }

    // Render indicators
    _buttonPainter.begin(context);

    Gfx::Pen indicatorPen = cPen;
    _buttonPainter.setPen(indicatorPen);
    _buttonPainter.setBrush( indicatorPen.color() );

    if( downButtonRect.width() > 0 && downButtonRect.height() > 0 )
        renderIndicator(_buttonPainter, downButtonRect, false, state.isDownHovered());

    if( upButtonRect.width() > 0 && upButtonRect.height() > 0 )
        renderIndicator(_buttonPainter, upButtonRect, true, state.isUpHovered());
}


void PlatinumSpinBoxRenderer::onRenderEntry(PaintContext& /*context*/,
                                            const Gfx::RectF& /*entryRect*/,
                                            const SpinBoxState& /*state*/)
{
}


void PlatinumSpinBoxRenderer::onRenderUpButton(PaintContext& /*context*/,
                                               const Gfx::RectF& /*buttonRect*/,
                                               const SpinBoxState& /*state*/)
{
}


void PlatinumSpinBoxRenderer::onRenderDownButton(PaintContext& /*context*/,
                                                 const Gfx::RectF& /*buttonRect*/,
                                                 const SpinBoxState& /*state*/)
{
}


void PlatinumSpinBoxRenderer::renderIndicator(Painter& painter,
                                              const Gfx::RectF& rect,
                                              bool up,
                                              bool hovered)
{
    painter.setClip(rect);

    Gfx::Pen cPen = _contour;

    if(hovered)
    {
        cPen = Gfx::Pen( _accentColor,
                         cPen.size(), cPen.style(),
                         cPen.capStyle(), cPen.joinStyle() );
    }

    painter.setPen(cPen);
    painter.setBrush( cPen.color() );

    const Gfx::Scaling& scaling = painter.scaling();

    double buttonWidth = rect.width();
    double buttonHeight = rect.height();
    double pixelWidth = scaling.toLogical(1.0);

    double triangleWidth = buttonWidth / 3;
    triangleWidth = scaling.align(triangleWidth);

    if( triangleWidth < pixelWidth * 2 )
        return;

    // even number of pixels
    int widthPixels = Pt::lround(triangleWidth / pixelWidth);
    if(widthPixels % 2 != 0)
      triangleWidth += pixelWidth;

    double triangleHeight = triangleWidth / 2.0;
    triangleHeight = scaling.align(triangleHeight);

    double x = (buttonWidth - triangleWidth) / 2;
    x = scaling.align(x);

    double y = (buttonHeight - triangleHeight) / 2;
    y = scaling.align(y);

    x += rect.x();
    y += rect.y();

    Gfx::PointF triangle[4];

    if( ! up )
    {
        triangle[0] = Gfx::PointF(x, y);
        triangle[1] = Gfx::PointF(x + triangleWidth, y);
        triangle[2] = Gfx::PointF(x + triangleHeight, y + triangleHeight);
    }
    else
    {
        triangle[0] = Gfx::PointF(x + triangleHeight, y);
        triangle[1] = Gfx::PointF(x + triangleWidth, y + triangleHeight);
        triangle[2] = Gfx::PointF(x, y + triangleHeight);
    }

    triangle[3] = triangle[0];

    painter.fillPolygon(triangle, 3);
    painter.drawPolyline(triangle, 4);
}


void PlatinumSpinBoxRenderer::onRenderText(PaintContext& context,
                                           const Gfx::RectF& textRect,
                                           const String& text,
                                           const Gfx::PointF& textPos,
                                           const Gfx::RectF& cursor,
                                           const SpinBoxState& state)
{
    if( textRect.width() <= 0 || textRect.height() <= 0 )
        return;

    _textPainter.begin(context);
    _textPainter.setClip(textRect);

    _textPainter.drawText(textPos, text);

    if( state.isEditable() && state.isFocused() )
    {
        Gfx::PointF top = cursor.topLeft();
        Gfx::PointF bottom = cursor.bottomLeft();
        _textPainter.drawLine(top, bottom);
    }
}


///////////////////////////////////////////////////////////////////////////////
// PlatinumLineEditRenderer
///////////////////////////////////////////////////////////////////////////////

PlatinumLineEditRenderer::PlatinumLineEditRenderer(std::size_t refs)
: LineEditRenderer(refs)
, _inset(5)
{
}


PlatinumLineEditRenderer::~PlatinumLineEditRenderer()
{
}


LineEditRenderer* PlatinumLineEditRenderer::onCreate() const
{
    return new PlatinumLineEditRenderer();
}


void PlatinumLineEditRenderer::onPrepare(const StyleOptions& options,
                                         const StyleOptions& lineEditOptions)
{
    const TextBackgroundOption* localBg = lineEditOptions.find<TextBackgroundOption>();
    _background = localBg ? localBg->value()
                          : options.get<TextBackgroundOption>().value();

    const ContourOption* localContour = lineEditOptions.find<ContourOption>();
    _contour = localContour ? localContour->value()
                            : options.get<ContourOption>().value();
    _contour.setJoinStyle(Gfx::Pen::BevelJoin);

    const Gfx::Font& baseFont = options.get<FontOption>().value();
    const FontOption* localFont = lineEditOptions.find<FontOption>();
    _font = localFont ? localFont->getFont(baseFont) : baseFont;

    const TextColorOption* localText = lineEditOptions.find<TextColorOption>();
    _textColor = localText ? localText->value()
                           : options.get<TextColorOption>().value();

    _accentColor = options.get<AccentColorOption>().value();
    _selectionBackground = options.get<AccentColorOption>().value();
    _selectionTextColor = options.get<TextBackgroundOption>().value().color();

    _textPainter.setFont( _font );
    _textPainter.setPen( Gfx::Pen(_textColor) );
}


Gfx::SizeF PlatinumLineEditRenderer::onMeasureFrame(PaintSurface& surface,
                                                 const Gfx::SizeF& contentSize)
{
    _textPainter.begin(surface);
    _textPainter.setFont( _font );

    Gfx::FontMetrics fm = _textPainter.fontMetrics();
    double textHeight = fm.ascent() + fm.descent();
    double entryHeight = textHeight + 2 * _inset;
    double entryWidth = contentSize.width() + 2 * _inset;
    return Gfx::SizeF(entryWidth, entryHeight);
}


Gfx::RectF PlatinumLineEditRenderer::onLayoutFrame(PaintSurface& /*surface*/,
                                                const Gfx::RectF& rect)
{
    double w = rect.width() - 2 * _inset;
    double h = rect.height() - 2 * _inset;
    if( w < 0 ) w = 0;
    if( h < 0 ) h = 0;

    Gfx::RectF textRect(rect);
    textRect.setOrigin( Gfx::PointF(rect.x() + _inset, rect.y() + _inset) );
    textRect.setSize( Gfx::SizeF(w, h) );
    return textRect;
}


const Painter& PlatinumLineEditRenderer::onGetTextPainter(PaintSurface& surface)
{
    _textPainter.begin(surface);
    _textPainter.setFont( _font );
    return _textPainter;
}


void PlatinumLineEditRenderer::onRenderEntry(PaintContext& context,
                                             const Gfx::RectF& rect,
                                             const LineEditState& state)
{
    if( rect.width() <= 0 || rect.height() <= 0 )
        return;

    _bgPainter.begin(context);

    Gfx::Brush bg = _background;

    Gfx::Pen cPen = _contour;

    if( state.isEnabled() )
    {
        if( state.isHighlighted() || state.isFocused() )
        {
            cPen = Gfx::Pen( _accentColor,
                             cPen.size(), cPen.style(),
                             cPen.capStyle(), cPen.joinStyle() );
        }
    }

    _bgPainter.setBrush(bg);
    _bgPainter.setPen(cPen);

    const double inset = _bgPainter.scaling().alignContour( cPen.size() ) / 2;

    double bw = rect.width() - 2 * inset;
    double bh = rect.height() - 2 * inset;

    _bgPainter.fillRect(rect);

    if( bw > 0 && bh > 0 )
    {
        Gfx::RectF borderRect( Gfx::PointF(rect.x() + inset, rect.y() + inset),
                               Gfx::SizeF(bw, bh) );
        _bgPainter.drawRect(borderRect);
    }
}


void PlatinumLineEditRenderer::onRenderSelection(PaintContext& context,
                                                  const Gfx::RectF& textRect,
                                                  const Gfx::RectF& selection,
                                                  const LineEditState& /*state*/)
{
    if( selection.width() <= 0 || textRect.width() <= 0 || textRect.height() <= 0 )
        return;

    Gfx::Brush selBg( _selectionBackground );

    _selectionPainter.begin(context);
    _selectionPainter.setClip(textRect);
    _selectionPainter.setBrush(selBg);
    _selectionPainter.fillRect(selection);
}


void PlatinumLineEditRenderer::onRenderText(PaintContext& context,
                                            const Gfx::RectF& textRect,
                                            const String& text,
                                            const Gfx::PointF& textPos,
                                            const LineEditState& state)
{
    if( textRect.width() <= 0 || textRect.height() <= 0 )
        return;

    if( state.isPlaceholder() )
        _textPainter.setPen( _contour );
    else
        _textPainter.setPen( Gfx::Pen(_textColor) );

    _textPainter.begin(context);
    _textPainter.setClip(textRect);
    _textPainter.setFont( _font );
    _textPainter.drawText(textPos, text);
}


void PlatinumLineEditRenderer::onRenderCursor(PaintContext& context,
                                              const Gfx::RectF& textRect,
                                              const Gfx::RectF& cursor,
                                              const LineEditState& state)
{
    if( ! state.isEditable() )
        return;

    if( ! state.isFocused() )
        return;

    if( textRect.width() <= 0 || textRect.height() <= 0 )
        return;

    _textPainter.setPen( Gfx::Pen(_textColor) );
    _textPainter.begin(context);
    _textPainter.setClip(textRect);

    _textPainter.drawLine( cursor.topLeft(), cursor.bottomLeft() );
}

///////////////////////////////////////////////////////////////////////////////
// PlatinumProgressBarRenderer
///////////////////////////////////////////////////////////////////////////////

PlatinumProgressBarRenderer::PlatinumProgressBarRenderer(std::size_t refs)
: ProgressBarRenderer(refs)
{
}


PlatinumProgressBarRenderer::~PlatinumProgressBarRenderer()
{
}


ProgressBarRenderer* PlatinumProgressBarRenderer::onCreate() const
{
    return new PlatinumProgressBarRenderer();
}


void PlatinumProgressBarRenderer::onPrepare(const StyleOptions& options,
                                            const StyleOptions& progressBarOptions)
{
    Gfx::Brush chunkBrush = options.get<AccentColorOption>().value() ;

    const ForegroundOption* localFg = progressBarOptions.find<ForegroundOption>();
    Gfx::Brush foregroundBrush = localFg ? localFg->value()
                                         : Gfx::Brush( options.get<ForegroundOption>().value() );

    _textBackground = options.get<TextBackgroundOption>().value().color();

    _trackPainter.setBrush(foregroundBrush);
    _chunkPainter.setBrush(chunkBrush);

    const Gfx::Font& baseFont = options.get<FontOption>().value();
    const FontOption* localFont = progressBarOptions.find<FontOption>();
    Gfx::Font resolvedFont = localFont ? localFont->getFont(baseFont) : baseFont;

    const TextColorOption* localText = progressBarOptions.find<TextColorOption>();
    Gfx::Color textCol = localText ? localText->value()
                                   : options.get<TextColorOption>().value();

    _textPainter.setFont( resolvedFont );
    _textPainter.setPen( Gfx::Pen(textCol) );

    _invertTextPainter.setFont( resolvedFont );
    _invertTextPainter.setPen( Gfx::Pen(_textBackground) );
}


Gfx::SizeF PlatinumProgressBarRenderer::onMeasureFrame(PaintSurface& surface,
                                                        const Gfx::SizeF& contentSize)
{
    _textPainter.begin(surface);

    Gfx::SizeF barSz = onMeasureBar(surface);
    Gfx::SizeF outerSz = barSz;

    if (contentSize.width() > outerSz.width())
        outerSz.setWidth(contentSize.width());
    if (contentSize.height() > outerSz.height())
        outerSz.setHeight(contentSize.height());

    return outerSz;
}


Gfx::SizeF PlatinumProgressBarRenderer::onMeasureBar(PaintSurface& surface)
{
    _textPainter.begin(surface);
    double barHeight = _textPainter.scaling().align(3.0);
    return Gfx::SizeF(100.0, barHeight); // arbitrary width for measure
}


void PlatinumProgressBarRenderer::onLayoutChrome(PaintSurface& surface,
                                                 const Gfx::RectF& rect,
                                                 const Gfx::SizeF& barSize,
                                                 const Gfx::SizeF& textSize,
                                                 Gfx::RectF& barRect,
                                                 Gfx::RectF& textRect)
{
    _textPainter.begin(surface);
    double barHeight = _textPainter.scaling().align(3.0);
    if( barHeight > rect.height() )
        barHeight = rect.height();

    double boxY = (rect.height() - barHeight) / 2.0;
    if( boxY < 0 )
        boxY = 0;
    boxY = _textPainter.scaling().align(boxY);

    barRect = Gfx::RectF(Gfx::PointF(rect.x(), rect.y() + boxY), Gfx::SizeF(rect.width(), barHeight));
    textRect = rect;
}


void PlatinumProgressBarRenderer::onLayoutBar(PaintSurface& surface,
                                              const Gfx::RectF& barRect,
                                              float progressRatio,
                                              Gfx::RectF& trackRect,
                                              Gfx::RectF& chunkRect)
{
    trackRect = barRect;
    chunkRect = Gfx::RectF(Gfx::PointF(barRect.x(), barRect.y()),
                           Gfx::SizeF(barRect.width() * progressRatio, barRect.height()));
}


const Painter& PlatinumProgressBarRenderer::onGetTextPainter(PaintSurface& surface)
{
    _textPainter.begin(surface);
    return _textPainter;
}


void PlatinumProgressBarRenderer::onRenderTrack(PaintContext& context,
                                                const Gfx::RectF& trackRect,
                                                const ProgressBarState& /*state*/)
{
    if( trackRect.width() <= 0 || trackRect.height() <= 0 )
        return;

    _trackPainter.begin(context);
    _trackPainter.fillRect(trackRect);
}


void PlatinumProgressBarRenderer::onRenderChunk(PaintContext& context,
                                                const Gfx::RectF& chunkRect,
                                                const ProgressBarState& /*state*/)
{
    if( chunkRect.width() <= 0 || chunkRect.height() <= 0 )
        return;

    _chunkPainter.begin(context);
    _chunkPainter.setClip(chunkRect);
    _chunkPainter.fillRect(chunkRect);

    double d = chunkRect.height();
    _chunkPainter.fillEllipse( Gfx::PointF(chunkRect.x() + chunkRect.width() - d / 2,
                                           chunkRect.y()),
                               Gfx::SizeF(d, d) );
}


void PlatinumProgressBarRenderer::onRenderText(PaintContext& context,
                                               const Gfx::RectF& textRect,
                                               const Gfx::RectF& chunkRect,
                                               const String& text,
                                               const Gfx::PointF& textPos,
                                               const ProgressBarState& /*state*/)
{
    if( textRect.width() <= 0 || textRect.height() <= 0 )
        return;

    _textPainter.begin(context);
    _textPainter.setClip(textRect);
    _textPainter.drawText(textPos, text);

    bool intersects = chunkRect.left() < textRect.right() && chunkRect.right() > textRect.left() &&
                      chunkRect.top() < textRect.bottom() && chunkRect.bottom() > textRect.top();

    // Overwrite the text with the inverted color, clipped to intersection of chunk and text area
    if( chunkRect.width() > 0 && chunkRect.height() > 0 && intersects )
    {
        double clipL = std::max(chunkRect.left(), textRect.left());
        double clipT = std::max(chunkRect.top(), textRect.top());
        double clipR = std::min(chunkRect.right(), textRect.right());
        double clipB = std::min(chunkRect.bottom(), textRect.bottom());

        Gfx::RectF clipRect(Gfx::PointF(clipL, clipT),
                            Gfx::SizeF(clipR - clipL, clipB - clipT));

        _invertTextPainter.begin(context);
        _invertTextPainter.setClip(clipRect);
        _invertTextPainter.drawText(textPos, text);
    }
}

///////////////////////////////////////////////////////////////////////////////
// PlatinumSliderRenderer
///////////////////////////////////////////////////////////////////////////////

PlatinumSliderRenderer::PlatinumSliderRenderer(std::size_t refs)
: SliderRenderer(refs)
{
}


PlatinumSliderRenderer::~PlatinumSliderRenderer()
{
}


SliderRenderer* PlatinumSliderRenderer::onCreate() const
{
    return new PlatinumSliderRenderer();
}


void PlatinumSliderRenderer::onPrepare(const StyleOptions& options,
                                       const StyleOptions& sliderOptions)
{
    _hoverBrush = options.get<AccentColorOption>().value();

    const ContourOption* localContour = sliderOptions.find<ContourOption>();
    _contourBrush = Gfx::Brush( localContour ? localContour->value().color()
                                             : options.get<ContourOption>().value().color() );

    const ForegroundOption* localForeground = sliderOptions.find<ForegroundOption>();
    Gfx::Brush foregroundBrush = localForeground ? localForeground->value()
                                                 : options.get<ForegroundOption>().value();

    _trackPainter.setBrush(foregroundBrush);
    _handlePainter.setBrush(_contourBrush);
}


Gfx::SizeF PlatinumSliderRenderer::onMeasureFrame(PaintSurface& /*surface*/,
                                                   const Gfx::SizeF& contentSize)
{
    double handleHeight = 17.0;
    return Gfx::SizeF(contentSize.width(), handleHeight);
}


Gfx::SizeF PlatinumSliderRenderer::onMeasureTrack(PaintSurface& /*surface*/)
{
    double trackHeight = 5.0;
    return Gfx::SizeF(0.0, trackHeight);
}


Gfx::SizeF PlatinumSliderRenderer::onMeasureHandle(PaintSurface& /*surface*/)
{
    return Gfx::SizeF(7.0, 17.0);
}


void PlatinumSliderRenderer::onLayoutChrome(PaintSurface& /*surface*/,
                                           const Gfx::RectF& rect,
                                           const Gfx::SizeF& trackSize,
                                           const Gfx::SizeF& handleSize,
                                           Gfx::RectF& trackRect,
                                           Gfx::RectF& handleRect)
{
    double handleWidth = handleSize.width();
    if( handleWidth > rect.width() )
        handleWidth = rect.width();

    double trackHeight = trackSize.height();
    if( trackHeight > rect.height() )
        trackHeight = rect.height();

    double trackX = rect.x() + handleWidth / 2;
    double trackY = rect.y() + (rect.height() - trackHeight) / 2.0;
    if( trackY < rect.y() )
        trackY = rect.y();

    double trackWidth = rect.width() - handleWidth;
    if( trackWidth < 0.0 )
        trackWidth = 0.0;

    trackRect = Gfx::RectF( Gfx::PointF(trackX, trackY),
                            Gfx::SizeF(trackWidth, trackHeight) );

    double handleH = handleSize.height();
    if( handleH > rect.height() )
        handleH = rect.height();

    handleRect = Gfx::RectF( Gfx::PointF(rect.x(), rect.y()),
                             Gfx::SizeF(handleWidth, handleH) );
}


void PlatinumSliderRenderer::onLayoutHandle(PaintSurface& /*surface*/,
                                            const Gfx::RectF& trackRect,
                                            float fraction,
                                            Gfx::RectF& handleRect)
{
    double handleWidth = handleRect.width();
    double handleHeight = handleRect.height();

    double handleX = trackRect.x() + trackRect.width() * fraction - handleWidth / 2;
    double handleY = trackRect.y() + trackRect.height() / 2 - handleHeight / 2;

    handleRect = Gfx::RectF( Gfx::PointF(handleX, handleY),
                             Gfx::SizeF(handleWidth, handleHeight) );
}


void PlatinumSliderRenderer::onRenderTrack(PaintContext& context,
                                           const Gfx::RectF& trackRect,
                                           const SliderState& /*state*/)
{
    if( trackRect.width() <= 0 || trackRect.height() <= 0 )
        return;

    _trackPainter.begin(context);
    _trackPainter.fillRect(trackRect);
}


void PlatinumSliderRenderer::onRenderHandle(PaintContext& context,
                                            const Gfx::RectF& handleRect,
                                            const SliderState& state)
{
    if( handleRect.width() <= 0 || handleRect.height() <= 0 )
        return;

    if( state.isHovered() )
        _handlePainter.setBrush( _hoverBrush );
    else
        _handlePainter.setBrush( _contourBrush );

    _handlePainter.begin(context);
    _handlePainter.fillRect(handleRect);
}

///////////////////////////////////////////////////////////////////////////////
// PlatinumScrollBarRenderer
///////////////////////////////////////////////////////////////////////////////

PlatinumScrollBarRenderer::PlatinumScrollBarRenderer(std::size_t refs)
: ScrollBarRenderer(refs)
{
}


PlatinumScrollBarRenderer::~PlatinumScrollBarRenderer()
{
}


ScrollBarRenderer* PlatinumScrollBarRenderer::onCreate() const
{
    return new PlatinumScrollBarRenderer();
}


void PlatinumScrollBarRenderer::onPrepare(const StyleOptions& options,
                                          const StyleOptions& scrollBarOptions)
{
    const BackgroundOption* localBg = scrollBarOptions.find<BackgroundOption>();
    _background = localBg ? localBg->value()
                          : options.get<BackgroundOption>().value();

    const ContourOption* localContour = scrollBarOptions.find<ContourOption>();
    _contour = localContour ? localContour->value()
                            : options.get<ContourOption>().value();

    _accentColor = options.get<AccentColorOption>().value();

    _trackPainter.setBrush( _background );
    _trackPainter.setPen( _contour );
    _handlePainter.setBrush( Gfx::Brush(_contour.color()) );
    _handlePainter.setPen( _contour );
    _buttonPainter.setBrush( _background );
    _buttonPainter.setPen( _contour );
}


Gfx::SizeF PlatinumScrollBarRenderer::onMeasureFrame(PaintSurface& /*surface*/,
                                                      const Gfx::SizeF& contentSize,
                                                      Direction direction)
{
    double thickness = 16.0;

    if( direction == Direction::Top || direction == Direction::Bottom )
        return Gfx::SizeF(thickness, contentSize.height());

    return Gfx::SizeF(contentSize.width(), thickness);
}


Gfx::SizeF PlatinumScrollBarRenderer::onMeasureTrack(PaintSurface& /*surface*/,
                                                      Direction direction)
{
    double thickness = 16.0;

    if( direction == Direction::Top || direction == Direction::Bottom )
        return Gfx::SizeF(thickness, 0.0);

    return Gfx::SizeF(0.0, thickness);
}


Gfx::SizeF PlatinumScrollBarRenderer::onMeasureHandle(PaintSurface& /*surface*/,
                                                       Direction direction)
{
    double minLength = 16.0;
    double thickness = 16.0;

    if( direction == Direction::Top || direction == Direction::Bottom )
        return Gfx::SizeF(thickness, minLength);

    return Gfx::SizeF(minLength, thickness);
}


Gfx::SizeF PlatinumScrollBarRenderer::onMeasureButton(PaintSurface& /*surface*/,
                                                       Direction direction)
{
    double thickness = 16.0;

    if( direction == Direction::Top || direction == Direction::Bottom )
        return Gfx::SizeF(thickness, thickness);

    return Gfx::SizeF(thickness, thickness);
}


void PlatinumScrollBarRenderer::onLayoutChrome(PaintSurface& /*surface*/,
                                              const Gfx::RectF& rect,
                                              Direction direction,
                                              const Gfx::SizeF& buttonSize,
                                              Gfx::RectF& trackRect,
                                              Gfx::RectF& decreaseRect,
                                              Gfx::RectF& increaseRect)
{
    // Degenerate case: no space at all
    if( rect.width() <= 0 || rect.height() <= 0 )
    {
        decreaseRect = Gfx::RectF();
        increaseRect = Gfx::RectF();
        trackRect = Gfx::RectF();
        return;
    }

    bool vertical = (direction == Direction::Top || direction == Direction::Bottom);

    if( vertical )
    {
        double totalLen = rect.height();
        double btnH = buttonSize.height();
        double minThumb = btnH; // minimum useful thumb length

        if( totalLen >= 2 * btnH + minThumb )
        {
            // Normal case: two buttons plus track
            decreaseRect = Gfx::RectF( Gfx::PointF(rect.x(), rect.y()),
                                       Gfx::SizeF(rect.width(), btnH) );

            increaseRect = Gfx::RectF( Gfx::PointF(rect.x(), rect.y() + totalLen - btnH),
                                       Gfx::SizeF(rect.width(), btnH) );

            double trackY = rect.y() + btnH;
            double trackH = totalLen - 2 * btnH;

            trackRect = Gfx::RectF( Gfx::PointF(rect.x(), trackY),
                                    Gfx::SizeF(rect.width(), trackH) );
        }
        else
        {
            // Tight case: buttons collapse, entire rect becomes track
            decreaseRect = Gfx::RectF();
            increaseRect = Gfx::RectF();
            trackRect = rect;
        }
    }
    else
    {
        double totalLen = rect.width();
        double btnW = buttonSize.width();
        double minThumb = btnW;

        if( totalLen >= 2 * btnW + minThumb )
        {
            // Normal case: two buttons plus track
            decreaseRect = Gfx::RectF( Gfx::PointF(rect.x(), rect.y()),
                                       Gfx::SizeF(btnW, rect.height()) );

            increaseRect = Gfx::RectF( Gfx::PointF(rect.x() + totalLen - btnW, rect.y()),
                                       Gfx::SizeF(btnW, rect.height()) );

            double trackX = rect.x() + btnW;
            double trackW = totalLen - 2 * btnW;

            trackRect = Gfx::RectF( Gfx::PointF(trackX, rect.y()),
                                    Gfx::SizeF(trackW, rect.height()) );
        }
        else
        {
            // Tight case: buttons collapse, entire rect becomes track
            decreaseRect = Gfx::RectF();
            increaseRect = Gfx::RectF();
            trackRect = rect;
        }
    }
}


void PlatinumScrollBarRenderer::onLayoutHandle(PaintSurface& /*surface*/,
                                               const Gfx::RectF& trackRect,
                                               Direction direction,
                                               float fraction,
                                               float viewProportion,
                                               Gfx::RectF& handleRect)
{
    bool vertical = (direction == Direction::Top || direction == Direction::Bottom);

    double minHandleLen = 16.0;

    if( vertical )
    {
        double trackLen = trackRect.height();
        double handleLen = trackLen * viewProportion;
        if( handleLen < minHandleLen )
            handleLen = minHandleLen;
        if( handleLen > trackLen )
            handleLen = trackLen;

        double travel = trackLen - handleLen;
        double handleY = trackRect.y() + travel * fraction;

        handleRect = Gfx::RectF( Gfx::PointF(trackRect.x(), handleY),
                                 Gfx::SizeF(trackRect.width(), handleLen) );
    }
    else
    {
        double trackLen = trackRect.width();
        double handleLen = trackLen * viewProportion;
        if( handleLen < minHandleLen )
            handleLen = minHandleLen;
        if( handleLen > trackLen )
            handleLen = trackLen;

        double travel = trackLen - handleLen;
        double handleX = trackRect.x() + travel * fraction;

        handleRect = Gfx::RectF( Gfx::PointF(handleX, trackRect.y()),
                                 Gfx::SizeF(handleLen, trackRect.height()) );
    }
}


void PlatinumScrollBarRenderer::onRenderTrack(PaintContext& context,
                                              const Gfx::RectF& trackRect,
                                              Direction /*direction*/,
                                              const ScrollBarState& /*state*/)
{
    if( trackRect.width() <= 0 || trackRect.height() <= 0 )
        return;

    _trackPainter.begin(context);
    _trackPainter.fillRect(trackRect);
}


void PlatinumScrollBarRenderer::onRenderHandle(PaintContext& context,
                                               const Gfx::RectF& handleRect,
                                               Direction /*direction*/,
                                               const ScrollBarState& state)
{
    if( handleRect.width() <= 0 || handleRect.height() <= 0 )
        return;

    if( state.isHandleHovered() || state.isHandlePressed() )
        _handlePainter.setBrush( Gfx::Brush(_accentColor) );
    else
        _handlePainter.setBrush( Gfx::Brush(_contour.color()) );

    _handlePainter.begin(context);
    _handlePainter.fillRect(handleRect);
}


void PlatinumScrollBarRenderer::onRenderDecreaseButton(PaintContext& context,
                                                       const Gfx::RectF& buttonRect,
                                                       Direction direction,
                                                       const ScrollBarState& state)
{
    if( buttonRect.width() <= 0 || buttonRect.height() <= 0 )
        return;

    _trackPainter.begin(context);
    _trackPainter.fillRect(buttonRect);

    _buttonPainter.begin(context);
    _buttonPainter.setClip(buttonRect);

    Gfx::Pen cPen = _contour;
    cPen.setJoinStyle(Gfx::Pen::BevelJoin);

    if( state.isDecreaseHovered() || state.isDecreasePressed() )
    {
        cPen = Gfx::Pen( _accentColor, cPen.size(), cPen.style(),
                         cPen.capStyle(), cPen.joinStyle() );
    }

    _buttonPainter.setPen(cPen);
    _buttonPainter.setBrush( cPen.color() );

    const Gfx::Scaling& scaling = _buttonPainter.scaling();
    double pixelWidth = scaling.toLogical(1.0);
    bool vertical = (direction == Direction::Top || direction == Direction::Bottom);

    double dim = vertical ? buttonRect.width() : buttonRect.height();

    double triangleWidth = dim / 2.0;
    triangleWidth = scaling.align(triangleWidth);

    if( triangleWidth < pixelWidth * 2 )
        return;

    int widthPixels = Pt::lround(triangleWidth / pixelWidth);
    if( widthPixels % 2 != 0 )
        triangleWidth += pixelWidth;

    double triangleHeight = triangleWidth / 2.0;
    triangleHeight = scaling.align(triangleHeight);

    double x = (buttonRect.width() - triangleWidth) / 2.0;
    x = scaling.align(x);

    double y = (buttonRect.height() - triangleHeight) / 2.0;
    y = scaling.align(y);

    x += buttonRect.x();
    y += buttonRect.y();

    Gfx::PointF triangle[4];

    if( vertical )
    {
        // Up arrow
        triangle[0] = Gfx::PointF(x + triangleHeight, y);
        triangle[1] = Gfx::PointF(x + triangleWidth, y + triangleHeight);
        triangle[2] = Gfx::PointF(x, y + triangleHeight);
    }
    else
    {
        // Left arrow
        double lx = buttonRect.x() + (buttonRect.width() - triangleHeight) / 2.0;
        double ly = buttonRect.y() + (buttonRect.height() - triangleWidth) / 2.0;
        lx = scaling.align(lx);
        ly = scaling.align(ly);

        triangle[0] = Gfx::PointF(lx, ly + triangleWidth / 2.0);
        triangle[1] = Gfx::PointF(lx + triangleHeight, ly);
        triangle[2] = Gfx::PointF(lx + triangleHeight, ly + triangleWidth);
    }

    triangle[3] = triangle[0];
    _buttonPainter.fillPolygon(triangle, 3);
    _buttonPainter.drawPolyline(triangle, 4);
}


void PlatinumScrollBarRenderer::onRenderIncreaseButton(PaintContext& context,
                                                       const Gfx::RectF& buttonRect,
                                                       Direction direction,
                                                       const ScrollBarState& state)
{
    if( buttonRect.width() <= 0 || buttonRect.height() <= 0 )
        return;

    _trackPainter.begin(context);
    _trackPainter.fillRect(buttonRect);

    _buttonPainter.begin(context);
    _buttonPainter.setClip(buttonRect);

    Gfx::Pen cPen = _contour;
    cPen.setJoinStyle(Gfx::Pen::BevelJoin);

    if( state.isIncreaseHovered() || state.isIncreasePressed() )
    {
        cPen = Gfx::Pen( _accentColor, cPen.size(), cPen.style(),
                         cPen.capStyle(), cPen.joinStyle() );
    }

    _buttonPainter.setPen(cPen);
    _buttonPainter.setBrush( cPen.color() );

    const Gfx::Scaling& scaling = _buttonPainter.scaling();
    double pixelWidth = scaling.toLogical(1.0);
    bool vertical = (direction == Direction::Top || direction == Direction::Bottom);

    double dim = vertical ? buttonRect.width() : buttonRect.height();

    double triangleWidth = dim / 2.0;
    triangleWidth = scaling.align(triangleWidth);

    if( triangleWidth < pixelWidth * 2 )
        return;

    int widthPixels = Pt::lround(triangleWidth / pixelWidth);
    if( widthPixels % 2 != 0 )
        triangleWidth += pixelWidth;

    double triangleHeight = triangleWidth / 2.0;
    triangleHeight = scaling.align(triangleHeight);

    double x = (buttonRect.width() - triangleWidth) / 2.0;
    x = scaling.align(x);

    double y = (buttonRect.height() - triangleHeight) / 2.0;
    y = scaling.align(y);

    x += buttonRect.x();
    y += buttonRect.y();

    Gfx::PointF triangle[4];

    if( vertical )
    {
        // Down arrow
        triangle[0] = Gfx::PointF(x, y);
        triangle[1] = Gfx::PointF(x + triangleWidth, y);
        triangle[2] = Gfx::PointF(x + triangleHeight, y + triangleHeight);
    }
    else
    {
        // Right arrow
        double lx = buttonRect.x() + (buttonRect.width() - triangleHeight) / 2.0;
        double ly = buttonRect.y() + (buttonRect.height() - triangleWidth) / 2.0;
        lx = scaling.align(lx);
        ly = scaling.align(ly);

        triangle[0] = Gfx::PointF(lx, ly);
        triangle[1] = Gfx::PointF(lx + triangleHeight, ly + triangleWidth / 2.0);
        triangle[2] = Gfx::PointF(lx, ly + triangleWidth);
    }

    triangle[3] = triangle[0];
    _buttonPainter.fillPolygon(triangle, 3);
    _buttonPainter.drawPolyline(triangle, 4);
}

///////////////////////////////////////////////////////////////////////////////
// PlatinumListBoxRenderer
///////////////////////////////////////////////////////////////////////////////

PlatinumListBoxRenderer::PlatinumListBoxRenderer(std::size_t refs)
: ListBoxRenderer(refs)
{
}


PlatinumListBoxRenderer::~PlatinumListBoxRenderer()
{
}


ListBoxRenderer* PlatinumListBoxRenderer::onCreate() const
{
    return new PlatinumListBoxRenderer();
}


void PlatinumListBoxRenderer::onPrepare(const StyleOptions& options,
                                        const StyleOptions& listBoxOptions)
{
    const ViewBackgroundOption* localBg = listBoxOptions.find<ViewBackgroundOption>();
    _viewBackground = localBg ? localBg->value()
                              : options.get<ViewBackgroundOption>().value();

    const ContourOption* localContour = listBoxOptions.find<ContourOption>();
    _contour = localContour ? localContour->value()
                            : options.get<ContourOption>().value();
    _contour.setJoinStyle(Gfx::Pen::BevelJoin);

    _bgPainter.setBrush(_viewBackground);
    _framePainter.setPen(_contour);
}


Gfx::SizeF PlatinumListBoxRenderer::onMeasureFrame(PaintSurface& /*surface*/,
                                                   const Gfx::SizeF& contentSize)
{
    double inset = _framePainter.pen().size();
    return Gfx::SizeF(contentSize.width() + 2 * inset,
                      contentSize.height() + 2 * inset);
}


Gfx::RectF PlatinumListBoxRenderer::onLayoutFrame(PaintSurface& surface,
                                                  const Gfx::RectF& frameRect)
{
    double inset = surface.scaling().alignContour( _framePainter.pen().size() );
    double w = frameRect.width() - 2 * inset;
    double h = frameRect.height() - 2 * inset;
    if( w < 0 ) w = 0;
    if( h < 0 ) h = 0;

    return Gfx::RectF(Gfx::PointF(frameRect.x() + inset, frameRect.y() + inset),
                      Gfx::SizeF(w, h));
}


void PlatinumListBoxRenderer::onRenderBackground(PaintContext& context,
                                                 const Gfx::RectF& rect,
                                                 const ListBoxState& /*state*/)
{
    if( rect.width() <= 0 || rect.height() <= 0 )
        return;

    _bgPainter.begin(context);
    _bgPainter.fillRect(rect);
}


void PlatinumListBoxRenderer::onRenderChrome(PaintContext& context,
                                            const Gfx::RectF& rect,
                                            const ListBoxState& /*state*/)
{
    if( rect.width() <= 0 || rect.height() <= 0 )
        return;

    _framePainter.begin(context);

    double inset = _framePainter.scaling().alignContour( _framePainter.pen().size() ) / 2;

    double bw = rect.width() - 2 * inset;
    double bh = rect.height() - 2 * inset;
    if( bw <= 0 || bh <= 0 )
        return;

    Gfx::RectF borderRect(Gfx::PointF(rect.x() + inset, rect.y() + inset),
                          Gfx::SizeF(bw, bh));

    _framePainter.drawRect(borderRect);
}

///////////////////////////////////////////////////////////////////////////////
// PlatinumListItemRenderer
///////////////////////////////////////////////////////////////////////////////

PlatinumListItemRenderer::PlatinumListItemRenderer(std::size_t refs)
: ListItemRenderer(refs)
, _hasBackground(false)
{
}


PlatinumListItemRenderer::~PlatinumListItemRenderer()
{
}


ListItemRenderer* PlatinumListItemRenderer::onCreate() const
{
    return new PlatinumListItemRenderer();
}


void PlatinumListItemRenderer::onPrepare(const StyleOptions& options,
                                         const StyleOptions& listItemOptions)
{
    _highlightBrush = Gfx::Brush( options.get<HighlightColorOption>().value() );
    _highlightedTextColor = options.get<HighlightedTextColorOption>().value();

    const BackgroundOption* localBg = listItemOptions.find<BackgroundOption>();
    _hasBackground = (localBg != 0);
    if(localBg)
        _background = localBg->value();

    const Gfx::Font& baseFont = options.get<FontOption>().value();
    const FontOption* localFont = listItemOptions.find<FontOption>();
    _font = localFont ? localFont->getFont(baseFont) : baseFont;

    const TextColorOption* localText = listItemOptions.find<TextColorOption>();
    _textColor = localText ? localText->value()
                           : options.get<TextColorOption>().value();

    _textPainter.setFont(_font);
    _textPainter.setPen( Gfx::Pen(_textColor) );
}


Gfx::SizeF PlatinumListItemRenderer::onMeasureContent(PaintSurface& /*surface*/,
                                                      const Gfx::SizeF& iconSize,
                                                      const Gfx::SizeF& textSize)
{
    double spacing = (iconSize.isEmpty() || textSize.isEmpty()) ? 0
                   : _font.size() * 0.5;

    double w = iconSize.width() + spacing + textSize.width();
    double h = std::max<double>(iconSize.height(), textSize.height());

    return Gfx::SizeF(w, h);
}


Gfx::SizeF PlatinumListItemRenderer::onMeasureFrame(PaintSurface& /*surface*/,
                                                    const Gfx::SizeF& contentSize)
{
    return contentSize;
}


const Painter& PlatinumListItemRenderer::onGetTextPainter(PaintSurface& surface)
{
    _textPainter.begin(surface);
    return _textPainter;
}


Gfx::RectF PlatinumListItemRenderer::onLayoutFrame(PaintSurface& /*surface*/,
                                                   const Gfx::RectF& rect)
{
    return rect;
}


void PlatinumListItemRenderer::onLayoutContent(PaintSurface& /*surface*/,
                                               const Gfx::RectF& contentRect,
                                               const Gfx::SizeF& iconSize,
                                               const Gfx::SizeF& textSize,
                                               Gfx::RectF& iconRect,
                                               Gfx::RectF& textRect)
{
    double spacing = (iconSize.isEmpty() || textSize.isEmpty()) ? 0
                   : _font.size() * 0.5;

    double iconX = contentRect.x();
    double iconH = iconSize.height();
    if( iconH > contentRect.height() )
        iconH = contentRect.height();

    double iconW = iconSize.width();
    if( iconW > contentRect.width() )
        iconW = contentRect.width();

    double iconY = contentRect.y() + (contentRect.height() - iconH) / 2.0;
    iconRect.set( Gfx::PointF(iconX, iconY), Gfx::SizeF(iconW, iconH) );

    double textX = contentRect.x() + iconSize.width() + spacing;
    double textW = contentRect.width() - iconSize.width() - spacing;
    if( textW < 0 )
        textW = 0;

    textRect.set( Gfx::PointF(textX, contentRect.y()),
                  Gfx::SizeF(textW, contentRect.height()) );
}


void PlatinumListItemRenderer::onRenderBackground(PaintContext& context,
                                                  const Gfx::RectF& rect,
                                                  const ListItemState& state)
{
    if( rect.width() <= 0 || rect.height() <= 0 )
        return;

    if( state.isSelected() || state.isHighlighted() )
    {
        _bgPainter.setBrush(_highlightBrush);
        _bgPainter.begin(context);
        _bgPainter.fillRect(rect);
    }
    else if( _hasBackground )
    {
        _bgPainter.setBrush(_background);
        _bgPainter.begin(context);
        _bgPainter.fillRect(rect);
    }
}


void PlatinumListItemRenderer::onRenderText(PaintContext& context,
                                            const Gfx::RectF& textRect,
                                            const String& text,
                                            const Gfx::PointF& pos,
                                            const ListItemState& state)
{
    if( textRect.width() <= 0 || textRect.height() <= 0 )
        return;

    if( state.isSelected() || state.isHighlighted() )
        _textPainter.setPen( Gfx::Pen(_highlightedTextColor) );
    else
        _textPainter.setPen( Gfx::Pen(_textColor) );

    _textPainter.begin(context);
    _textPainter.setClip(textRect);
    _textPainter.drawText(pos, text);
}


void PlatinumListItemRenderer::onRenderIcon(PaintContext& context,
                                            const Gfx::RectF& iconRect,
                                            const Pixmap& picture,
                                            const Gfx::PointF& pos,
                                            const ListItemState& /*state*/)
{
    if( iconRect.width() <= 0 || iconRect.height() <= 0 )
        return;

    _iconPainter.begin(context);
    _iconPainter.setClip(iconRect);
    _iconPainter.setCompositionMode(Gfx::CompositionMode::SourceOver);
    _iconPainter.drawPixmap(pos, picture);
}

///////////////////////////////////////////////////////////////////////////////
// PlatinumComboBoxRenderer
///////////////////////////////////////////////////////////////////////////////

PlatinumComboBoxRenderer::PlatinumComboBoxRenderer(std::size_t refs)
: ComboBoxRenderer(refs)
, _inset(5)
{
}


PlatinumComboBoxRenderer::~PlatinumComboBoxRenderer()
{
}


ComboBoxRenderer* PlatinumComboBoxRenderer::onCreate() const
{
    return new PlatinumComboBoxRenderer();
}


void PlatinumComboBoxRenderer::onPrepare(const StyleOptions& options)
{
    _background = options.get<TextBackgroundOption>().value();

    _contour = options.get<ContourOption>().value();
    _contour.setJoinStyle(Gfx::Pen::BevelJoin);

    _foreground = options.get<ForegroundOption>().value();
    _font = options.get<FontOption>().value();
    _textColor = options.get<TextColorOption>().value();
    _accentColor = options.get<AccentColorOption>().value();

    _bgPainter.setBrush(_background);
    _bgPainter.setPen(_contour);

    _buttonPainter.setBrush(_foreground);
    _buttonPainter.setPen(_contour);

    _textPainter.setFont(_font);
    _textPainter.setPen( Gfx::Pen(_textColor) );
}


Gfx::SizeF PlatinumComboBoxRenderer::onMeasureFrame(PaintSurface& surface,
                                                     const Gfx::SizeF& contentSize)
{
    _textPainter.begin(surface);
    _textPainter.setFont(_font);

    Gfx::FontMetrics fm = _textPainter.fontMetrics();
    double textHeight = fm.ascent() + fm.descent();
    double entryHeight = textHeight + 2 * _inset;
    double buttonWidth = entryHeight;
    double entryWidth = contentSize.width() + 2 * _inset;

    double width = entryWidth + buttonWidth;
    double height = entryHeight;

    return Gfx::SizeF(width, height);
}


Gfx::SizeF PlatinumComboBoxRenderer::onMeasureButton(PaintSurface& surface)
{
    _textPainter.begin(surface);
    _textPainter.setFont(_font);

    Gfx::FontMetrics fm = _textPainter.fontMetrics();
    double textHeight = fm.ascent() + fm.descent();
    double h = textHeight + 2 * _inset;
    return Gfx::SizeF(h, h);
}


void PlatinumComboBoxRenderer::onLayoutChrome(PaintSurface& /*surface*/,
                                             const Gfx::RectF& rect,
                                             Gfx::RectF& entryRect,
                                             Gfx::RectF& buttonRect,
                                             Gfx::RectF& textRect)
{
    double buttonWidth = rect.height();
    if( buttonWidth > rect.width() )
        buttonWidth = rect.width();

    double entryW = rect.width() - buttonWidth;
    if( entryW < 0 )
        entryW = 0;

    entryRect.set( Gfx::PointF(rect.x(), rect.y()),
                   Gfx::SizeF(entryW, rect.height()) );

    buttonRect.set( Gfx::PointF(rect.x() + entryW, rect.y()),
                    Gfx::SizeF(buttonWidth, rect.height()) );

    double textW = entryW - 2 * _inset;
    if( textW < 0 )
        textW = 0;

    textRect.set( Gfx::PointF(entryRect.x() + _inset, entryRect.y()),
                  Gfx::SizeF(textW, entryRect.height()) );
}


const Painter& PlatinumComboBoxRenderer::onGetTextPainter(PaintSurface& surface)
{
    _textPainter.begin(surface);
    return _textPainter;
}


void PlatinumComboBoxRenderer::onRenderEntry(PaintContext& context,
                                             const Gfx::RectF& entryRect,
                                             const ComboBoxState& state)
{
    // Not used — onRenderChrome draws the integrated chrome
}


void PlatinumComboBoxRenderer::onRenderButton(PaintContext& context,
                                              const Gfx::RectF& buttonRect,
                                              const ComboBoxState& state,
                                              const ComboBoxButtonState& buttonState)
{
    // Not used — onRenderChrome draws the integrated chrome
}


void PlatinumComboBoxRenderer::onRenderChrome(PaintContext& context,
                                              const Gfx::RectF& rect,
                                              const Gfx::RectF& /*entryRect*/,
                                              const Gfx::RectF& buttonRect,
                                              const ComboBoxState& state,
                                              const ComboBoxButtonState& buttonState)
{
    if( rect.width() <= 0 || rect.height() <= 0 )
        return;

    _bgPainter.begin(context);

    Gfx::Pen cPen = _contour;

    if( state.isEnabled() )
    {
        if( state.isHighlighted() || state.isFocused() )
        {
            cPen = Gfx::Pen( _accentColor,
                             cPen.size(), cPen.style(),
                             cPen.capStyle(), cPen.joinStyle() );
        }
    }

    _bgPainter.setPen(cPen);

    const double inset = _bgPainter.scaling().alignContour( cPen.size() ) / 2;

    double bw = rect.width() - 2 * inset;
    double bh = rect.height() - 2 * inset;
    if( bw <= 0 || bh <= 0 )
        return;

    Gfx::RectF borderRect( Gfx::PointF(rect.x() + inset, rect.y() + inset),
                           Gfx::SizeF(bw, bh) );

    _bgPainter.fillRect(rect);
    _bgPainter.drawRect(borderRect);

    // Button separator and indicator
    if( buttonRect.width() <= 0 || buttonRect.height() <= 0 )
        return;

    _buttonPainter.begin(context);
    _buttonPainter.setClip(buttonRect);

    const Gfx::Scaling& scaling = _buttonPainter.scaling();
    double pixelWidth = scaling.toLogical(1.0);

    Gfx::Pen btnPen = cPen;

    if( buttonState.isHighlighted() ||
        buttonState.isPressed() )
    {
        btnPen = Gfx::Pen( _accentColor,
                           btnPen.size(), btnPen.style(),
                           btnPen.capStyle(), btnPen.joinStyle() );
    }

    _buttonPainter.setPen(btnPen);
    _buttonPainter.setBrush( btnPen.color() );

    // separator line
    double gap = scaling.align(5);
    _buttonPainter.drawLine( Gfx::PointF(buttonRect.x() + pixelWidth, buttonRect.y() + gap),
                             Gfx::PointF(buttonRect.x() + pixelWidth, buttonRect.y() + buttonRect.height() - gap) );

    // triangle indicator — only if button area is large enough
    double buttonWidth = buttonRect.width();
    double buttonHeight = buttonRect.height();

    double triangleWidth = buttonWidth / 3.0;
    triangleWidth = scaling.align(triangleWidth);

    if( triangleWidth < pixelWidth * 2 )
        return;

    int widthPixels = Pt::lround(triangleWidth / pixelWidth);
    if(widthPixels % 2 != 0)
        triangleWidth += pixelWidth;

    double triangleHeight = triangleWidth / 2.0;
    triangleHeight = scaling.align(triangleHeight);

    double x = (buttonWidth - triangleWidth) / 2;
    x = buttonRect.x() + scaling.align(x);

    double y = (buttonHeight - triangleHeight) / 2;
    y = buttonRect.y() + scaling.align(y);

    Gfx::PointF triangle[3];
    triangle[0] = Gfx::PointF(x, y);
    triangle[1] = Gfx::PointF(x + triangleWidth, y);
    triangle[2] = Gfx::PointF(x + triangleHeight, y + triangleHeight);

    _buttonPainter.fillPolygon(triangle, 3);
}


void PlatinumComboBoxRenderer::onRenderText(PaintContext& context,
                                            const Gfx::RectF& textRect,
                                            const String& text,
                                            const Gfx::PointF& textPos,
                                            const Gfx::RectF& cursor,
                                            const ComboBoxState& state)
{
    if( textRect.width() <= 0 || textRect.height() <= 0 )
        return;

    _textPainter.begin(context);
    _textPainter.setClip(textRect);
    _textPainter.drawText(textPos, text);

    if( state.isEditable() && state.isFocused() )
    {
        _textPainter.drawLine( cursor.topLeft(), cursor.bottomLeft() );
    }
}

///////////////////////////////////////////////////////////////////////////////
// PlatinumTabViewRenderer
///////////////////////////////////////////////////////////////////////////////

PlatinumTabViewRenderer::PlatinumTabViewRenderer(std::size_t refs)
: TabViewRenderer(refs)
, _inset(0)
{
}


PlatinumTabViewRenderer::~PlatinumTabViewRenderer()
{
}


TabViewRenderer* PlatinumTabViewRenderer::onCreate() const
{
    return new PlatinumTabViewRenderer();
}


void PlatinumTabViewRenderer::onPrepare(const StyleOptions& options,
                                        const StyleOptions& tabViewOptions)
{
    const Gfx::Font& baseFont = options.get<FontOption>().value();
    const FontOption* localFont = tabViewOptions.find<FontOption>();
    _font = localFont ? localFont->getFont(baseFont) : baseFont;
    _inset = _font.size() / 2.0;

    const ContourOption* localContour = tabViewOptions.find<ContourOption>();
    _contour = localContour ? localContour->value()
                            : options.get<ContourOption>().value();
    _contour.setJoinStyle(Gfx::Pen::BevelJoin);

    const BackgroundOption* localBg = tabViewOptions.find<BackgroundOption>();
    _background = localBg ? localBg->value()
                          : options.get<BackgroundOption>().value();

    const TextColorOption* localText = tabViewOptions.find<TextColorOption>();
    _textColor = localText ? localText->value()
                           : options.get<TextColorOption>().value();

    const AccentColorOption* localAccent = tabViewOptions.find<AccentColorOption>();
    _accentColor = localAccent ? localAccent->value()
                               : options.get<AccentColorOption>().value();

    _bgPainter.setBrush(_background);
    _framePainter.setPen(_contour);

    _textPainter.setFont(_font);
    _textPainter.setPen( Gfx::Pen(_textColor) );

    _activeTextPainter.setFont(_font);
    _activeTextPainter.setPen( Gfx::Pen(_accentColor) );
}


Gfx::SizeF PlatinumTabViewRenderer::onMeasureTab(PaintSurface& surface,
                                                   const Pt::String& text)
{
    Gfx::SizeF s;
    s.setHeight( _font.size() * 2.4 );

    _textPainter.begin(surface);
    Gfx::TextMetrics tm = _textPainter.textMetrics(text);
    s.setWidth( tm.advance() + _inset * 2.0 );

    return s;
}


Gfx::RectF PlatinumTabViewRenderer::onLayoutTab(PaintSurface& surface,
                                                  const Gfx::RectF& tabRect)
{
    double w = tabRect.width() - _inset * 2.0;
    if( w < 0 )
        w = 0;

    return Gfx::RectF(
        Gfx::PointF(tabRect.x() + _inset, tabRect.y()),
        Gfx::SizeF(w, tabRect.height()) );
}


const Painter& PlatinumTabViewRenderer::onGetTextPainter(PaintSurface& surface)
{
    _textPainter.begin(surface);
    return _textPainter;
}


void PlatinumTabViewRenderer::onRenderBackground(PaintContext& context,
                                                  const Gfx::RectF& contentRect,
                                                  const TabViewState& /*state*/)
{
    if( contentRect.width() <= 0 || contentRect.height() <= 0 )
        return;

    _bgPainter.begin(context);
    _bgPainter.setBrush(_background);
    _bgPainter.fillRect(contentRect);
}


void PlatinumTabViewRenderer::onRenderChrome(PaintContext& context,
                                             const Gfx::RectF& contentRect,
                                             const Gfx::RectF& /*activeTabRect*/,
                                             const TabViewState& /*state*/)
{
    if( contentRect.width() <= 0 || contentRect.height() <= 0 )
        return;

    _framePainter.begin(context);
    _framePainter.setPen(_contour);
    _framePainter.drawRect(contentRect);
}


void PlatinumTabViewRenderer::onRenderTab(PaintContext& context,
                                          const Gfx::RectF& tabRect,
                                          const Pt::String& text,
                                          const Gfx::PointF& textPos,
                                          const TabViewItemState& state)
{
    if( tabRect.width() <= 0 || tabRect.height() <= 0 )
        return;

    bool active = state.isActive();

    Painter& textPainter = active ? _activeTextPainter : _textPainter;
    textPainter.begin(context);
    textPainter.setClip(tabRect);
    textPainter.drawText(textPos, text);

    // Draw underline indicator
    _framePainter.begin(context);

    if( active )
        _framePainter.setPen( Gfx::Pen(_accentColor) );
    else
        _framePainter.setPen( Gfx::Pen(_contour) );

    double halfInset = _inset / 2.0;
    Gfx::PointF from(tabRect.left() + halfInset,
                      tabRect.bottom() - 1.0);
    Gfx::PointF to(tabRect.right() - halfInset,
                    tabRect.bottom() - 1.0);

    if( to.x() > from.x() )
        _framePainter.drawLine(from, to);
}

///////////////////////////////////////////////////////////////////////////////
// PlatinumStyle
///////////////////////////////////////////////////////////////////////////////

PlatinumStyle::PlatinumStyle()
{
    FacetPtr<PlatinumRendererBase> base(new PlatinumRendererBase);
    set(base.get());
    set(new PlatinumButtonRenderer(base));
    set(new PlatinumCheckBoxRenderer);
    set(new PlatinumPanelRenderer(base));
    set(new PlatinumLineEditRenderer);
    set(new PlatinumScrollBarRenderer);
    set(new PlatinumProgressBarRenderer);
    set(new PlatinumSliderRenderer);
    set(new PlatinumListBoxRenderer);
    set(new PlatinumListItemRenderer);
    set(new PlatinumComboBoxRenderer);
    set(new PlatinumSpinBoxRenderer);
    set(new PlatinumTabViewRenderer);
}


PlatinumStyle::~PlatinumStyle()
{
}

} // namespace

} // namespace
