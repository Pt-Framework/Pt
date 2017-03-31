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

        void resize(const Gfx::Size& size);    

        const Gfx::Size& size() const;

        void setClip( const Gfx::Rect& clip);

        void setCompositionMode(const Gfx::CompositionMode& mode);

        void setPen(const Gfx::Pen& pen);

        void setBrush(const Gfx::Brush& brush);

        void setFont(const Gfx::Font& font);

        Gfx::FontMetrics fontMetrics(const Pt::String& text) const;

        void drawLine(const Gfx::Point& from, const Gfx::Point& to);

        void drawText(const Gfx::Point& to, const Pt::String& Text);

        void drawRect(const Gfx::Rect& rectangle);

        void fillRect(const Gfx::Rect& rectangle);

        void drawEllipse(const Gfx::Point& topLeft, const Gfx::Size& size);

        void fillEllipse(const Gfx::Point& topLeft, const Gfx::Size& size);

        void drawPolyline(const Gfx::Point* points, size_t pointCount);

        void fillPolygon(const Gfx::Point* points, size_t pointCount);

        void drawSurface(const Gfx::Point& toF, const PixmapSurface& surface);

        void drawSurface(const Gfx::Point& to, 
                         const PixmapSurface& pm,
                         const Gfx::Rect& pmRect);

        void drawImage(const Gfx::Point& to, const Gfx::Image& image);

        void drawImage(const Gfx::Point& to, const Gfx::Image& image, const Gfx::Rect& r);

        void drawPicture(const Gfx::Point& to, const Picture& pic);
        
    private:
        Gfx::Size            _size;
        Gfx::Image            _image;
        Gfx::ImagePainter     _painter;
};

} // namespace

}  // namespace

#endif
