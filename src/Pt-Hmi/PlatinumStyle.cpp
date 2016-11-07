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
#include <Pt/Hmi/PushButton.h>
#include <Pt/Hmi/Painter.h>
#include <Pt/Hmi/PaintSurface.h>

namespace Pt {

namespace Hmi {

///////////////////////////////////////////////////////////////////////////////
// PlatinumButtonRenderer
///////////////////////////////////////////////////////////////////////////////

PlatinumButtonRenderer::PlatinumButtonRenderer()
{
}

    
PlatinumButtonRenderer::~PlatinumButtonRenderer()
{
}


void PlatinumButtonRenderer::onRenderBackground(const PushButton& button, 
                                                const StyleOptions& options,
                                                PaintSurface& surface, 
                                                const Gfx::RectF& rect) const
{
    Gfx::Color buttonColor = options.foreground();
    Gfx::Color frameColor = Gfx::Color(buttonColor.red() * 0.7f,
                                       buttonColor.green() * 0.7f,
                                       buttonColor.blue() * 0.7f);

    const Gfx::SizeF& size = button.size();

    if( size.width() < 0 || size.height() < 0)
        return;

    if( button.isEnabled() )
    {
        if(button.isHovered())
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
}


void PlatinumButtonRenderer::onRenderContent(const PushButton& button, 
                                             const StyleOptions& options,
                                             PaintSurface& surface, 
                                             const Gfx::RectF& rect) const
{
    Gfx::Color buttonColor = options.foreground();
    Gfx::Color frameColor = Gfx::Color(buttonColor.red() * 0.6f,
                                       buttonColor.green() * 0.6f,
                                       buttonColor.blue() * 0.6f);

    const Gfx::Font& textFont = options.font();
    Gfx::Color textColor = options.textColor();

    const String& text = button.text();
    const Gfx::SizeF& size = button.size();

    Painter painter(surface);
    painter.setClip(rect);
    painter.setPen(textColor);
    painter.setFont(textFont);
    painter.setCompositionMode(Gfx::CompositionMode::SourceCopy);

    Gfx::FontMetrics metric = painter.fontMetrics( button.text() );

    const double widthHalf      = size.width() / 2;
    const double heightHalf     = size.height() / 2;
    const double textWidthHalf  = metric.width() / 2;
    const double textHeightHalf = metric.height() / 2;

    Gfx::PointF textPos = Gfx::PointF(widthHalf - textWidthHalf, 
                                      heightHalf - textHeightHalf + metric.ascent());

    painter.drawText(textPos, text);

    const Char* ch = button.mnemonic();
    if(ch)
    {
        String::size_type n = text.find(*ch);

        if(n != String::npos)
        {
            Pt::String mnemonicText(text, 0, n);
            Gfx::FontMetrics fm = painter.fontMetrics(mnemonicText);
            Gfx::PointF from(textPos.x() + fm.width(), textPos.y() + 1);

            mnemonicText = *ch;
            fm = painter.fontMetrics(mnemonicText);
            Gfx::PointF to( from.x() + fm.width(), from.y() );

            painter.drawLine(from, to);
        }
    }

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
// PlatinumStyle
///////////////////////////////////////////////////////////////////////////////

PlatinumStyle::PlatinumStyle()
{
    set(new PlatinumButtonRenderer);
}


PlatinumStyle::~PlatinumStyle()
{
}

} // namespace

} // namespace
