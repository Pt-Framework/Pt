/* Copyright (C) 2006-2015 Laurentiu-Gheorghe Crisan
 * Copyright (C) 2006-2015 Marc Boris Duerner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA*/
#ifndef PT_GFX_IMAGEPAINTER_H
#define PT_GFX_IMAGEPAINTER_H

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/Gfx/Rasterizer.h>

namespace Pt {
namespace Gfx {

class PT_GFX_API ImagePainter : public Painter
{
  public:
    ImagePainter( Image& image );
    
    virtual ~ImagePainter();

    void setImage(Image& image);

    virtual void setRenderMode( RenderMode::Type mode);
    
    virtual void setPen(const Pen& pen);

    virtual const Pen& pen() const;

    virtual void setBrush(const Brush& brush);

    virtual const Brush& brush() const;

    virtual void setFont(const Font& font);

    virtual const Font& font() const;

    virtual FontMetrics fontMetrics(const Pt::String& text) const;

    virtual void drawLine(const  PointF& from, const  PointF& to);

    virtual void drawText(const  PointF& to, const Pt::String& text );

    virtual void drawRect(const  RectF& rect);
    
    virtual void fillRect(const  RectF& rect);

    virtual void drawEllipse(const PointF& topLeft, const  SizeF& size);

    virtual void fillEllipse(const PointF& topLeft, const  SizeF& size);

    virtual void drawPolyline(const PointF* points, const size_t pointCount);
    
    virtual void fillPolygon(const PointF* points, const size_t pointCount);

    virtual void drawImage(const  PointF& to, const Image& image);

    virtual void drawImage(const PointF& to, const Image& image, const RectF& imageRect);

    virtual void setClip( const RectF& clip );

    virtual void setPixel(const  PointF& to);
        
    virtual const Gfx::RectF& clip() const;

    virtual void clear( const Gfx::Color& color =Gfx::Color( 1, 1, 1 ) );

    static FontMetrics fontMetrics( const Font& font, const Pt::String& text );

  private:
    Rasterizer _rasterizer;
};

}}

#endif

