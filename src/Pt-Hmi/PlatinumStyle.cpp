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
#include <Pt/Hmi/Frame.h>
#include <Pt/Hmi/Panel.h>
#include <Pt/Hmi/Label.h>
#include <Pt/Hmi/PushButton.h>
#include <Pt/Hmi/CheckBox.h>
#include <Pt/Hmi/Painter.h>
#include <Pt/Hmi/PaintSurface.h>

namespace Pt {

namespace Hmi {

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


void PlatinumButtonRenderer::renderButtonText(Painter& painter, const Gfx::PointF& textPos,
                                              const String& text, const Char* mnemonic)
{
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


void PlatinumButtonRenderer::onRender(const Button& button, 
                                      PaintSurface& surface, 
                                      const Gfx::RectF& rect) const
{
    const StyleOptions* options = button.getFacet<StyleOptions>();

    if( options == 0)
      return;

    Gfx::Color buttonColor = options->foreground();

    Gfx::Color frameColor = Gfx::Color(buttonColor.red() * 0.7f,
                                       buttonColor.green() * 0.7f,
                                       buttonColor.blue() * 0.7f);

    Gfx::Color textColor =  options->textColor();

    Gfx::Color foreground = options->foreground();

    const Gfx::Font& textFont = options->font();
    
    const String& text = button.text();
    
    const Gfx::SizeF& size = button.size();

    if( size.width() < 0 || size.height() < 0)
        return;

    //
    // draw the button area
    //

    if( button.isEnabled() )
    {
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
    }

    Painter painter(surface);
    painter.setClip(rect);
    painter.setCompositionMode(Gfx::CompositionMode::SourceCopy);

    Gfx::RectF borderRect(Gfx::PointF(0,0), size);

    borderRect.setOrigin( Gfx::PointF(1, 1) );
    borderRect.setSize( Gfx::SizeF(size.width() - 1, 
                                   size.height() - 1) );

    double corner = 1.0;
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

    painter.setBrush(buttonColor); 
    painter.fillPolygon(outline, 9);

    painter.setPen(frameColor);
    painter.drawPolyline(outline, 9);

    //
    // draw the button text
    //

    Gfx::FontMetrics metric = painter.fontMetrics( button.text() );

    double textX = size.width() / 2 - metric.width() / 2;
    double textY = (size.height() / 2) - (metric.height() / 2) + metric.ascent();
    Gfx::PointF textPos(textX, textY);

    painter.setPen(textColor);
    painter.setFont(textFont);
    renderButtonText(painter, textPos, text, button.mnemonic());

    //
    // draw the focus rect
    //

    if( button.hasFocus() )
    {
        Gfx::SizeF focusSize = size;
        focusSize.addHeight(-4);
        focusSize.addWidth(-4);

        Gfx::Pen pen(frameColor, 1, Gfx::Pen::Dash);
        painter.setPen(pen);
        
        Gfx::RectF rect(Gfx::PointF(2,2), focusSize);
        painter.drawRect(rect);
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

    Gfx::Color foreground = options->foreground();
    Gfx::Color frameColor = Gfx::Color(foreground.red() * 0.6f,
                                       foreground.green() * 0.6f,
                                       foreground.blue() * 0.6f);

    const Gfx::Font& textFont = options->font();
    const Gfx::Color& textColor = options->textColor();

    const String& text = cb.text();
    const Gfx::SizeF& size = cb.size();

    Painter painter(surface);
    painter.setClip(rect);
    painter.setCompositionMode(Gfx::CompositionMode::SourceCopy);

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

    Gfx::FontMetrics metric = painter.fontMetrics( cb.text() );

    double textX = 2 * boxSize;
    double textY = (size.height() / 2) - (metric.height() / 2) + metric.ascent();
    Gfx::PointF textPos(textX, textY);

    painter.setPen(textColor);
    painter.setFont(textFont);
    PlatinumButtonRenderer::renderButtonText(painter, textPos, text, cb.mnemonic());

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
// PlatinumFrameRenderer
///////////////////////////////////////////////////////////////////////////////

PlatinumFrameRenderer::PlatinumFrameRenderer(std::size_t refs)
: FrameRenderer(refs)
{
}

    
PlatinumFrameRenderer::~PlatinumFrameRenderer()
{
}


void PlatinumFrameRenderer::onRender(const Frame& f, 
                                     PaintSurface& surface, 
                                     const Gfx::RectF& rect) const
{
    const StyleOptions* options = f.getFacet<StyleOptions>();
    if( ! options)
      return;

    Gfx::Color frameColor = f.borderColor() ? *f.borderColor()     
                                            : options->foreground();

    Painter painter(surface);
    painter.setClip(rect);
    painter.setCompositionMode(Gfx::CompositionMode::SourceCopy);

    Gfx::RectF frameRect( Gfx::PointF(0, 0), f.size() );

    painter.setPen(frameColor);
    painter.drawRect(frameRect);
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


void PlatinumPanelRenderer::onRender(const Panel& p, 
                                     PaintSurface& surface, 
                                     const Gfx::RectF& rect) const
{
    const StyleOptions* options = p.getFacet<StyleOptions>();
    if( ! options )
      return;

    Painter painter(surface);
    painter.setClip(rect);
    painter.setCompositionMode(Gfx::CompositionMode::SourceCopy);

    const Gfx::SizeF& size = p.size();
    Gfx::RectF borderRect(Gfx::PointF(0,0), size);

    borderRect.setOrigin( Gfx::PointF(1, 1) );
    borderRect.setSize( Gfx::SizeF(size.width() - 1, 
                                   size.height() - 1) );

    double corner = 1.0;
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

    const Gfx::Color* color = p.planeColor();
    if(color)
        painter.setBrush(*color);
    else
        painter.setBrush( options->background() ); 
    
    painter.fillPolygon(outline, 9);
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


void PlatinumLabelRenderer::onRender(const Label& l, 
                                     PaintSurface& surface, 
                                     const Gfx::RectF& updateRect) const
{
    const StyleOptions* options = l.getFacet<StyleOptions>();
    if( ! options )
      return;

    Painter painter(surface);
    painter.setClip(updateRect);

    const Gfx::SizeF& size = l.size();
    Gfx::PointF pos(0, 0);
    Gfx::FontMetrics metric = painter.fontMetrics( l.text() );

    switch( l.textAlignment() )
    {
        case Label::TopLeft:
        {
            pos = Gfx::PointF(0, metric.ascent());
            break;
        }
        
        case Label::TopCenter:
        {
            const double widthHalf     = size.width() / 2;
            const double textWidthHalf = metric.width() / 2;
            pos = Gfx::PointF(widthHalf - textWidthHalf, metric.ascent());
            break;
        }
        break;

        case Label::TopRight:
        {
            const double width     = size.width();
            const double textWidth = metric.width();
            pos = Gfx::PointF(width - textWidth, metric.ascent());
            break;
        }
        break;

        case Label::MiddleLeft:
        {
            const double heightHalf     = size.height() / 2;
            const double textHeightHalf = metric.height() / 2;

            pos = Gfx::PointF(0, (heightHalf - textHeightHalf) + metric.ascent());
            break;
        }

        default:
        case Label::MiddleCenter:
        {            
            const double widthHalf      = size.width() / 2;
            const double heightHalf     = size.height() / 2;
            const double textWidthHalf  = metric.width() / 2;
            const double textHeightHalf = metric.height() / 2;

            pos = Gfx::PointF(widthHalf - textWidthHalf, 
                              heightHalf - textHeightHalf + metric.ascent());
            break;
        }

        case Label::MiddleRight:
        {
            const double width          = size.width();
            const double textWidth      = metric.width();
            const double heightHalf     = size.height()/2;
            const double textHeightHalf = metric.height()/2;

            pos = Gfx::PointF(width - textWidth, 
                              heightHalf - textHeightHalf + metric.ascent());
            break;
        }

        case Label::BottomLeft:
        {
            const double height     = size.height();
            const double textHeight = metric.height();

            pos = Gfx::PointF(0, height- textHeight + metric.ascent());
            break;
        }

        case Label::BottomCenter:
        {
            const double widthHalf     = size.width() / 2;
            const double textWidthHalf = metric.width() / 2;
            const double height        = size.height();
            const double textHeight    = metric.height();

            pos = Gfx::PointF(widthHalf - textWidthHalf, 
                              height - textHeight + metric.ascent());
            break;
        }

        case Label::BottomRight:
        {
            const double width      = size.width();
            const double textWidth  = metric.width();
            const double height     = size.height();
            const double textHeight = metric.height();

            pos = Gfx::PointF(width - textWidth, 
                              height- textHeight + metric.ascent());
            break;
        }
    }

    const Gfx::Color* color = l.textColor();
    if(color)
        painter.setPen(*color);
    else
        painter.setPen( options->textColor() ); 

    const Gfx::Font* font = l.font();
    if(font)
        painter.setFont(*font);
    else
        painter.setFont( options->font() );

    painter.drawText( pos, l.text() );
}

///////////////////////////////////////////////////////////////////////////////
// PlatinumStyle
///////////////////////////////////////////////////////////////////////////////

PlatinumStyle::PlatinumStyle()
{
    set(new PlatinumButtonRenderer);
    set(new PlatinumCheckBoxRenderer);
    set(new PlatinumFrameRenderer);
    set(new PlatinumPanelRenderer);
    set(new PlatinumLabelRenderer);
}


PlatinumStyle::~PlatinumStyle()
{
}

} // namespace

} // namespace
