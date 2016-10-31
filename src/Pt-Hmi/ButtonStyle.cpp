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

#include <Pt/Hmi/ButtonStyle.h>
#include <Pt/Hmi/Button.h>
#include <Pt/Hmi/Painter.h>
#include <Pt/Hmi/PaintSurface.h>
#include <Pt/Hmi/Application.h>

namespace Pt {

namespace Hmi {

ButtonStyle::ButtonStyle()
: WidgetStyle( typeid(ButtonStyle) )
{
}

    
ButtonStyle::~ButtonStyle()
{
}


PtButtonStyle::PtButtonStyle()
{
}

    
PtButtonStyle::~PtButtonStyle()
{
}


void PtButtonStyle::onRenderBackground(Button& button, 
                                       PaintSurface& surface, 
                                       const Gfx::RectF& updateRect) const
{
    const Pt::Gfx::Pen& fg = button.foreground().isNull() ? foreground()
                                                          : button.foreground();

    const Pt::Gfx::Brush& bg = button.background().isNull() ? background()
                                                            : button.background();

    Gfx::Color bkgColor = bg.color();

    Gfx::Color frameColor = Gfx::Color(bkgColor.red() * 0.6f ,
                                       bkgColor.green() * 0.6f ,
                                       bkgColor.blue() * 0.6f);

    const Gfx::SizeF& size = button.size();

    if( size.width() < 0 || size.height() < 0)
        return;

    if( button.isEnabled() )
    {
        // TODO: use enter/leave events
        bool mouseOver = Application::instance().pointerWidget() == &button;
        if(mouseOver)
        {
            bkgColor = Gfx::Color(bkgColor.red() * 0.9f ,
                                  bkgColor.green() * 0.9f ,
                                  bkgColor.blue() * 0.9f);
        }

        if( button.isPressed() )
        {
            bkgColor = Gfx::Color(bkgColor.red() * 0.8f ,
                                  bkgColor.green() * 0.8f ,
                                  bkgColor.blue() * 0.8f);
        }
    }

    Painter painter(surface);
    painter.setClip(updateRect);
    painter.setPen(fg);
    painter.setCompositionMode(Gfx::CompositionMode::SourceCopy);

    Gfx::RectF borderRect(Gfx::PointF(0,0), size);

    borderRect.setOrigin( Gfx::PointF(1, 1) );
    borderRect.setSize( Gfx::SizeF(size.width() - 1, 
                                    size.height() - 1) );

    double corner = 2.0;
    std::vector<Gfx::PointF> outline(9);

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

    if( bg.color().alpha() != 0 )
    {
        painter.setBrush(bkgColor); 
        painter.fillPolygon(&outline[0], outline.size());
    }

    painter.setPen( Gfx::Pen(frameColor) );
    painter.drawPolyline(&outline[0], outline.size());
}


void PtButtonStyle::onRenderContent(Button& button, 
                                    PaintSurface& surface, 
                                    const Gfx::RectF& updateRect) const
{
    const Pt::Gfx::Font& textFont = button.font().isNull() ? font() : button.font();
    const Pt::Gfx::Pen& fg = button.foreground().isNull() ? foreground()
                                                          : button.foreground();

    const Pt::Gfx::Brush& bg = button.background().isNull() ? background()
                                                            : button.background();

    const String& text = button.text();
    const Gfx::SizeF& size = button.size();

    Painter painter(surface);
    painter.setClip(updateRect);
    painter.setPen(fg);
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

        Gfx::Color armedColor( Gfx::Color::fromRgb8(176,176,176) );
        Gfx::Pen pen(armedColor, 1, Gfx::Pen::Dash);
        painter.setPen(pen);
        
        Gfx::RectF rect(Gfx::PointF(2,2), focusSize);
        painter.drawRect(rect);
    }
}

} // namespace

} // namespace
