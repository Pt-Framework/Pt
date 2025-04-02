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
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
  MA 02110-1301 USA
*/

#ifndef Pt_Forms_ImagePaintSurface_h
#define Pt_Forms_ImagePaintSurface_h

#include <Pt/Forms/Api.h>
#include <Pt/Forms/PaintSurface.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/ImagePainter.h>

namespace Pt {

namespace Forms {

class PixmapSurface;

class PaintSurfaceImpl
{
    public:        
        virtual ~PaintSurfaceImpl();

        virtual const Gfx::SizeF& size() const = 0;

        virtual void setPen(const Gfx::Pen& pen) = 0;

        virtual void setBrush(const Gfx::Brush& brush) = 0;

        virtual void setFont(const Gfx::Font& font) = 0;

        virtual Gfx::FontMetrics fontMetrics(const Pt::String& text) const = 0;
    
        virtual void drawLine(const Gfx::PointF& from, const Gfx::PointF& to) = 0;

        virtual void drawText(const Gfx::PointF& to, const Pt::String& Text) = 0;

        virtual void drawRect(const Gfx::RectF& rectangle) = 0;

        virtual void fillRect(const Gfx::RectF& rectangle) = 0;

        virtual void drawEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size) = 0;

        virtual void fillEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size) = 0;

        virtual void drawPolyline(const Gfx::PointF* points, size_t pointCount) = 0;

        virtual void fillPolygon(const Gfx::PointF* points, size_t pointCount) = 0;

        virtual void drawSurface(const Gfx::PointF& toF, const PixmapSurface& surface) = 0;
        
        virtual void drawSurface(const Gfx::PointF& toF, 
                                 const PixmapSurface& pm,
                                 const Gfx::RectF& pmRect) = 0;
        
        virtual void drawImage(const Gfx::PointF& to, const Gfx::Image& image) = 0;

        static std::string defaultFont();

        static std::list<std::string> fontFamilyNames(); 

        static Gfx::FontMetrics fontMetrics(const Gfx::Font& font, const Pt::String& text);

    protected:
        PaintSurfaceImpl();
};


class PaintRegionImpl : public PaintSurfaceImpl
{
    public:        
        PaintRegionImpl();
        
        virtual ~PaintRegionImpl();   

        void set(PaintSurface& surface, const Gfx::RectF& area); 

        virtual const Gfx::SizeF& size() const;

        virtual void setPen(const Gfx::Pen& pen);

        virtual void setBrush(const Gfx::Brush& brush);

        virtual void setFont(const Gfx::Font& font);

        virtual Gfx::FontMetrics fontMetrics(const Pt::String& text) const;

        virtual void drawLine(const Gfx::PointF& from, const Gfx::PointF& to);

        virtual void drawText(const Gfx::PointF& to, const Pt::String& Text);

        virtual void drawRect(const Gfx::RectF& rectangle);

        virtual void fillRect(const Gfx::RectF& rectangle);

        virtual void drawEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size);

        virtual void fillEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size);

        virtual void drawPolyline(const Gfx::PointF* points, size_t pointCount);

        virtual void fillPolygon(const Gfx::PointF* points, size_t pointCount);

        virtual void drawSurface(const Gfx::PointF& toF, const PixmapSurface& surface);

        virtual void drawSurface(const Gfx::PointF& toF, 
                                 const PixmapSurface& pm,
                                 const Gfx::RectF& pmRect);

        virtual void drawImage(const Gfx::PointF& to, const Gfx::Image& image);

    private:
        PaintSurface* _surface;
        Gfx::RectF _area;
};


class PixmapSurfaceImpl : public PaintSurfaceImpl
{
    public:        
        PixmapSurfaceImpl();
        
        virtual ~PixmapSurfaceImpl();  

        void clear();

        const Gfx::Image& image() const
        {
            return _image;
        }

        Gfx::Image& image()
        {
            return _image;
        }
    
        void resize(const Gfx::Size& size, size_t stride);

        void resize(const Gfx::SizeF& size);    

        virtual const Gfx::SizeF& size() const;

        virtual void setPen(const Gfx::Pen& pen);

        virtual void setBrush(const Gfx::Brush& brush);

        virtual void setFont(const Gfx::Font& font);

        virtual Gfx::FontMetrics fontMetrics(const Pt::String& text) const;

        virtual void drawLine(const Gfx::PointF& from, const Gfx::PointF& to);

        virtual void drawText(const Gfx::PointF& to, const Pt::String& Text);

        virtual void drawRect(const Gfx::RectF& rectangle);

        virtual void fillRect(const Gfx::RectF& rectangle);

        virtual void drawEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size);

        virtual void fillEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size);

        virtual void drawPolyline(const Gfx::PointF* points, size_t pointCount);

        virtual void fillPolygon(const Gfx::PointF* points, size_t pointCount);

        virtual void drawSurface(const Gfx::PointF& toF, const PixmapSurface& surface);

        virtual void drawSurface(const Gfx::PointF& toF, 
                                 const PixmapSurface& pm,
                                 const Gfx::RectF& pmRect);

        virtual void drawImage(const Gfx::PointF& to, const Gfx::Image& image);

    private:
        Gfx::SizeF            _size;
        Gfx::Image            _image;
        Gfx::ImagePainter     _painter;
};

} // namespace

} // namespace

#endif
