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

#include <Pt/Hmi/PlatinumStyle.h>
#include <Pt/Hmi/StyleOptions.h>
#include <Pt/Hmi/PaintSurface.h>
#include <Pt/Hmi/Painter.h>
#include <Pt/Hmi/Picture.h>
#include <Pt/Hmi/Panel.h>
#include <Pt/Hmi/Label.h>
#include <Pt/Hmi/LineEdit.h>
#include <Pt/Hmi/PushButton.h>
#include <Pt/Hmi/CheckBox.h>
#include <Pt/Hmi/MenuBar.h>
#include <Pt/Hmi/Menu.h>
#include <Pt/Hmi/MenuItem.h>
#include <Pt/Hmi/ScrollBar.h>

namespace {

Pt::Gfx::Color brighten(const Pt::Gfx::Color& c, float factor)
{
    float r = c.red() * factor;
    float g = c.green() * factor;
    float b = c.blue() * factor;

    Pt::uint16_t r16 = r > 65535 ? 65535 : static_cast<Pt::uint16_t>(r);
    Pt::uint16_t g16 = g > 65535 ? 65535 : static_cast<Pt::uint16_t>(g);
    Pt::uint16_t b16 = b > 65535 ? 65535 : static_cast<Pt::uint16_t>(b);

    return Pt::Gfx::Color(c.alpha(), r16, g16, b16);
}

} // namespace

