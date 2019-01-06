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

#ifndef Pt_Hmi_cocoa_PainterImpl_h
#define Pt_Hmi_cocoa_PainterImpl_h

#include <Pt/Hmi/Api.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/Gfx/Font.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/Gfx/FontMetrics.h>
#include <Pt/String.h>

#include <vector>

namespace Pt {

namespace Hmi {

class PainterImpl
{
    public:
        PainterImpl()
        { }

        ~PainterImpl()
        {
        }

        void setPen(const Gfx::Pen& pen)
        {
        }

        void setBrush(const Gfx::Brush& brush)
        {
        }

        void setClip(const Gfx::RectF& rectF)
        {
        }

        void setCompositionMode(const Gfx::CompositionMode& mode)
        {
        }

        void setFont(const Gfx::Font& font)
        {
        }
        
        static Gfx::FontMetrics fontMetrics(const Gfx::Font& font, 
                                            const Pt::String& text)
        {   
            return Gfx::FontMetrics(0, 0, 0, 0);; 
        }
        
        static std::string defaultFont()
        {
            return std::string();
        }

        static void setDefaultFont(const std::string& f)
        {
        }

        //static void setFontDir(const Pt::System::Path& path)
        //{
        //}
};
/*
class PaintSurfaceImpl;
class PaintSurface;

class PainterImpl
{
    public:
        PainterImpl(PaintSurfaceImpl* surface);

        virtual ~PainterImpl();

        void setPen(const Gfx::Pen& pen);

        const Gfx::Pen& pen() const;

        void setBrush(const Gfx::Brush& brush);

        const Gfx::Brush& brush() const;

        const Gfx::Font& font() const;

        void setFont(const Gfx::Font& font);

        Gfx::FontMetrics fontMetrics() const;

        Gfx::FontMetrics fontMetrics(const Pt::String& text) const;

        const std::list<std::string>& fontFamilyNames();

        virtual void drawLine(const Gfx::PointF& from, const Gfx::PointF& to);

        virtual void drawText(const Gfx::PointF& to, const Pt::String& text);           

        virtual void drawRect(const Gfx::RectF& rect);

        virtual void drawEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size);

        virtual void drawPolyline(const Gfx::PointF* points, const size_t pointCount);

        virtual void fillRect(const Gfx::RectF& rect);

        virtual void fillEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size);

        virtual void fillPolygon(const Gfx::PointF* points, const size_t pointCount);

        virtual void drawSurface(const Gfx::PointF& to, PaintSurface& pm);

        virtual void drawImage(const Gfx::PointF& to, const Gfx::ARgbImage& image);

        virtual void drawImage(const Gfx::PointF& to, const Gfx::ARgbImage& image, const Gfx::Region& imageRegion);

        int depth()
        {
            return 0;
        }
    
        void setSurface(PaintSurface& s);

    
        void addFontName(const std::string& fontName)
        {
        }
    private:
    Pt::Gfx::PointF tranPoint(const Pt::Gfx::PointF& p);
	protected:
        Gfx::Pen            _pen;
        Gfx::Brush          _brush;
        Gfx::Font           _font;
        PaintSurfaceImpl*   _surface;
};
*/

} // namespace

} // namespace

#endif
