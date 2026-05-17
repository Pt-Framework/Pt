/* Copyright (C) 2013 Marc Boris Duerner 
   Copyright (C) 2013 Laurentiu-Gheorghe Crisan
   Copyright (C) 2017 Ilja Maier

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
#include <Pt/Forms/LineEdit.h>
#include <Pt/Forms/PushButton.h>
#include <Pt/Forms/CheckBox.h>
#include <Pt/Forms/MenuBar.h>
#include <Pt/Forms/Menu.h>
#include <Pt/Forms/MenuItem.h>
#include <Pt/Forms/ScrollBar.h>
#include <Pt/Forms/ProgressBar.h>
#include <Pt/Forms/Slider.h>
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

} // namespace

namespace Pt {

namespace Forms {

///////////////////////////////////////////////////////////////////////////////
// PlatinumRendererBase
///////////////////////////////////////////////////////////////////////////////

PlatinumRendererBase::PlatinumRendererBase()
{
}

    
PlatinumRendererBase::~PlatinumRendererBase()
{
}


void PlatinumRendererBase::renderFrame(Painter& painter,
                                       const Gfx::RectF& rect,
                                       const Gfx::Pen& pen,
                                       double corner) const
{
    double inset = painter.scaling().alignContour( pen.size() ) / 2;

    Gfx::Polygon polygon = getPolygon(rect, inset, corner);

    painter.setPen(pen);
    painter.drawPolyline(&polygon[0], polygon.size());
}


void PlatinumRendererBase::renderFrame(Painter& painter,
                                       const Gfx::RectF& rect,
                                       double penSize,
                                       double corner) const
{
    double inset = painter.scaling().alignContour(penSize) / 2;

    Gfx::Polygon polygon = getPolygon(rect, inset, corner);

    painter.drawPolyline(&polygon[0], polygon.size());
}


void PlatinumRendererBase::renderPlane(Painter& painter,
                                       const Gfx::RectF& rect,
                                       const Gfx::Brush& brush,
                                       double corner) const

{
    double inset = painter.scaling().toLogical(0.5);

    Gfx::Polygon polygon = getPolygon(rect, inset, corner);

    painter.setBrush(brush);
    painter.fillPolygon(&polygon[0], polygon.size());
}


void PlatinumRendererBase::renderPlane(Painter& painter,
                                       const Gfx::RectF& rect,
                                       double corner) const
{
    double inset = painter.scaling().toLogical(0.5);

    Gfx::Polygon polygon = getPolygon(rect, inset, corner);

    painter.fillPolygon(&polygon[0], polygon.size());
}


Gfx::Polygon PlatinumRendererBase::getPolygon(const Gfx::RectF& rect, 
                                             double inset, double corner)
{
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
// PlatinumLabelRenderer
///////////////////////////////////////////////////////////////////////////////

PlatinumLabelRenderer::PlatinumLabelRenderer(std::size_t refs)
: LabelRenderer(refs)
{
}


PlatinumLabelRenderer::~PlatinumLabelRenderer()
{
}


LabelRenderer* PlatinumLabelRenderer::onCreate() const
{
    return new PlatinumLabelRenderer();
}


void PlatinumLabelRenderer::onPrepare(const StyleOptions& options)
{
    const Gfx::Brush* bg = background();
    if( bg )
        _bgPainter.setBrush(*bg);

    const Gfx::Pen* pen = contour();
    if( pen )
    {
        Gfx::Pen framePen = *pen;
        framePen.setJoinStyle(Gfx::Pen::BevelJoin);
        _framePainter.setPen(framePen);
    }

    _textPainter.setFont( font() );
    _textPainter.setPen( textColor() );
}


void PlatinumLabelRenderer::onRenderBackground(PaintContext& context,
                                               const Gfx::RectF& rect,
                                               const StyleOptions& options,
                                               StyleFlags /*state*/)
{
    if( ! background() )
        return;

    _bgPainter.begin(context);
    _baseRenderer.renderPlane(_bgPainter, rect, options.cornerRadius());
}


Gfx::SizeF PlatinumLabelRenderer::onMeasureFrame(PaintSurface& /*surface*/,
                                                  const Gfx::SizeF& contentSize)
{
    return contentSize;
}


Gfx::RectF PlatinumLabelRenderer::onLayoutFrame(PaintSurface& /*surface*/,
                                                 const Gfx::RectF& frameRect)
{
    return frameRect;
}


void PlatinumLabelRenderer::onRenderFrame(PaintContext& context,
                                          const Gfx::RectF& rect,
                                          const StyleOptions& options,
                                          StyleFlags /*state*/)
{
    if( ! contour() )
        return;

    _framePainter.begin(context);
    _baseRenderer.renderFrame(_framePainter, rect,
                              _framePainter.pen().size(),
                              options.cornerRadius());
}


const Painter& PlatinumLabelRenderer::onGetTextPainter(PaintSurface& surface)
{
    _textPainter.begin(surface);
    return _textPainter;
}


void PlatinumLabelRenderer::onRenderText(PaintContext& context,
                                         const Gfx::RectF& rect,
                                         const StyleOptions& /*options*/,
                                         const String& text,
                                         const Gfx::PointF& pos,
                                         StyleFlags /*state*/)
{
    _textPainter.begin(context);
    _textPainter.setClip(rect);
    _textPainter.drawText(pos, text);
}


void PlatinumLabelRenderer::onRenderIcon(PaintContext& context,
                                         const Gfx::RectF& rect,
                                         const StyleOptions& /*options*/,
                                         const Pixmap& picture,
                                         const Gfx::PointF& pos,
                                         StyleFlags /*state*/)
{
    _bgPainter.begin(context);
    _bgPainter.setClip(rect);
    Gfx::CompositionMode prev = _bgPainter.compositionMode();
    _bgPainter.setCompositionMode(Gfx::CompositionMode::SourceOver);
    _bgPainter.drawPixmap(pos, picture);
    _bgPainter.setCompositionMode(prev);
}

