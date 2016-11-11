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
#include <Pt/Hmi/Application.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/FontMetrics.h>

namespace Pt {

namespace Hmi {

Label::Label()
: Panel()
, _contentAlignment(TopLeft)
, _font(Application::instance().font())
{
    _userFont = _font;
}


Label::~Label()
{
}


void Label::setText(const Pt::String& text)
{
    _text = Widget::setMnemonic(text);
    invalidate();
}


void Label::setFont(const Gfx::Font& f)
{
    _userFont = f;
    invalidate();
}


Gfx::SizeF Label::onAutoSize() const
{
   Gfx::FontMetrics fm = Hmi::Painter::fontMetrics( _font, _text);

  return Gfx::SizeF( fm.width() + padding().leftRight(), 
                       fm.height() + padding().topBottom() );
}


void Label::onInvalidate()
{
   _font = Application::instance().makeFont(_userFont);
   Panel::onInvalidate();
}


void Label::onPaintBackground(PaintSurface& surface, const Gfx::RectF& updateRect)
{
    Panel::onPaintBackground(surface, updateRect);
}


void Label::onPaintContent(PaintSurface& surface, const Gfx::RectF& updateRect)
{
    Panel::onPaintContent(surface, updateRect);
       
    Gfx::SizeF  size = this->size();
    Gfx::PointF pos(0, 0);

    const StyleOptions* options = getFacet<StyleOptions>();
    if( options == 0)
      return;

    Painter painter(surface);
    painter.setClip(updateRect);
    painter.setPen(options->textColor());
    painter.setFont(_font);

    Gfx::FontMetrics metric = painter.fontMetrics(_text);

    switch(_contentAlignment)
    {
        case TopLeft:
        {
            pos = Gfx::PointF(0, metric.ascent());
        }
        break;

        case TopCenter:
        {
            const double widthHalf     = size.width()/2;
            const double textWidthHalf = metric.width()/2;
            pos = Gfx::PointF(widthHalf - textWidthHalf, metric.ascent());
        }
        break;

        case TopRight:
        {
            const double width     = size.width();
            const double textWidth = metric.width();
            pos = Gfx::PointF(width - textWidth, metric.ascent());
        }
        break;

        case MiddleLeft:
        {
            const double heightHalf     = size.height()/2;
            const double textHeightHalf = metric.height()/2;

            pos = Gfx::PointF(0, (heightHalf - textHeightHalf) + metric.ascent());
        }
        break;

        default:
        case MiddleCenter:
        {            
            const double widthHalf      = size.width()/2;
            const double heightHalf     = size.height()/2;
            const double textWidthHalf  = metric.width()/2;
            const double textHeightHalf = metric.height()/2;

            pos = Gfx::PointF(widthHalf - textWidthHalf, (heightHalf - textHeightHalf) + metric.ascent());
        }
        break;

        case MiddleRight:
        {
            const double width          = size.width();
            const double textWidth      = metric.width();
            const double heightHalf     = size.height()/2;
            const double textHeightHalf = metric.height()/2;

            pos = Gfx::PointF(width - textWidth, (heightHalf - textHeightHalf) + metric.ascent());
        }
        break;

        case BottomLeft:
        {
            const double height     = size.height();
            const double textHeight = metric.height();

            pos = Gfx::PointF(0, (height- textHeight) + metric.ascent());
        }
        break;

        case BottomCenter:
        {
            const double widthHalf     = size.width()/2;
            const double textWidthHalf = metric.width()/2;
            const double height        = size.height();
            const double textHeight    = metric.height();

            pos = Gfx::PointF(widthHalf - textWidthHalf, (height- textHeight) + metric.ascent());
        }
        break;

        case BottomRight:
        {
            const double width      = size.width();
            const double textWidth  = metric.width();
            const double height     = size.height();
            const double textHeight = metric.height();

            pos = Gfx::PointF(width - textWidth, (height- textHeight) + metric.ascent());
        }
        break;
    }

    painter.drawText(pos, _text);

    const Char* ch = mnemonic();

    if(ch)
    {
        String::size_type n = _text.find( *ch );

        if(n != String::npos)
        {
            Pt::String text(_text, 0, n);
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
