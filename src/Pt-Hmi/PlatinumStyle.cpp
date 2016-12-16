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
                                       const StyleOptions& options,
                                       const Gfx::Color* color,
                                       double corner) const
{
    Gfx::Color borderColor = color ? *color : options.contourColor();
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
    
    painter.setPen(borderColor);
    painter.drawPolyline(outline, 9);
}


void PlatinumRendererBase::renderFrame(Painter& painter,
                                       const Gfx::RectF& frameRect,
                                       const Gfx::Color& borderColor,
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
    
    painter.setPen(borderColor);
    painter.drawPolyline(outline, 9);
}


void PlatinumRendererBase::renderPlane(Painter& painter,
                                       const Gfx::RectF& rect,
                                       const StyleOptions& options,
                                       const Gfx::Brush* brush,
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

    if(brush)
        painter.setBrush( *brush );
    else
        painter.setBrush( options.background() );

    painter.fillPolygon(outline, 9);
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
    contour = options.contourColor();
    textPen = options.textColor(); 
    font = options.font();

    if( button.isEnabled() )
    {
        Gfx::Color buttonColor = options.foreground().color();

        if( button.isHighlighted() )
        {
            buttonColor = Gfx::Color(buttonColor.red() * 0.9f,
                                     buttonColor.green() * 0.9f,
                                     buttonColor.blue() * 0.9f);
        }

        if( button.isPressed() )
        {
            buttonColor = Gfx::Color(buttonColor.red() * 0.8f,
                                     buttonColor.green() * 0.8f,
                                     buttonColor.blue() * 0.8f);
        }

        foreground = buttonColor;
    }
    else
    {
        foreground = options.foreground();
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
                                                const Gfx::Color& contour) const 
{
    Gfx::RectF borderRect( button.size() );
    _baseRenderer.renderPlane(painter, borderRect, brush);

    if( button.hasFocus() )
    {
        Gfx::SizeF focusSize = button.size();
        focusSize.addHeight(-4);
        focusSize.addWidth(-4);

        Gfx::Pen pen(options.contourColor(), 1, Gfx::Pen::Dash);
        painter.setPen(pen);
        
        Gfx::RectF rect(Gfx::PointF(2,2), focusSize);
        painter.drawRect(rect);
    }

    _baseRenderer.renderFrame(painter, borderRect, contour);
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


void PlatinumCheckBoxRenderer::onRender(const CheckBox& cb, 
                                        PaintSurface& surface, 
                                        const Gfx::RectF& rect) const
{
    const StyleOptions* options = cb.getFacet<StyleOptions>();
    if( options == 0)
      return;

    Gfx::Color foreground = options->contourColor();
    Gfx::Color frameColor = Gfx::Color(foreground.red() * 0.6f,
                                       foreground.green() * 0.6f,
                                       foreground.blue() * 0.6f);

    const Gfx::Font& textFont = options->font();
    const Gfx::Color& textColor = options->textColor();

    const String& text = cb.text();
    const Gfx::SizeF& size = cb.size();

    Painter painter(surface);
    painter.setClip(rect);

    double boxSize = 12;

    //
    // draw the checkbox rectangle
    //
    
    double boxX = boxSize / 2;
    double boxY = (size.height() - boxSize) / 2;
    
    Gfx::RectF boxRect( Gfx::PointF(boxX, boxY), 
                        Gfx::SizeF(boxSize, boxSize) );

    painter.setBrush( Gfx::Color::fromRgb8(255, 255, 255) );
    painter.fillRect(boxRect);

    painter.setPen(frameColor);
    painter.drawRect(boxRect);

    if( cb.isChecked() )
    {
        Gfx::PointF tl = boxRect.topLeft() + Gfx::PointF(2, 2);
        Gfx::PointF br = boxRect.bottomRight() - Gfx::PointF(2, 2);
        Gfx::PointF tr = boxRect.topRight() + Gfx::PointF(-2, 2);
        Gfx::PointF bl = boxRect.bottomLeft() - Gfx::PointF(-2, 2);

        Pt::Gfx::Pen pen(textColor, 2, Gfx::Pen::Solid, Gfx::Pen::RoundCap);
        painter.setPen(pen);
        painter.drawLine(tl, br);
        painter.drawLine(tr, bl);
    }

    //
    // draw the checkbox text
    //
    
    painter.setFont(textFont);
    Gfx::FontMetrics metric = painter.fontMetrics( cb.text() );

    double textX = 2 * boxSize;
    double textY = (size.height() / 2) - (metric.height() / 2) + metric.ascent();
    Gfx::PointF textPos(textX, textY);

    _baseRenderer.renderItemText(painter, textPos, text, cb.mnemonic(),
                                 textFont, textColor);

    //
    // draw the focus rect
    //
    
    if( cb.hasFocus() )
    {       
        double focusX = textX - 2;
        double focusY = textY - metric.ascent();
        Gfx::RectF focusRect( Gfx::PointF(focusX, focusY), 
                              Gfx::SizeF(metric.width() + 4, metric.height() + 1) );
        
        Gfx::Pen pen(frameColor, 1, Gfx::Pen::Dash);
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
                                          const Gfx::Color& borderColor) const 
{
    Gfx::RectF borderRect( Gfx::PointF(0,0), p.size() );
    
    _baseRenderer.renderFrame(painter, borderRect, borderColor);
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
                                          const Gfx::Color& borderColor) const 
{
    Gfx::RectF borderRect( Gfx::PointF(0,0), l.size() );
    
    _baseRenderer.renderFrame(painter, borderRect, borderColor);
}


void PlatinumLabelRenderer::onRenderText(const Label& l,
                                         const StyleOptions& options,
                                         Painter& painter, 
                                         const Gfx::RectF& rect,
                                         const String& text,
                                         const Gfx::PointF& textPos,
                                         const Gfx::Font& font, 
                                         const Gfx::Color& textColor) const 
{
    painter.setFont(font);
    painter.setPen(textColor);
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


void PlatinumLineEditRenderer::onRenderItem(const LineEdit& le, 
                                            const StyleOptions& options,
                                            Painter& painter, 
                                            const Gfx::RectF& rect,
                                            const Gfx::Color& contourColor,
                                            const Gfx::Brush& background) const
{
    painter.setBrush(background);
    painter.fillRect(rect);

    painter.setPen(contourColor);
    painter.drawRect(rect);
}


void PlatinumLineEditRenderer::onRenderText(const LineEdit& le, 
                                            const StyleOptions& options,
                                            Painter& painter, 
                                            const Gfx::RectF& rect,
                                            const String& text,
                                            const Gfx::PointF& textPos,
                                            const Gfx::Font& font,
                                            const Gfx::Color& textColor) const
{
    painter.setPen(textColor);
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


void PlatinumMenuRenderer::onRender(const Menu& m, 
                                    PaintSurface& surface, 
                                    const Gfx::RectF& rect) const
{
    const StyleOptions* options = m.getFacet<StyleOptions>();
    if( ! options)
      return;

    const Gfx::SizeF& size = m.size();

    Painter painter( surface );
    painter.setClip(rect);

    //
    // icon strip on the left side
    //
    
    // TODO: use separate render funtion for iconstrip

    Pt::ssize_t iconWidth = m.iconWidth();

    if(iconWidth > 0)
    {
        Gfx::RectF iconStrip( Gfx::PointF(0, 0),
                              Gfx::SizeF(iconWidth, size.height()) );
        
        // only the damaged region
        //iconStrip = iconStrip.intersect(rect);
        //Gfx::Brush brush = Pt::Gfx::Color(0.95f, 0.95f, 0.95f);

        // TODO: need painter clipping for gradient
        
         Gfx::Brush brush(Gfx::Color(65535* 0.90f, 65535*0.90f, 65535*0.91f),
                          Gfx::Color(65535*0.99f, 65535*0.99f, 65535*0.99f), 
                          Gfx::Brush::Vertical);

        painter.setBrush(brush);
        painter.fillRect(iconStrip);
    }

    //
    // menu border
    //
    Gfx::RectF borderRect(size);

    Gfx::Pen pen(Gfx::Color(65535*0.5f, 65535*0.5f, 65535*0.51f), 1 );
    painter.setPen(pen);
    painter.drawRect(borderRect);
}


void PlatinumMenuRenderer::onRenderItem(const MenuItem& m, 
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