///////////////////////////////////////////////////////////////////////////////
// PlatinumButtonRenderer
///////////////////////////////////////////////////////////////////////////////

PlatinumButtonRenderer::PlatinumButtonRenderer(std::size_t refs)
: ButtonRenderer(refs)
{
}


PlatinumButtonRenderer::~PlatinumButtonRenderer()
{
}


ButtonRenderer* PlatinumButtonRenderer::onCreate() const
{
    return new PlatinumButtonRenderer(1);
}


void PlatinumButtonRenderer::onPrepare(const StyleOptions& options)
{
    Gfx::Pen cPen = contour();
    cPen.setJoinStyle(Gfx::Pen::BevelJoin);

    _normalPainter.setBrush( foreground() );
    _normalPainter.setPen( cPen );

    Gfx::Brush pressedBrush( accentColor() );
    _pressedPainter.setBrush( pressedBrush );
    _pressedPainter.setPen( cPen );

    Gfx::Brush highlightBrush( highlightColor() );
    _highlightPainter.setBrush( highlightBrush );
    _highlightPainter.setPen( cPen );

    _textPainter.setFont( font() );
    _textPainter.setPen( textColor() );
}


Gfx::SizeF PlatinumButtonRenderer::onMeasureSurface(PaintSurface& /*surface*/,
                                                     const Gfx::SizeF& contentSize)
{
    Spacing ins(3);
    return Gfx::SizeF(contentSize.width() + ins.leftRight(),
                      contentSize.height() + ins.topBottom());
}


Gfx::RectF PlatinumButtonRenderer::onLayoutSurface(PaintSurface& /*surface*/,
                                                    const Gfx::RectF& surfaceRect)
{
    Spacing ins(3);
    return Gfx::RectF(Gfx::PointF(ins.left(), ins.top()),
                       Gfx::SizeF(surfaceRect.width() - ins.leftRight(),
                                  surfaceRect.height() - ins.topBottom()));
}


void PlatinumButtonRenderer::onRenderSurface(PaintContext& context,
                                             const Gfx::RectF& rect,
                                             const StyleOptions& options,
                                             ButtonStyleFlags state)
{
    Painter* painter = 0;

    if( state.has(ButtonStyleFlags::Pressed) )
        painter = &_pressedPainter;
    else if( state.has(StyleFlags::Highlighted) )
        painter = &_highlightPainter;
    else
        painter = &_normalPainter;

    painter->begin(context);

    const Gfx::Scaling& scaling = painter->scaling();
    double corner = scaling.align(1.0);

    _baseRenderer.renderPlane(*painter, rect, painter->brush(), corner);

    _baseRenderer.renderFrame(*painter, rect, painter->pen(), corner);

    if( state.has(StyleFlags::Focused) )
    {
        double inset = scaling.toLogical(0.5);
        double focusOffset = scaling.align(2.0) + inset;

        Gfx::RectF focusRect(rect);
        focusRect.shift(focusOffset, focusOffset);
        focusRect.expand(-2 * focusOffset, -2 * focusOffset);

        Gfx::Pen savedPen = painter->pen();
        Gfx::Pen focusPen( savedPen.color(), 1, Gfx::Pen::Dash );
        painter->setPen(focusPen);
        painter->drawRect(focusRect);
        painter->setPen(savedPen);
    }
}


const Painter& PlatinumButtonRenderer::onGetTextPainter(PaintSurface& surface)
{
    _textPainter.begin(surface);
    return _textPainter;
}


void PlatinumButtonRenderer::onRenderText(PaintContext& context,
                                          const Gfx::RectF& rect,
                                          const StyleOptions& options,
                                          const String& text,
                                          const Gfx::PointF& pos,
                                          ButtonStyleFlags state)
{
    if( state.has(ButtonStyleFlags::Pressed) && state.has(ButtonStyleFlags::Flat) )
    {
        Gfx::Pen accentPen( accentColor() );
        _textPainter.setPen(accentPen);
    }

    _textPainter.begin(context);
    _textPainter.setClip(rect);
    _textPainter.drawText(pos, text);
}


Gfx::RectF PlatinumButtonRenderer::onLayoutMnemonic(PaintSurface& surface,
                                                    const String& text,
                                                    const Gfx::PointF& textPos,
                                                    const Gfx::FontMetrics& fontMet,
                                                    String::size_type mnemonicIndex)
{
    if( mnemonicIndex == String::npos || mnemonicIndex >= text.size() )
        return Gfx::RectF();

    _textPainter.begin(surface);

    Pt::String leftText(text, 0, mnemonicIndex);
    Gfx::TextMetrics fmLeft = _textPainter.textMetrics(leftText);

    Pt::String charText(1, text[mnemonicIndex]);
    Gfx::TextMetrics fmChar = _textPainter.textMetrics(charText);

    return Gfx::RectF(
        Gfx::PointF(textPos.x() + fmLeft.advance(), textPos.y()),
        Gfx::SizeF(fmChar.advance(), fontMet.descent()) );
}


void PlatinumButtonRenderer::onRenderMnemonic(PaintContext& context,
                                              const Gfx::RectF& rect,
                                              const StyleOptions& options,
                                              const Gfx::RectF& mnemonic,
                                              ButtonStyleFlags state)
{
    if( mnemonic.isNull() )
        return;

    _textPainter.begin(context);
    _textPainter.setClip(rect);

    double mnemonicY = mnemonic.top() + 1;
    _textPainter.drawLine( Gfx::PointF(mnemonic.left(), mnemonicY),
                           Gfx::PointF(mnemonic.right(), mnemonicY) );
}


