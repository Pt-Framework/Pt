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

#ifndef Pt_Hmi_PixmapSurfaceImpl_h
#define Pt_Hmi_PixmapSurfaceImpl_h

#include <Pt/Hmi/Api.h>
#include <Pt/Hmi/PaintSurface.h>
#include <Pt/Gfx/ImagePainter.h>
#include "PaintSurfaceImpl.h"

namespace Pt {
namespace Hmi {

class PixmapSurfaceImpl : public PaintSurfaceImpl
{
    public:        
        PixmapSurfaceImpl();
        
        virtual ~PixmapSurfaceImpl();  

        void clear(const Gfx::Color& c);

        const Gfx::Image& image() const
        {
            return _image;
        }

        Gfx::Image& image()
        {
            return _image;
        }
    
        const Gfx::ImageFormat& format() const;

        void begin(Painter& painter);  
        
        void finish();    

        void resize(const Gfx::Size& size, size_t stride);

        void resize(const Gfx::SizeF& size);    

        const Gfx::SizeF& size() const;

        void setClip( const Gfx::RectF& clip);

        void resetClip();

        void setCompositionMode(const Gfx::CompositionMode& mode);

        void setPen(const Gfx::Pen& pen);

        void setBrush(const Gfx::Brush& brush);

        void setFont(const Gfx::Font& font);

        Gfx::FontMetrics fontMetrics(const Pt::String& text) const;

        void drawLine(const Gfx::PointF& from, const Gfx::PointF& to);

        void drawText(const Gfx::PointF& to, const Pt::String& Text);

        void drawText(const Gfx::PointF& to, const Pt::String& Text, const Gfx::Transform& trans);

        void drawRect(const Gfx::RectF& rectangle);

        void fillRect(const Gfx::RectF& rectangle);

        void drawEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size);

        void fillEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size);

        void drawPolyline(const Gfx::PointF* points, size_t pointCount);

        void fillPolygon(const Gfx::PointF* points, size_t pointCount);

        void drawPath(const Gfx::Path& path, float smoothness)
        {}

        void fillPath(const Gfx::Path& path, float smoothness)
        {}

        void drawSurface(const Gfx::PointF& toF, const PixmapSurface& surface);

        void drawSurface(const Gfx::PointF& toF, 
                         const PixmapSurface& pm,
                         const Gfx::RectF& pmRect);

        void drawImage(const Gfx::PointF& to, const Gfx::Image& image);

        void drawImage(const Gfx::PointF& to, const Gfx::Image& image, const Gfx::RectF& r);

        void drawPicture(const Gfx::PointF& to, const Picture& pic);
        
    private:
        Gfx::SizeF            _size;
        Gfx::Image            _image;
        Gfx::ImagePainter     _painter;
};

} // namespace

}  // namespace

#endif
