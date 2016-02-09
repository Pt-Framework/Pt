/* Copyright (C) 2015 Marc Boris Duerner 
   Copyright (C) 2015 Laurentiu-Gheorghe Crisan

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
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
  MA 02110-1301 USA
*/

#include <Pt/Hmi/Label.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/FontMetrics.h>
#include <Pt/Utf8Codec.h>

namespace Pt {

namespace Hmi {

Label::Label()
: Panel()
{
    setForegroundColor(Gfx::Color::fromRgb8(0,0,0,0));
    setBorderStyle(NoBorder);
    setAutoSize(true);
}


Label::~Label()
{
}


Gfx::SizeF Label::onAutoSize() const
{
    String text = Pt::Utf8Codec::decode( caption() );
    Gfx::FontMetrics fm = Hmi::Painter::fontMetrics( font(), text);
    return Gfx::SizeF( fm.width() + layout().padding().leftRight(), 
                       fm.height() + layout().padding().topBottom() );
}


void Label::onUpdate()
{
    Widget::onUpdate();
}


void Label::onPaint(PaintSurface& surface)
{
    Panel::onPaint(surface);

    Painter& painter = surface.painter();    
    Gfx::SizeF         size = surface.size();
    Gfx::PointF        pos(0,0);
    Gfx::Color         foreColor = foregroundColor();
    Gfx::Pen           pen(1, foreColor);

    painter.setFont( font() );
    painter.setPen(pen);

    String captionStr = Pt::Utf8Codec::decode( caption() );
    Gfx::FontMetrics metric = painter.fontMetrics(captionStr);

    Alignment align = ! isAutoSize() ? contentAlignment() 
                                     : Widget::MiddleCenter;

    switch(align)
    {
        case Widget::TopLeft:
        {
            pos = Gfx::PointF(0, metric.ascent());            
        }
        break;

        case Widget::TopCenter:
        {
            const double widthHalf          = size.width()/2;                                              
            const double textWidthHalf    = metric.width()/2;                                                  
            pos = Gfx::PointF(widthHalf - textWidthHalf, metric.ascent());    
        }
        break;

        case Widget::TopRight:
        {
            const double width          = size.width();                
            const double textWidth    = metric.width();                                    
            pos = Gfx::PointF(width - textWidth, metric.ascent());    
        }
        break;

        case Widget::MiddleLeft:
        {              
            const double heightHalf          = size.height()/2;                
            const double textHeightHalf = metric.height()/2;

            pos = Gfx::PointF(0, (heightHalf - textHeightHalf) + metric.ascent());
        }
        break;

        default:
        case Widget::MiddleCenter:
        {            
            const double widthHalf          = size.width()/2;                
            const double heightHalf          = size.height()/2;                
            const double textWidthHalf    = metric.width()/2;    
            const double textHeightHalf = metric.height()/2;    

            pos = Gfx::PointF(widthHalf - textWidthHalf, (heightHalf - textHeightHalf) + metric.ascent());                            
        }
        break;

        case Widget::MiddleRight:
        {
            const double width          = size.width();                
            const double textWidth    = metric.width();    

            const double heightHalf          = size.height()/2;                
            const double textHeightHalf = metric.height()/2;    

            pos = Gfx::PointF(width - textWidth, (heightHalf - textHeightHalf) + metric.ascent());    
        }
        break;

        case Widget::BottomLeft:
        {
            const double height      = size.height();                
            const double textHeight = metric.height();    

            pos = Gfx::PointF(0, (height- textHeight) + metric.ascent());    
        }
        break;

        case Widget::BottomCenter:
        {
            const double widthHalf          = size.width()/2;                
            const double textWidthHalf    = metric.width()/2;    

            const double height      = size.height();                
            const double textHeight = metric.height();    

            pos = Gfx::PointF(widthHalf - textWidthHalf, (height- textHeight) + metric.ascent());    
        }
        break;

        case Widget::BottomRight:
        {
            const double width          = size.width();                
            const double textWidth    = metric.width();    

            const double height      = size.height();                
            const double textHeight = metric.height();    

            pos = Gfx::PointF(width - textWidth, (height- textHeight) + metric.ascent());    
        }
        break;
    }

    painter.drawText(pos, captionStr);

    const Char* ch = mnemonic();
    if(ch)
    {            
        String::size_type n = captionStr.find( ch->narrow() );
        if(n != String::npos)
        {
            Pt::String text(captionStr.c_str(), n);
            Gfx::FontMetrics fm = painter.fontMetrics(text);
            Gfx::PointF from(pos.x() + fm.width(), pos.y() + 1);

            text = *ch;
            fm = painter.fontMetrics(text);
            Gfx::PointF to( from.x() + fm.width(), from.y() );

            painter.drawLine(from, to);
        }
    }
}

} // namespace

}  // namespace
