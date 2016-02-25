/* Copyright (C) 2015 Laurentiu-Gheorghe Crisan
   Copyright (C) 2015 Marc Boris Duerner 

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
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA*/
#ifndef PT_HMI_PAINTER_H
#define PT_HMI_PAINTER_H

#include <Pt/Hmi/Api.h>
#include <Pt/Hmi/Painter.h>
#include <Pt/Gfx/FontMetrics.h>
#include <Pt/Gfx/Painter.h>

namespace Pt {

namespace Hmi {

class PainterImpl;
class PaintSurface;
class PixmapSurface;

class PT_HMI_API Painter : public Gfx::Painter
{
    public:
        Painter(PaintSurface& surface);

        virtual ~Painter();

        void setSurface(PaintSurface& surface); 

        void setOrigin(const Gfx::PointF& p);

        virtual void setClip( const Gfx::RectF& clip );
        
        virtual const Gfx::RectF& clip() const;

        virtual void setRenderMode(Gfx::RenderMode::Type mode);

        virtual void setPen(const Gfx::Pen& pen);

        virtual const Gfx::Pen& pen() const;

        virtual void setBrush(const Gfx::Brush& brush);

        virtual const Gfx::Brush& brush() const;

        virtual void setFont(const Gfx::Font& font);

        virtual const Gfx::Font& font() const;    

        virtual Gfx::FontMetrics fontMetrics(const Pt::String& Text) const; 
        
        static Gfx::FontMetrics fontMetrics(const Gfx::Font& font, const Pt::String& text);   

        virtual void drawLine(const Gfx::PointF& from, const Gfx::PointF& to);

        virtual void drawText(const Gfx::PointF& to, const Pt::String& Text);

        virtual void drawRect(const Gfx::RectF& rectangle);        

        virtual void fillRect(const Gfx::RectF& rectangle);

        virtual void drawEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size);

        virtual void fillEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size);

        virtual void drawPolyline(const Gfx::PointF* points, const size_t pointCount);

        virtual void fillPolygon(const Gfx::PointF* points, const size_t pointCount);

        virtual void drawSurface(const Gfx::PointF& to, const PixmapSurface& pm);
        
        virtual void drawImage(const Gfx::PointF& to, const Gfx::Image& image);
    
        virtual void flush();    

        virtual void clear( const Gfx::Color& color = Gfx::Color( 1, 1, 1 ) );
    
        PainterImpl* impl()
        {
            return _impl;
        }

    private:
        PainterImpl* _impl;
};

}

}

#endif
