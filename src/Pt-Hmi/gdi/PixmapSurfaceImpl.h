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

#ifndef Pt_Hmi_PixmalSurfaceImpl_h
#define Pt_Hmi_PixmalSurfaceImpl_h

#include <Pt/Gfx/ImageSurface.h>

#include <Pt/Gfx/PaintSurface.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/Gfx/Color.h>
#include <Pt/System/Path.h>

#include <vector>

#include <Windows.h>

//#define PT_HMI_PIXMAP_IMPL_IMAGE 1

namespace Pt {

namespace Hmi {

class PaintData;
class PixmapSurface;

#ifdef PT_HMI_PIXMAP_IMPL_IMAGE

class PixmapSurfaceImpl : public Gfx::ImageSurface
{
    friend class PixmapSurface;

    public:
        PixmapSurfaceImpl()
        { }

        void clear(const Gfx::Color& c)
        { }

        void set(const Gfx::Image& image)
        {
            reset(image);
        }

    protected:
        virtual void drawSurface(const Gfx::PointF& to, 
                                 const Gfx::PaintSurface& surface);

        virtual void drawSurface(const Gfx::PointF& to, 
                                 const Gfx::PaintSurface& pm, const Gfx::RectF& pmRect);
};

#else

class PixmapSurfaceImpl : public Gfx::Canvas
{
    public:
        PixmapSurfaceImpl();

        virtual ~PixmapSurfaceImpl();
        
        void clear(const Gfx::Color& c);

        void resize(const Gfx::SizeF& size);

        const Gfx::SizeF& size() const;
        
        Gfx::PaintData* getPaint(Gfx::PaintData* paint);

        Gfx::Canvas* canvas();

        void finish();

        double scaleFactor() const;

        void setScaleFactor(double scaleFactor);

        const Gfx::Scaling& scaling() const
        {
            return _scaling;
        }

        Gfx::Image toImage() const;

        void set(const Gfx::Image& image);

    protected:
        virtual const Gfx::ImageFormat& onGetFormat() const;

        virtual const Gfx::SizeF& onSize() const
        {
            return _size;
        }

        virtual const Gfx::Scaling& onGetScaling() const
        {
            return _scaling;
        }

        virtual void onFinish()
        {
            finish();
        }

    public:
        void setClip(const Gfx::RectF& clip);

        void resetClip();
         
        void setCompositionMode(const Gfx::CompositionMode& mode);

        void setPen(const Gfx::Pen& pen);

        void setBrush(const Gfx::Brush& brush);

        void setFont(const Gfx::Font& font);

        Gfx::FontMetrics fontMetrics(const Pt::String& text) const;

        void drawText(const Gfx::PointF& to, const Pt::String& text)
        {
            Gfx::Transform trans;
            drawText(to, text, trans);
        }

        void drawText(const Gfx::PointF& to, const Pt::String& text, 
                      const Gfx::Transform& trans);
   
        void drawLine(const Gfx::Line& line);

        void drawRect(const Gfx::RectF& rect);

        void fillRect(const Gfx::RectF& rect);

        void drawEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size);

        void fillEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size);

        void drawPolyline(const Gfx::Polyline& line);

        void fillPolygon(const Gfx::Polyline& line);

        void drawPath(const Gfx::Path& path, float smoothness)
        {}

        void fillPath(const Gfx::Path& path, float smoothness)
        {}

        virtual void drawChord(const Gfx::PointF& topLeft, const Gfx::SizeF& size, float degBegin, float degEnd)
        {}

        virtual void fillChord(const Gfx::PointF& topLeft, const Gfx::SizeF& size, float degBegin, float degEnd)
        {}

        virtual void drawPie(const Gfx::PointF& topLeft, const Gfx::SizeF& size, float degBegin, float degEnd)
        {}

        virtual void fillPie(const Gfx::PointF& topLeft, const Gfx::SizeF& size, float degBegin, float degEnd)
        {}

        virtual void drawArc(const Gfx::PointF& topLeft, const Gfx::SizeF& size, float degBegin, float degEnd)
        {}


        virtual void drawSurface(const Gfx::PointF& to, const Gfx::PaintSurface& surface);

        virtual void drawSurface(const Gfx::PointF& to, const Gfx::PaintSurface& pm, const Gfx::RectF& pmRect);


        void drawPixmap(const Gfx::PointF& toF, const PixmapSurface& surface);

        void drawPixmap(const Gfx::PointF& toF, 
                        const PixmapSurface& surface, const Gfx::RectF& rect);

        void drawImage(const Gfx::PointF& to, const Gfx::Image& image);

        void drawImage(const Gfx::PointF& to, 
                       const Gfx::Image& image, const Gfx::RectF& rect);

        static const std::string& defaultFont();

        static void setDefaultFont(const std::string& name);

        static std::vector<std::string> fontNames();

        static void setFontDir(const System::Path& path);

        HDC deviceContext() const;

    private: 
        void bitBlit(const Gfx::Point& pos, size_t width, size_t height, 
                     HBITMAP bitmap, DWORD op);

        static void toPreMulAlpha(const Pt::Gfx::Image& image, 
                                  std::vector<Pt::uint8_t>& preMul);

        static std::string& getDefaultFont();

        static std::string getSystemFont();

    private:
        PaintData*     _paint;
        Gfx::SizeF     _size;
        Gfx::Scaling   _scaling;
        HDC            _dc;
        HBITMAP        _bitmap;
        HPEN           _oldPen;
        HBRUSH         _oldBrush;
        HFONT          _oldFont;
        HBITMAP        _oldBitmap;
        std::wstring   _text;

        Gfx::Color                _penColor;

        bool                      _gradientBrush;
        Gfx::Brush::GradientStyle _gradient;
        Gfx::Color                _gradientStart;
        Gfx::Color                _gradientStop;
        Gfx::CompositionMode      _compositionMode;
};

#endif // PT_HMI_PIXMAP_IMPL_IMAGE

} // namespace

} // namespace

#endif