namespace Pt {

namespace Hmi {

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
                                       const Gfx::RectF& frameRect,
                                       const Gfx::Pen& pen,
                                       double corner) const
{
    Gfx::RectF borderRect = frameRect;

    borderRect.setOrigin( Gfx::PointF(corner, corner) );
    borderRect.setSize( Gfx::SizeF(frameRect.size().width() - corner, 
                                   frameRect.size().height() - corner) );

    Gfx::PointF outline[9] = {};

    // top left
    outline[0].setX(0);
    outline[0].setY(corner);

    outline[1].setX(corner);
    outline[1].setY(0);

    // top right
    outline[2].setX(borderRect.width() - corner);
    outline[2].setY(0);

    outline[3].setX(borderRect.width());
    outline[3].setY(corner);

    // bottom right
    outline[4].setX(borderRect.width());
    outline[4].setY(borderRect.height() - corner);

    outline[5].setX(borderRect.width() - corner);
    outline[5].setY(borderRect.height());

    // bottom left
    outline[6].setX(corner);
    outline[6].setY(borderRect.height());

    outline[7].setX(0);
    outline[7].setY(borderRect.height() - corner);
            
    outline[8] = outline[0];
    
    painter.setPen(pen);
    painter.drawPolyline(outline, 9);
}


void PlatinumRendererBase::renderPlane(Painter& painter,
                                       const Gfx::RectF& rect,
                                       const Gfx::Brush& brush,
                                       double corner) const
{
    Gfx::RectF borderRect( Gfx::PointF(corner, corner),
                           Gfx::SizeF(rect.size().width() - corner, 
                                      rect.size().height() - corner) );

    Gfx::PointF outline[9] = {};

    // top left
    outline[0].setX(0);
    outline[0].setY(corner);

    outline[1].setX(corner);
    outline[1].setY(0);

    // top right
    outline[2].setX(borderRect.width() - corner);
    outline[2].setY(0);

    outline[3].setX(borderRect.width());
    outline[3].setY(corner);

    // bottom right
    outline[4].setX(borderRect.width());
    outline[4].setY(borderRect.height() - corner);

    outline[5].setX(borderRect.width() - corner);
    outline[5].setY(borderRect.height());

    // bottom left
    outline[6].setX(corner);
    outline[6].setY(borderRect.height());

    outline[7].setX(0);
    outline[7].setY(borderRect.height() - corner);
            
    outline[8] = outline[0];

    painter.setBrush( brush );
    painter.fillPolygon(outline, 9);
}


void PlatinumRendererBase::renderItemText(Painter& painter,
                                          const Gfx::PointF& textPos,
                                          const String& text,
                                          const Char* mnemonic,
                                          const Gfx::Font& font,
                                          const Gfx::Color& color) const
{
    painter.setPen(color);
    painter.setFont(font);

    painter.drawText(textPos, text);

    if(mnemonic)
    {
        String::size_type n = text.find(*mnemonic);

        if(n != String::npos)
        {
            Pt::String mnemonicText(text, 0, n);
            Gfx::FontMetrics fm = painter.fontMetrics(mnemonicText);
            Gfx::PointF from(textPos.x() + fm.width(), textPos.y() + 1);

            mnemonicText = *mnemonic;
            fm = painter.fontMetrics(mnemonicText);
            Gfx::PointF to( from.x() + fm.width() - 1, from.y() );

            painter.drawLine(from, to);
        }
    }
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


void PlatinumButtonRenderer::onPrepare(const PushButton& button,
                                       const StyleOptions& options,
                                       Gfx::Brush& foreground,
                                       Gfx::Pen& contour,
                                       Gfx::Font& font,
                                       Gfx::Pen& textPen) const 
{
    if( button.isEnabled() )
    {
        Gfx::Color buttonColor = foreground.color();

        if( button.isHighlighted() )
        {
            buttonColor = brighten(buttonColor, 0.9);
        }

        if( button.isPressed() )
        {
            buttonColor = brighten(buttonColor, 0.9);
        }

        foreground = buttonColor;
    }
}


void PlatinumButtonRenderer::onPrepareIcon(const PushButton& button,
                                           const StyleOptions& options,
                                           const Gfx::Image& icon,
                                           Picture& picture) const
{
    picture.set(icon);
}


void PlatinumButtonRenderer::onRenderBackground(const PushButton& button,
                                                const StyleOptions& options,
                                                Painter& painter, 
                                                const Gfx::RectF& rect,
                                                const Gfx::Brush& brush,
                                                const Gfx::Pen& pen) const 
{
    Gfx::RectF borderRect( button.size() );
    _baseRenderer.renderPlane(painter, borderRect, brush);

    if( button.hasFocus() )
    {
        Gfx::SizeF focusSize = button.size();
        focusSize.addHeight(-4);
        focusSize.addWidth(-4);

        Gfx::Pen focusPen(pen.color(), 1, Gfx::Pen::Dash);
        painter.setPen(focusPen);
        
        Gfx::RectF rect(Gfx::PointF(2,2), focusSize);
        painter.drawRect(rect);
    }

    _baseRenderer.renderFrame(painter, borderRect, pen);
}


void PlatinumButtonRenderer::onRenderText(const PushButton& button,
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
// PlatinumButtonRenderer
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
    boxSize.set( font.size(), font.size() );
}


void PlatinumCheckBoxRenderer::onRenderBox(const CheckBox& cb,
                                           const StyleOptions& options,
                                           Painter& painter, 
                                           const Gfx::RectF& rect,
                                           const Gfx::RectF& boxRect,
                                           const Gfx::Brush& brush,
                                           const Gfx::Pen& pen) const
{
    Gfx::Color checkColor = options.textColor();

    painter.setBrush(brush);
    painter.fillRect(boxRect);

    painter.setPen(pen);
    painter.drawRect(boxRect);

    if( cb.isChecked() )
    {
        Gfx::PointF tl = boxRect.topLeft() + Gfx::PointF(2, 2);
        Gfx::PointF br = boxRect.bottomRight() - Gfx::PointF(2, 2);
        Gfx::PointF tr = boxRect.topRight() + Gfx::PointF(-2, 2);
        Gfx::PointF bl = boxRect.bottomLeft() - Gfx::PointF(-2, 2);

        Pt::Gfx::Pen pen(checkColor, 2, Gfx::Pen::Solid, Gfx::Pen::RoundCap);
        painter.setPen(pen);
        painter.drawLine(tl, br);
        painter.drawLine(tr, bl);
    }
}


void PlatinumCheckBoxRenderer::onRenderText(const CheckBox& cb,
                                            const StyleOptions& options,
                                            Painter& painter, 
                                            const Gfx::RectF& rect,
                                            const String& text,
                                            const Gfx::PointF& textPos,
                                            const Gfx::FontMetrics& textMetric,
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
        Gfx::RectF focusRect( Gfx::PointF(textPos.x() - 2, 
                                          textPos.y() - textMetric.ascent()), 
                              Gfx::SizeF(textMetric.width() + 4, 
                                         textMetric.height() ) );
        
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
    Gfx::RectF borderRect( Gfx::PointF(0,0), p.size() );
    
    _baseRenderer.renderPlane(painter, borderRect, brush);
}


void PlatinumPanelRenderer::onRenderFrame(const Panel& p,
                                          const StyleOptions& options,
                                          Painter& painter, 
                                          const Gfx::RectF& rect,
                                          const Gfx::Pen& pen) const 
{
    Gfx::RectF borderRect( Gfx::PointF(0,0), p.size() );
    
    _baseRenderer.renderFrame(painter, borderRect, pen);
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


void PlatinumLabelRenderer::onPrepare(const Label& l,
                                      const StyleOptions& options,
                                      Gfx::Font& font,
                                      Gfx::Pen& textPen) const 
{
    textPen = options.textColor();
}


void PlatinumLabelRenderer::onRenderBackground(const Label& l,
                                               const StyleOptions& options,
                                               Painter& painter, 
                                               const Gfx::RectF& rect,
                                               const Gfx::Brush& brush) const 
{
    Gfx::RectF borderRect( Gfx::PointF(0,0), l.size() );
    
    _baseRenderer.renderPlane(painter, borderRect, brush);
}


void PlatinumLabelRenderer::onRenderFrame(const Label& l,
                                          const StyleOptions& options,
                                          Painter& painter, 
                                          const Gfx::RectF& rect,
                                          const Gfx::Pen& contour) const 
{
    Gfx::RectF borderRect( Gfx::PointF(0,0), l.size() );
    
    _baseRenderer.renderFrame(painter, borderRect, contour);
}


void PlatinumLabelRenderer::onRenderText(const Label& l,
                                         const StyleOptions& options,
                                         Painter& painter, 
                                         const Gfx::RectF& rect,
                                         const String& text,
                                         const Gfx::PointF& textPos,
                                         const Gfx::Font& font, 
                                         const Gfx::Pen& textPen) const 
{
    painter.setFont(font);
    painter.setPen(textPen);
    painter.drawText( textPos, text);
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
                                         Gfx::Pen& textPen,
                                         Gfx::Pen& placeholderPen) const
{
}


void PlatinumLineEditRenderer::onRenderItem(const LineEdit& le, 
                                            const StyleOptions& options,
                                            Painter& painter, 
                                            const Gfx::RectF& rect,
                                            const Gfx::Pen& contour,
                                            const Gfx::Brush& brush) const
{
    painter.setBrush(brush);
    painter.fillRect(rect);

    painter.setPen(contour);
    painter.drawRect(rect);
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
    brush = options.background();
    contour = options.contourColor();
}


void PlatinumMenuRenderer::onRenderBackground(const Menu& m, 
                                              const StyleOptions& options,
                                              Painter& painter, 
                                              const Gfx::RectF& rect,
                                              const Gfx::Pen& contour,
                                              const Gfx::Brush& brush) const
{
    const Gfx::SizeF& size = m.size();

    //
    // icon strip on the left side
    //
    
    // TODO: use separate render funtion for iconstrip

    Pt::ssize_t iconWidth = m.iconWidth();

    if(iconWidth > 0)
    {
        Gfx::RectF iconStrip( Gfx::PointF(0, 0),
                              Gfx::SizeF(iconWidth, size.height()) );
                
         Gfx::Brush brush(brush.color(),
                          Gfx::Color(65000, 65000, 65000), 
                          Gfx::Brush::Vertical);

        painter.setBrush(brush);
        painter.fillRect(iconStrip);
    }

    //
    // menu border
    //
    Gfx::RectF borderRect(size);

    painter.setPen(contour);
    painter.drawRect(borderRect);
}


void PlatinumMenuRenderer::onPrepareItem(const MenuItem& m, 
                                         const StyleOptions& options,
                                         const Gfx::Image& icon,
                                         Picture& picture,
                                         Gfx::Brush& brush,
                                         Gfx::Pen& contour,
                                         Gfx::Font& font,
                                         Gfx::Pen& textPen) const
{
    picture.set(icon);
    brush = m.isHighlighted() ? options.highlightColor() 
                              : options.background();
    contour = options.contourColor();
    font = options.font();
    textPen = options.textColor();
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
                                             PaintSurface& surface, 
                                             const Gfx::RectF& rect) const
{
    static const double indicatorWidth = 5.0;

    const StyleOptions* options = m.getFacet<StyleOptions>();
    if( ! options)
      return;

    Painter painter(surface);
    painter.setClip(rect);

    double x = m.size().width() - indicatorWidth - m.padding().right();
    double y = m.size().height() / 2;

    Gfx::PointF indicator[3] = { Gfx::PointF(x - 3, y - 4),
                                 Gfx::PointF(x + 1, y),
                                 Gfx::PointF(x - 3, y + 4) };
  
    Gfx::Brush brush( options->textColor() );
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


void PlatinumMenuBarRenderer::onRender(const MenuBar& m, 
                                    PaintSurface& surface, 
                                    const Gfx::RectF& rect) const
{
    const StyleOptions* options = m.getFacet<StyleOptions>();
    if( ! options)
      return;

    Painter painter(surface);
    painter.setBrush( options->background() );
    painter.fillRect(rect);
}


void PlatinumMenuBarRenderer::onRenderItem(const MenuBarItem& m, 
                                           PaintSurface& surface, 
                                           const Gfx::RectF& rect) const
{
    const StyleOptions* options = m.getFacet<StyleOptions>();
    if( ! options)
      return;

    bool highlight = m.isHighlighted();
    if(highlight)
    {
        Gfx::Color bgColor = options->highlightColor();
        Gfx::Brush brush = brighten(bgColor, 0.85f);

        Painter painter(surface);
        painter.setClip(rect);
        painter.setBrush(brush);
        painter.fillRect( Gfx::RectF(Gfx::PointF(0,0), m.size()) );
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


void PlatinumScrollBarRenderer::onRender(const ScrollBar& s,
                                         const Gfx::RectF& handleRect,
                                         PaintSurface& surface, 
                                         const Gfx::RectF& rect) const
{
    const StyleOptions* options = s.getFacet<StyleOptions>();
    if( ! options)
      return;

    Painter painter(surface);
    painter.setClip(rect);

    painter.setBrush( options->foreground() );
    painter.fillRect(rect);

    painter.setPen( options->contourColor() );
    painter.drawRect( Gfx::RectF( s.size() ) );

    painter.setBrush( options->foreground() );
    painter.fillRect(handleRect);

    Gfx::Color handleFrameColor = options->contourColor();
    painter.setPen(handleFrameColor);
    painter.drawRect(handleRect);
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
}


PlatinumStyle::~PlatinumStyle()
{
}

} // namespace

} // namespace
