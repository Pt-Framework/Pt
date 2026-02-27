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

#ifndef PT_FORMS_PAINTERIMPL_H
#define PT_FORMS_PAINTERIMPL_H

#include <Pt/Forms/Api.h>
#include <Pt/Forms/PaintSurface.h>
#include <Pt/Forms/Painter.h>
#include <Pt/Gfx/ImagePainter.h>
#include <Pt/Gfx/TextMetrics.h>

namespace Pt {

namespace Forms {

class PaintSurfaceImpl;
class PaintSurface;
class PixmapSurface;

class PainterImpl
{
    public:
        PainterImpl(PaintSurfaceImpl* surface);
        
        virtual ~PainterImpl();

        void setSurface(PaintSurface& surface);

        void setOrigin(const Gfx::PointF& p)
        {
            _origin = p;
        }

        void setClip(const Gfx::RectF& clip)
        {
            _clip = clip;
        }
        
        const Gfx::RectF& clip() const
        {
            return _clip;
        }

        void setRenderMode(Gfx::RenderMode::Type mode);

        void setPen(const Gfx::Pen& pen);

        const Gfx::Pen& pen() const;

        void setBrush(const Gfx::Brush& brush);

        const Gfx::Brush& brush() const;

        void setFont(const Gfx::Font& font);

        const Gfx::Font& font() const;        

        Gfx::TextMetrics textMetrics(const Pt::String& Text) const;

        static Gfx::TextMetrics textMetrics(const Gfx::Font& font, const Pt::String& text);

        std::list<std::string> fontFamilyNames();        

        void drawLine(const Gfx::PointF& from, const Gfx::PointF& to);                

        void drawText(const Gfx::PointF& to, const Pt::String& Text);

        void drawRect(const Gfx::RectF& rectangle);

        void fillRect(const Gfx::RectF& rectangle);

        void drawEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size);

        void fillEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size);

        void drawPolyline(const Gfx::PointF* points, const size_t pointCount);

        void fillPolygon(const Gfx::PointF* points, const size_t pointCount);

        void drawSurface(const Gfx::PointF& to, const PixmapSurface& pm);

        void drawSurface(const Gfx::PointF& to, 
                         const PixmapSurface& pm, 
                         const Gfx::RectF& pmRect);
        
        void drawImage(const Gfx::PointF& to, const Gfx::Image& image);

        void flush();
        
        void clear(const Gfx::ColorF& color);

    private:
        PaintSurfaceImpl*      _surface;
        Gfx::PointF            _origin;
        Gfx::RectF             _clip;
        Gfx::Pen               _pen;
        Gfx::Brush             _brush;
        Gfx::Font              _font;
};

} // namespace

} // namespace

#endif
