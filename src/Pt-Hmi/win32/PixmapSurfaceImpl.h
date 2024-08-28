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

#include <Pt/Gfx/Canvas.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/Gfx/Color.h>
#include <Pt/System/Path.h>

#include <vector>

#include <Windows.h>

//#define PT_HMI_WIN32_RASTER 1

namespace Pt {

namespace Hmi {

class PaintData;
class PixmapSurface;

#ifdef PT_HMI_WIN32_RASTER

class PixmapSurfaceImpl 
{
    public:
        explicit PixmapSurfaceImpl(PixmapSurface&);

        void clear(const Gfx::Color& c)
        { }

        void set(const Gfx::Image& image)
        {
            _image.reset(image);
        }

        void resize(const Gfx::SizeF& size)
        {
            _image.resize(size);
        }

        const Gfx::SizeF& size() const
        {
            return _image.size();
        }

        const Gfx::Scaling& scaling() const
        {
            return _image.scaling();
        }

        void setScaleFactor(double scaleFactor)
        {
            _image.setScaleFactor(scaleFactor);
        }

        const Gfx::ImageFormat& format() const
        {
            return _image.format();
        }

        const Gfx::Canvas* canvas() const
        {
            return _image.canvas();
        }

        Gfx::Image toImage() const
        {
            return _image.toImage();
        }

        Gfx::PaintContextPtr beginPaint(Gfx::PaintContext* context)
        {
            return _image.beginPaint(context);
        }

        const Gfx::ImageSurface& imageSurface() const
        {
            return _image;
        }

        static const std::string& defaultFont()
        {
            return Gfx::ImageSurface::defaultFont();
        }

        static void setDefaultFont(const std::string& name)
        {
            Gfx::ImageSurface::setDefaultFont(name);
        }

        static std::vector<std::string> fontNames()
        {
            return Gfx::ImageSurface::fontNames();
        }
        
        static void setFontDir(const System::Path& path)
        {
            Gfx::ImageSurface::setFontDir(path);
        }
    
    private:
        Gfx::ImageSurface _image;
};

#else // PT_HMI_WIN32_RASTER

class PixmapSurfaceImpl : public Gfx::Canvas
{
    public:
        PixmapSurfaceImpl(Gfx::PaintSurface& surface);

        virtual ~PixmapSurfaceImpl();
        
        void clear(const Gfx::Color& c);

        Gfx::Image toImage() const;

        void set(const Gfx::Image& image);

        void resize(const Gfx::SizeF& size);
        
        void setScaleFactor(double scaleFactor);

        const Gfx::ImageFormat& format() const;
        
        const Gfx::SizeF& size() const;

        const Gfx::Scaling& scaling() const;

        const Canvas* canvas() const
        { 
            return this;
        }
    
    protected:
        virtual Gfx::PaintContext* onBeginPaint(Gfx::PaintContext* context) override;

        virtual void onFinishPaint() override;

    public:
        void setCompositionMode(const Gfx::CompositionMode& mode);

        void setPen(const PaintData& paint);

        void setBrush(const PaintData& paint, const Gfx::Brush& brush);

        void setFont(const PaintData& paint);

        void setClip(const PaintData* paint);

    public:
        void drawLine(const Gfx::Line& line);

        void drawPolyline(const Gfx::Polyline& line);

        void fillPolygon(const Gfx::Polyline& line);

        Gfx::FontMetrics fontMetrics(const Pt::String& text) const;

    public:
        //void setPen(const Gfx::Pen& pen);

        //void setBrush(const Gfx::Brush& brush);

        //void setClip(const Gfx::RectF& clip);

        //void resetClip();

        //void setFont(const Gfx::Font& font);

        void drawText(const Gfx::PointF& to, const Pt::String& text)
        {
            Gfx::Transform trans;
            drawText(to, text, trans);
        }

        void drawText(const Gfx::PointF& to, const Pt::String& text, 
                      const Gfx::Transform& trans);
   
        void drawRect(const Gfx::RectF& rect);

        void fillRect(const Gfx::RectF& rect);

        void drawEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size);

        void fillEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size);

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

        virtual void drawSurface(const Gfx::PointF& to, 
                                 const Gfx::PaintSurface& surface);

        virtual void drawSurface(const Gfx::PointF& to, 
                                 const Gfx::PaintSurface& pm, 
                                 const Gfx::RectF& pmRect);

        void drawPixmap(const Gfx::PointF& toF, 
                        const PixmapSurfaceImpl& surface);

        void drawPixmap(const Gfx::PointF& toF, 
                        const PixmapSurfaceImpl& surface, 
                        const Gfx::RectF& rect);

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

#endif // PT_HMI_WIN32_RASTER

} // namespace

} // namespace

#endif