void PlatinumButtonRenderer::onPrepareIcon(const StyleOptions& options,
                                           const Gfx::Image& icon,
                                           Pixmap& picture,
                                           ButtonStyleFlags state) const
{
    if( state.has(ButtonStyleFlags::Pressed) && state.has(ButtonStyleFlags::Flat) )
    {
        Gfx::Color tintColor = accentColor();

        Gfx::Image tintedIcon = icon;
        Gfx::PixelView pixelView(tintedIcon);

        for(Gfx::PixelView::Iterator it = pixelView.begin(); it != pixelView.end(); ++it)
        {
            Gfx::Color color = it->getColor();

            color.setRed( tintColor.red() );
            color.setGreen( tintColor.green() );
            color.setBlue( tintColor.blue() );

            (*it) = color;
        }

        picture.reset(tintedIcon);
    }
    else
    {
        picture.reset(icon);
    }
}


void PlatinumButtonRenderer::onRenderIcon(PaintContext& context,
                                          const Gfx::RectF& rect,
                                          const StyleOptions& options,
                                          const Pixmap& picture,
                                          const Gfx::PointF& pos,
                                          ButtonStyleFlags state)
{
    _normalPainter.begin(context);
    _normalPainter.setClip(rect);
    Gfx::CompositionMode prev = _normalPainter.compositionMode();
    _normalPainter.setCompositionMode(Gfx::CompositionMode::SourceOver);
    _normalPainter.drawPixmap(pos, picture);
    _normalPainter.setCompositionMode(prev);
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


void PlatinumCheckBoxRenderer::onPrepare(const CheckBox& cb,
                                         const StyleOptions& options,
                                         Gfx::Brush& brush,
                                         Gfx::Pen& contour,
                                         Gfx::Font& font,
                                         Gfx::Pen& textPen,
                                         Gfx::SizeF& boxSize) const 
{
    contour.setJoinStyle(Gfx::Pen::BevelJoin);

    boxSize.set( font.size() * 1.2, font.size() * 1.2 );
}


void PlatinumCheckBoxRenderer::onRenderBox(const CheckBox& cb,
                                           const StyleOptions& options,
                                           Painter& painter, 
                                           const Gfx::RectF& rect,
                                           const Gfx::RectF& box,
                                           const Gfx::Brush& brush,
                                           const Gfx::Pen& pen) const
{
    const Gfx::Scaling& scaling = painter.scaling();

    Gfx::RectF boxRect = scaling.align(box);
    double inset = scaling.toLogical(0.5);
    double checkOffset = scaling.alignContour(3);

    Gfx::RectF checkRect = boxRect;
    checkRect.shift(checkOffset, checkOffset);
    checkRect.shrink(2 * checkOffset, 2 * checkOffset);

    Gfx::RectF borderRect = boxRect;
    borderRect.shift(inset, inset);
    borderRect.shrink(2 * inset, 2 * inset);

    painter.setBrush(brush);
    painter.fillRect(boxRect);

    painter.setPen(pen);
    painter.drawRect(borderRect);

    if( cb.isChecked() )
    {
        painter.setBrush( options.textColor() );
        painter.fillRect(checkRect);
    }
}


void PlatinumCheckBoxRenderer::onRenderText(const CheckBox& cb,
                                            const StyleOptions& options,
                                            Painter& painter, 
                                            const Gfx::RectF& rect,
                                            const String& text,
                                            const Gfx::PointF& textPos,
                                            const Gfx::TextMetrics& textMetric,
                                            const Gfx::Font& font, 
                                            const Gfx::Pen& textPen,
                                            const Gfx::RectF& mnemonic) const 
{
    painter.setFont(font);
    painter.setPen(textPen);
    painter.drawText(textPos, text);

    if( ! mnemonic.isNull() )
    {
        double menmonicY = textPos.y() + 1;
        painter.drawLine( Gfx::PointF(mnemonic.left(), menmonicY), 
                          Gfx::PointF(mnemonic.right(), menmonicY) );
    }

    if( cb.hasFocus() )
    {       
        Gfx::FontMetrics fm = painter.fontMetrics();

        Gfx::RectF focusRect( Gfx::PointF(textPos.x() - 2, 
                                          textPos.y() - fm.ascent()), 
                              Gfx::SizeF(textMetric.advance() + 4, 
                                         fm.height() ) );
        
        Gfx::Pen pen(textPen.color(), 1, Gfx::Pen::Dash);
        painter.setPen(pen);
        painter.drawRect(focusRect);
    }
}

///////////////////////////////////////////////////////////////////////////////
// PlatinumPanelRenderer
///////////////////////////////////////////////////////////////////////////////

PlatinumPanelRenderer::PlatinumPanelRenderer(std::size_t refs)
: PanelRenderer(refs)
{
}

    
PlatinumPanelRenderer::~PlatinumPanelRenderer()
{
}


void PlatinumPanelRenderer::onRenderBackground(const Panel& p,
                                               const StyleOptions& options,
                                               Painter& painter, 
                                               const Gfx::RectF& rect,
                                               const Gfx::Brush& brush) const 
{
    Gfx::RectF borderRect( p.size() );
    double corner = painter.scaling().align(1.0);

    _baseRenderer.renderPlane(painter, borderRect, brush, corner);
}


void PlatinumPanelRenderer::onRenderFrame(const Panel& p,
                                          const StyleOptions& options,
                                          Painter& painter, 
                                          const Gfx::RectF& rect,
                                          const Gfx::Pen& pen) const 
{
    Gfx::RectF borderRect( p.size() );
    double corner = painter.scaling().align(1.0);

    _baseRenderer.renderFrame(painter, borderRect, pen, corner);
}


///////////////////////////////////////////////////////////////////////////////
// PlatinumLineEditRenderer
///////////////////////////////////////////////////////////////////////////////

PlatinumLineEditRenderer::PlatinumLineEditRenderer(std::size_t refs)
: LineEditRenderer(refs)
{
}

    
PlatinumLineEditRenderer::~PlatinumLineEditRenderer()
{
}


void PlatinumLineEditRenderer::onPrepare(const LineEdit& le, 
                                         const StyleOptions& options,
                                         Gfx::Brush& brush,
                                         Gfx::Pen& contour,
                                         Gfx::Font& font,
                                         Gfx::Pen& textPen) const
{

    if( ! le.hasFocus() && le.isEmpty() )
    {
        textPen = contour;
    }

    if( le.isEnabled() )
    {
        if( le.isHighlighted() || le.hasFocus() )
        {
            contour = Gfx::Pen( options.accentColor(), 
                                contour.size(), contour.style(), 
                                contour.capStyle(), contour.joinStyle() );
        }
    }
}


void PlatinumLineEditRenderer::onRenderBackground(const LineEdit& le, 
                                                  const StyleOptions& options,
                                                  Painter& painter, 
                                                  const Gfx::RectF& rect,
                                                  const Gfx::Pen& contour,
                                                  const Gfx::Brush& brush) const
{
    const double inset = painter.scaling().alignContour( contour.size() ) / 2;

    Gfx::RectF borderRect( le.size() );
    borderRect.shift(inset, inset);
    borderRect.shrink(2 * inset, 2 * inset);

    painter.setBrush(brush);
    painter.fillRect(borderRect);

    painter.setPen(contour);
    painter.drawRect(borderRect);
}


void PlatinumLineEditRenderer::onRenderText(const LineEdit& le, 
                                            const StyleOptions& options,
                                            Painter& painter, 
                                            const Gfx::RectF& rect,
                                            const String& text,
                                            const Gfx::PointF& textPos,
                                            const Gfx::Font& font,
                                            const Gfx::Pen& textPen) const
{
    painter.setPen(textPen);
    painter.setFont(font);
    painter.drawText(textPos, text);
}


void PlatinumLineEditRenderer::onRenderCursor(const LineEdit& le, 
                                              const StyleOptions& options,
                                              Painter& painter, 
                                              const Gfx::RectF& rect,
                                              const Gfx::RectF& cursorRect ) const
{
    painter.setPen( options.textColor() );
    
    painter.drawLine( cursorRect.topLeft(),
                      cursorRect.bottomLeft() );
}

///////////////////////////////////////////////////////////////////////////////
// PlatinumMenuRenderer
///////////////////////////////////////////////////////////////////////////////

PlatinumMenuRenderer::PlatinumMenuRenderer(std::size_t refs)
: MenuRenderer(refs)
{
}

    
PlatinumMenuRenderer::~PlatinumMenuRenderer()
{
}


void PlatinumMenuRenderer::onPrepare(const Menu& m, 
                                     const StyleOptions& options,
                                     Gfx::Brush& brush,
                                     Gfx::Pen& contour) const
{
}


void PlatinumMenuRenderer::onRenderBackground(const Menu& m, 
                                              const StyleOptions& options,
                                              Painter& painter, 
                                              const Gfx::RectF& rect,
                                              const Gfx::Brush& brush,
                                              const Gfx::Pen& contour) const
{
    Gfx::SizeF size = m.size();

    size -= 1;

    double inset = painter.scaling().alignContour( contour.size() ) / 2;

    //
    // icon strip on the left side
    //
    
    // TODO: use separate render funtion for iconstrip

    Pt::ssize_t iconWidth = m.iconWidth();

    if(iconWidth > 0)
    {
        Gfx::RectF iconStrip( Gfx::PointF(0, 0),
                              Gfx::SizeF(iconWidth, size.height()) );
                
         Gfx::Brush brush = Gfx::Brush::verticalGradient(brush.color(),
                                                         Gfx::Color(253, 253, 253) );

        painter.setBrush(brush);
        painter.fillRect(iconStrip);
    }

    //
    // menu border
    //
    Gfx::RectF borderRect( m.size() );
    borderRect.shift(inset, inset);
    borderRect.shrink(2 * inset, 2 * inset);

    painter.setPen(contour);
    painter.drawRect(borderRect);
}


void PlatinumMenuRenderer::onPrepareItem(const MenuItem& m, 
                                         const StyleOptions& options,
                                         const Gfx::Image& icon,
                                         PixmapSurface& picture,
                                         Gfx::Brush& brush,
                                         Gfx::Pen& contour,
                                         Gfx::Font& font,
                                         Gfx::Pen& textPen) const
{
    picture.reset(icon);

    if( m.isHighlighted() )
        brush =  options.highlightColor();
}


void PlatinumMenuRenderer::onRenderItem(const MenuItem& m, 
                                        const StyleOptions& options,
                                        Painter& painter, 
                                        const Gfx::RectF& rect,
                                        Gfx::Brush& brush,
                                        Gfx::Pen& contour) const
{
    bool highlight = m.isHighlighted();
    if(highlight)
    {
        painter.setBrush(brush);
        painter.fillRect(rect);
    }
}


void PlatinumMenuRenderer::onRenderIndicator(const MenuItem& m, 
                                             const StyleOptions& options,
                                             Painter& painter, 
                                             const Gfx::RectF& rect) const
{
    static const double indicatorWidth = 5.0;

    double x = m.size().width() - indicatorWidth - m.padding().right();
    double y = m.size().height() / 2;

    Gfx::PointF indicator[3] = { Gfx::PointF(x - 3, y - 4),
                                 Gfx::PointF(x + 1, y),
                                 Gfx::PointF(x - 3, y + 4) };
  
    Gfx::Brush brush( m.textColor() );
    painter.setBrush(brush);
    painter.fillPolygon(indicator, 3);
}

///////////////////////////////////////////////////////////////////////////////
// PlatinumMenuBarRenderer
///////////////////////////////////////////////////////////////////////////////

PlatinumMenuBarRenderer::PlatinumMenuBarRenderer(std::size_t refs)
: MenuBarRenderer(refs)
{
}

    
PlatinumMenuBarRenderer::~PlatinumMenuBarRenderer()
{
}


void PlatinumMenuBarRenderer::onPrepare(const MenuBar& m, 
                                        const StyleOptions& options,
                                        Gfx::Brush& brush,
                                        Gfx::Pen& contour) const
{
}


void PlatinumMenuBarRenderer::onRenderBackground(const MenuBar& m, 
                                                 const StyleOptions& options,
                                                 Painter& painter, 
                                                 const Gfx::RectF& rect,
                                                 const Gfx::Brush& brush,
                                                 const Gfx::Pen& contour) const
{
    painter.setBrush(brush);
    painter.fillRect(rect);
}


void PlatinumMenuBarRenderer::onPrepareItem(const MenuBarItem& m, 
                                            const StyleOptions& options, 
                                            Gfx::Brush& brush,
                                            Gfx::Pen& contour,
                                            Gfx::Font& font,
                                            Gfx::Pen& textPen) const
{
    if( m.isHighlighted() )
        brush = options.highlightColor();
}


void PlatinumMenuBarRenderer::onRenderItem(const MenuBarItem& m, 
                                           const StyleOptions& options,
                                           Painter& painter, 
                                           const Gfx::RectF& rect,
                                           const Gfx::Brush& brush,
                                           const Gfx::Pen& contour) const
{
    if( m.isHighlighted() )
    {
        painter.setBrush(brush);
        painter.fillRect( Gfx::RectF(Gfx::PointF(0,0), m.size()) );
    }
}


void PlatinumMenuBarRenderer::onRenderItemText(const MenuBarItem& m,
                                               const StyleOptions& options,
                                               Painter& painter, 
                                               const Gfx::RectF& rect,
                                               const String& text,
                                               const Gfx::PointF& textPos,
                                               const Gfx::Font& font, 
                                               const Gfx::Pen& textPen,
                                               const Gfx::RectF& mnemonic) const 
{
    painter.setFont(font);
    painter.setPen(textPen);
    painter.drawText(textPos, text);

    if( ! mnemonic.isNull() )
    {
        double menmonicY = textPos.y() + 1;
        painter.drawLine( Gfx::PointF(mnemonic.left(), menmonicY), 
                          Gfx::PointF(mnemonic.right(), menmonicY) );
    }
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


void PlatinumScrollBarRenderer::onPrepare(const ScrollBar& s,
                                          const StyleOptions& options,
                                          Gfx::Brush& background,
                                          Gfx::Brush& foreground,
                                          Gfx::Pen& contour) const
{
}


void PlatinumScrollBarRenderer::onRender(const ScrollBar& s,
                                         const StyleOptions& options,
                                         Painter& painter,
                                         const Gfx::RectF& rect,
                                         const Gfx::RectF& handleRect,
                                         const Gfx::Brush& background,
                                         const Gfx::Brush& foreground,
                                         const Gfx::Pen& contour) const
{
    const double inset = painter.scaling().alignContour( contour.size() ) / 2;

    Gfx::RectF borderRect = Gfx::RectF( s.size() );
    borderRect.shift(inset, inset);
    borderRect.shrink(2 * inset, 2 * inset);

    Gfx::RectF gripRect =  handleRect;
    gripRect.shift(inset, inset);
    gripRect.shrink(2 * inset, 2 * inset);

    painter.setBrush(background);
    painter.fillRect(rect);

    painter.setPen(contour);
    painter.drawRect(borderRect);

    painter.setBrush(foreground);
    painter.fillRect(gripRect);

    painter.setPen(contour);
    painter.drawRect(gripRect);
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


void PlatinumProgressBarRenderer::onPrepare(const ProgressBar&    p,
                                            const StyleOptions&  options,
                                            Gfx::Brush&          background,
                                            Gfx::Brush&          foreground,
                                            Gfx::Pen&            contour,
                                            Gfx::Pen&            textPen,
                                            Gfx::Font&            font
                                            ) const
{
}

void PlatinumProgressBarRenderer::onRender( const ProgressBar& p,
                                            const StyleOptions& options,
                                            Painter& painter,
                                            const Gfx::RectF& rect,
                                            const Gfx::Brush& background,
                                            const Gfx::Brush& foreground,
                                            const Gfx::Pen& contour,
                                            const Gfx::Pen& textPen,
                                            const Gfx::Font& font
                                         ) const
{
    const Gfx::Scaling& scaling = painter.scaling();

    double barHeight = scaling.align(3.0);
    
    double boxY = p.size().height() / 2 - barHeight / 2;
    boxY = scaling.align(boxY);

    Gfx::PointF barPos(0.0, boxY);
    Gfx::SizeF barSize(p.size().width(), barHeight);
    Gfx::RectF barRect(barPos, barSize);

    Gfx::SizeF progressSize( barSize.width() * p.progress(), barHeight );
    Gfx::RectF progressRect(barPos, progressSize);

    painter.setBrush(background);
    painter.fillRect(barRect);

    painter.setBrush(foreground);
    painter.fillRect(progressRect);

    painter.fillEllipse( Gfx::PointF(progressRect.width() - barHeight / 2, 
                                     progressRect.y()),
                         Gfx::SizeF(progressRect.height(), 
                                    progressRect.height()) );
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


void PlatinumSliderRenderer::onPrepare( const Slider& s,
                                        const StyleOptions& options,
                                        Gfx::Brush& background,
                                        Gfx::Brush& foreground,
                                        Gfx::Pen& contour,
                                        Gfx::Pen& textPen,
                                        Gfx::Font& font) const
{
    bool highlight = s.isHighlighted() && s.isEnabled();

    foreground = highlight ? options.accentColor() 
                           : options.contour().color();
}


void PlatinumSliderRenderer::onRender( const Slider& s,
                                       const StyleOptions& options,
                                       Painter& painter,
                                       const Gfx::RectF& rect,
                                       const Gfx::Brush& background,
                                       const Gfx::Brush& foreground,
                                       const Gfx::Pen& contour,
                                       const Gfx::Pen& textPen,
                                       const Gfx::Font& font) const
{
    double handleWidth = 7.0;
    double handleHeight = 17.0;
    double sliderHeight = 5.0;

    double sliderX = handleWidth / 2;
    double sliderY = s.size().height() / 2 - sliderHeight / 2;
    double sliderWidth = s.size().width() - handleWidth;

    Gfx::RectF boxRect( Gfx::PointF(sliderX, sliderY),
                        Gfx::SizeF(sliderWidth, sliderHeight) );

    painter.setBrush(background);
    painter.fillRect(boxRect);

    int range = s.maximum() - s.minimum();
    int offset = s.position() - s.minimum();
    double handleOffset = sliderWidth * offset / range;

    double handleX = handleOffset;
    double handleY = s.size().height() / 2 - handleHeight / 2;

    Gfx::RectF handleRect( Gfx::PointF(handleX, handleY),
                           Gfx::SizeF(handleWidth, handleHeight) );

    painter.setBrush(foreground);
    painter.fillRect(handleRect);
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


void PlatinumListBoxRenderer::onPrepareLayout(Spacing& frameSize)
{
    frameSize = Spacing(1);
}


void PlatinumListBoxRenderer::onRenderBackground(const ListBox& lb,
                                                 const StyleOptions& options,
                                                 Painter& painter, 
                                                 const Gfx::RectF& rect,
                                                 const Gfx::Brush& brush) const 
{   
    painter.setBrush(brush);
    painter.fillRect(rect);
}


void PlatinumListBoxRenderer::onRenderFrame(const ListBox& lb,
                                            const StyleOptions& options,
                                            Painter& painter, 
                                            const Gfx::RectF& rect,
                                            const Gfx::Pen& pen) const 
{
    const double inset = painter.scaling().alignContour( pen.size() ) / 2;

    Gfx::RectF borderRect = Gfx::RectF( lb.size() );
    borderRect.shift(inset, inset);
    borderRect.shrink(2 * inset, 2 * inset);
    
    painter.setPen(pen);
    painter.drawRect(borderRect);
}


void PlatinumListBoxRenderer::onPrepareItem(const ListBoxItem& item, 
                                            const StyleOptions& options,
                                            Gfx::Brush& brush,
                                            Gfx::Pen& contour,
                                            Gfx::Font& font,
                                            Gfx::Pen& textPen) const
{
    if( item.isHighlighted() || item.isSelected() )
    {
        brush =  options.highlightColor();
        textPen = options.highlightedTextColor();
    }
}


void PlatinumListBoxRenderer::onRenderItem(const ListBoxItem& item, 
                                           const StyleOptions& options,
                                           Painter& painter, 
                                           const Gfx::RectF& rect,
                                           Gfx::Brush& brush,
                                           Gfx::Pen& contour) const
{
    if( item.isHighlighted() || item.isSelected() )
    {
        painter.setBrush(brush);
        painter.fillRect(rect);
    }
}

///////////////////////////////////////////////////////////////////////////////
// PlatinumComboBoxRenderer
///////////////////////////////////////////////////////////////////////////////

PlatinumComboBoxRenderer::PlatinumComboBoxRenderer(std::size_t refs)
: ComboBoxRenderer(refs)
{
}

    
PlatinumComboBoxRenderer::~PlatinumComboBoxRenderer()
{
}


void PlatinumComboBoxRenderer::onPrepare(const ComboBox& cb, 
                                         const StyleOptions& options,
                                         Gfx::Brush& background,
                                         Gfx::Brush& foreground,
                                         Gfx::Pen& contour,
                                         Gfx::Font& font,
                                         Gfx::Pen& textPen) const
{
    if( cb.isEnabled() )
    {
        if( cb.isHighlighted() || cb.hasFocus() )
        {
            contour = Gfx::Pen( options.accentColor(), 
                                contour.size(), contour.style(), 
                                contour.capStyle(), contour.joinStyle() );
        }
    }

    foreground = contour.color();
}


void PlatinumComboBoxRenderer::onRenderBackground(const ComboBox& cb, 
                                                  const StyleOptions& options,
                                                  Painter& painter, 
                                                  const Gfx::RectF& rect,
                                                  const Gfx::Pen& contour,
                                                  const Gfx::Brush& background) const
{
    const double inset = painter.scaling().alignContour( contour.size() ) / 2;

    Gfx::RectF borderRect = Gfx::RectF( cb.size() );
    borderRect.shift(inset, inset);
    borderRect.shrink(2 * inset, 2 * inset);

    painter.setBrush(background);
    painter.fillRect(borderRect);

    painter.setPen(contour);
    painter.drawRect(borderRect);
}


void PlatinumComboBoxRenderer::onPrepareLayout(const ComboBox& cb,
                                               Gfx::SizeF& buttonSize) const
{
    double buttonWidth = cb.size().height();
    double buttonHeight = cb.size().height();
    buttonSize = Gfx::SizeF(buttonWidth, buttonHeight);
}


void PlatinumComboBoxRenderer::onRenderButton(const ComboBox& cb, 
                                              const StyleOptions& options,
                                              Painter& painter, 
                                              const Gfx::RectF& rect,
                                              const Gfx::Pen& contour,
                                              const Gfx::Brush& foreground) const
{
    const Gfx::Scaling& scaling = painter.scaling();

    double pixelWidth = scaling.toLogical(1.0);
    
    double buttonX = cb.size().width() - cb.size().height();
    double buttonWidth = cb.size().height();
    double buttonHeight = cb.size().height();
    
    double gap = scaling.align(5);

    painter.setPen(contour);
    painter.drawLine( Gfx::PointF(buttonX + pixelWidth, gap),
                      Gfx::PointF(buttonX + pixelWidth, cb.size().height() - gap) );

    double triangleWidth = buttonWidth / 3.0;
    triangleWidth = scaling.align(triangleWidth);

    // even number of pixels
    int widthPixels = Pt::lround(triangleWidth / pixelWidth);
    if(widthPixels % 2 != 0)
      triangleWidth += pixelWidth;

    double triangleHeight = triangleWidth / 2.0;
    triangleHeight = scaling.align(triangleHeight);

    double x = (buttonWidth - triangleWidth) / 2;
    x = buttonX + scaling.align(x);
    
    double y = (buttonHeight - triangleHeight) / 2;
    y = scaling.align(y);

    Gfx::PointF triangle[3];
    triangle[0] = Gfx::PointF(x, y);
    triangle[1] = Gfx::PointF(x + triangleWidth, y);
    triangle[2] = Gfx::PointF(x + triangleHeight, y + triangleHeight);

    painter.setBrush(foreground);
    painter.fillPolygon(triangle, 3);
}


void PlatinumComboBoxRenderer::onRenderText(const ComboBox& cb,
                                            const StyleOptions& options,
                                            Painter& painter, 
                                            const Gfx::RectF& rect,
                                            const String& text,
                                            const Gfx::PointF& textPos,
                                            const Gfx::Font& font, 
                                            const Gfx::Pen& textPen,
                                            const Gfx::RectF& cursor) const
{
    //
    // text
    //
    painter.setPen(textPen);
    painter.setFont(font);
    painter.drawText(textPos, text);

    //
    // cursor
    //
    if( cb.isEditable() && cb.hasFocus() )
    {
        painter.drawLine( cursor.topLeft(),
                          cursor.bottomLeft() );
    }
}

///////////////////////////////////////////////////////////////////////////////
// PlatinumSpinBoxRenderer
///////////////////////////////////////////////////////////////////////////////

PlatinumSpinBoxRenderer::PlatinumSpinBoxRenderer(std::size_t refs)
: SpinBoxRenderer(refs)
{
}

    
PlatinumSpinBoxRenderer::~PlatinumSpinBoxRenderer()
{
}


void PlatinumSpinBoxRenderer::onPrepare(const SpinBox& sb, 
                                        const StyleOptions& options,
                                        Gfx::Brush& background,
                                        Gfx::Pen& contour,
                                        Gfx::Font& font,
                                        Gfx::Pen& textPen) const
{
    contour.setJoinStyle(Gfx::Pen::BevelJoin);

    if( sb.isEnabled() )
    {
        if( sb.isHighlighted() || sb.hasFocus() )
        {
            contour = Gfx::Pen( options.accentColor(), 
                                contour.size(), contour.style(), 
                                contour.capStyle(), contour.joinStyle() );
        }
    }
}


void PlatinumSpinBoxRenderer::onPrepareButton(const SpinBoxButton& sb, 
                                              const StyleOptions& options,
                                              Gfx::Brush& foreground,
                                              Gfx::Pen& contour) const
{
    if( sb.isEnabled() )
    {
        if( sb.isHighlighted() )
        {
            contour = Gfx::Pen( options.accentColor(), 
                                contour.size(), contour.style(), 
                                contour.capStyle(), contour.joinStyle() );
        }
    }

    foreground = contour.color();
}


void PlatinumSpinBoxRenderer::onLayout(const SpinBox& sb,
                                       Gfx::RectF& downButton,
                                       Gfx::RectF& upButton,
                                       Gfx::RectF& textBox) const
{   
    double cursorWidth = 5; // TODO: cursor
    double buttonWidth = sb.size().height();

    textBox.setOrigin( Gfx::PointF(buttonWidth + cursorWidth, 0) );

    textBox.setSize( Gfx::SizeF(sb.size().width() - 2 * buttonWidth - 2 * cursorWidth, 
                                sb.size().height()) );

    downButton.setOrigin( Gfx::PointF(0, 0) );
    downButton.setSize( Gfx::SizeF(buttonWidth, buttonWidth) );

    upButton.setOrigin( Gfx::PointF(sb.size().width() - buttonWidth, 0) );
    upButton.setSize( Gfx::SizeF(buttonWidth, buttonWidth) );
}


void PlatinumSpinBoxRenderer::onRenderBackground(const SpinBox& sb, 
                                                 const StyleOptions& options,
                                                 Painter& painter, 
                                                 const Gfx::RectF& rect,
                                                 const Gfx::Pen& contour,
                                                 const Gfx::Brush& background) const
{
    double buttonWidth = sb.size().height();
    double boxWidth = sb.size().width() - 2 * buttonWidth;

    const double inset = painter.scaling().alignContour( contour.size() ) / 2;

    Gfx::RectF boxRect( Gfx::PointF(buttonWidth, 0), 
                        Gfx::SizeF(boxWidth, sb.size().height() ) );

    Gfx::RectF borderRect( Gfx::PointF(buttonWidth, inset), 
                           Gfx::SizeF(boxWidth, sb.size().height() - 2 * inset ) );

    painter.setBrush(background);
    painter.fillRect(boxRect);

    painter.setPen(contour);
    painter.drawRect(borderRect);
}


void PlatinumSpinBoxRenderer::onRenderButton(const SpinBoxButton& sb, 
                                             const StyleOptions& options,
                                             Painter& painter, 
                                             const Gfx::RectF& rect,
                                             const Gfx::Brush& foreground,
                                             const Gfx::Pen& contour) const
{
    const Gfx::Scaling& scaling = painter.scaling();

    double buttonWidth = sb.size().height();
    double buttonHeight = sb.size().height();
    double pixelWidth = scaling.toLogical(1.0);

    double triangleWidth = buttonWidth / 3;
    triangleWidth = scaling.align(triangleWidth);

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

    Gfx::PointF triangle[4];

    if( sb.type() == sb.Down)
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

    painter.setBrush(foreground);
    painter.fillPolygon(triangle, 3);
    
    painter.setPen(contour);
    painter.drawPolyline(triangle, 4);
}


void PlatinumSpinBoxRenderer::onRenderText(const SpinBox& sb,
                                           const StyleOptions& options,
                                           Painter& painter, 
                                           const Gfx::RectF& rect,
                                           const String& text,
                                           const Gfx::PointF& textPos,
                                           const Gfx::Font& font, 
                                           const Gfx::Pen& textPen,
                                           const Gfx::RectF& cursor) const
{
    //
    // text
    //
    painter.setPen(textPen);
    painter.setFont(font);
    painter.drawText(textPos, text);

    //
    // cursor
    //
    if( sb.isEditable() && sb.hasFocus() )
    {
        painter.drawLine( cursor.topLeft(),
                          cursor.bottomLeft() );
    }
}

///////////////////////////////////////////////////////////////////////////////
// PlatinumTabViewRenderer
///////////////////////////////////////////////////////////////////////////////

PlatinumTabViewRenderer::PlatinumTabViewRenderer(std::size_t refs)
: TabViewRenderer(refs)
{
}

    
PlatinumTabViewRenderer::~PlatinumTabViewRenderer()
{
}


void PlatinumTabViewRenderer::onPrepare(const TabView& tv,
                                        const StyleOptions& options,
                                        Gfx::Brush& background,
                                        Gfx::Brush& foreground,
                                        Gfx::Pen& contour) const
{
}


void PlatinumTabViewRenderer::onRender(const TabView& tv,
                                       const StyleOptions& options,
                                       Painter& painter,
                                       const Gfx::RectF& rect,
                                       const Gfx::Brush& background,
                                       const Gfx::Brush& foreground,
                                       const Gfx::Pen& contour) const
{
}


Gfx::SizeF PlatinumTabViewRenderer::onMeasureTabs(PaintSurface& surface,
                                                  const std::vector<TabItem>& tabs,
                                                  const Gfx::Font& font) const
{
    Spacing spacing(font.size() / 2, font.size() / 2 );

    Gfx::SizeF s;
    s.setHeight(font.size() * 2.4);

    Painter _painter(surface);
    _painter.setFont(font);

    std::vector<TabItem>::const_iterator it;
    for(it = tabs.begin(); it != tabs.end(); ++it)
    {
        Gfx::TextMetrics fm = _painter.textMetrics( it->text() );
        s.addWidth( fm.advance() + spacing.leftRight() );
    }

    return s;
}


void PlatinumTabViewRenderer::onLayoutTabs(PaintSurface& surface,
                                           std::vector<TabItem>& tabs,
                                           const Gfx::RectF& rect, 
                                           const Gfx::Font& font) const
{
    Spacing spacing(font.size() / 2, font.size() / 2 );

    Gfx::PointF tabPos;

    Painter _painter(surface);
    _painter.setFont(font);

    std::vector<TabItem>::iterator it;
    for(it = tabs.begin(); it != tabs.end(); ++it)
    {
        Gfx::TextMetrics fm = _painter.textMetrics( it->text() );

        double tabWidth = fm.advance() + spacing.leftRight();
        
        Gfx::RectF tabRect;
        tabRect.setOrigin(tabPos);
        tabRect.setWidth(tabWidth);
        tabRect.setHeight( rect.height() );

        it->setGeometry(tabRect);

        tabPos.addX(tabWidth);
    }
}


void PlatinumTabViewRenderer::onPrepareTabs(const TabBar& tabs,
                                            const StyleOptions& options,
                                            const Gfx::Brush& background,
                                            const Gfx::Brush& foreground,
                                            const Gfx::Pen& contour,
                                            const Gfx::Font& font, 
                                            const Gfx::Pen& textPen) const
{
}


void PlatinumTabViewRenderer::onRenderTabs(const std::vector<TabItem>& tabs,
                                           const StyleOptions& options,
                                           Painter& painter,
                                           const Gfx::RectF& rect,
                                           const Gfx::Brush& background,
                                           const Gfx::Brush& foreground,
                                           const Gfx::Pen& contour,
                                           const Gfx::Font& font, 
                                           const Gfx::Pen& textPen) const
{
    Spacing spacing(font.size() / 2, font.size() / 2 );

    std::vector<TabItem>::const_iterator it;
    for(it = tabs.begin(); it != tabs.end(); ++it)
    {
        if( it->isPressed() )
            painter.setPen( options.accentColor() );
        else
            painter.setPen(textPen);

        painter.setFont(font);

        Gfx::TextMetrics fm = painter.textMetrics( it->text() );
        Gfx::FontMetrics fontMet = painter.fontMetrics();
        
        double textX = it->geometry().left() + spacing.left();
        double textY = it->geometry().height() / 2 + fontMet.ascent() / 2;
        Gfx::PointF textPos(textX, textY);
        painter.drawText( textPos, it->text() );

        if( it->isPressed() )
            painter.setPen( options.accentColor() );
        else
            painter.setPen(contour);

        Gfx::PointF from(it->geometry().left() + spacing.left() / 2, 
                          it->geometry().height() - 1);
        Gfx::PointF to(it->geometry().left() + it->geometry().width() - spacing.right() / 2,
                        it->geometry().height() - 1);
        painter.drawLine(from, to);
    }
}

///////////////////////////////////////////////////////////////////////////////
// PlatinumStyle
///////////////////////////////////////////////////////////////////////////////

PlatinumStyle::PlatinumStyle()
{
    set(new PlatinumButtonRenderer);
    set(new PlatinumCheckBoxRenderer);
    set(new PlatinumPanelRenderer);
    set(new PlatinumLabelRenderer);
    set(new PlatinumLineEditRenderer);
    set(new PlatinumMenuRenderer);
    set(new PlatinumMenuBarRenderer);
    set(new PlatinumScrollBarRenderer);
    set(new PlatinumProgressBarRenderer);
    set(new PlatinumSliderRenderer);
    set(new PlatinumListBoxRenderer);
    set(new PlatinumComboBoxRenderer);
    set(new PlatinumSpinBoxRenderer);
    set(new PlatinumTabViewRenderer);
}


PlatinumStyle::~PlatinumStyle()
{
}

} // namespace

} // namespace
