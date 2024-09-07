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

#include <Pt/Hmi/Api.h>

//#define PT_HMI_WIN32_RASTER 1

#ifdef PT_HMI_WIN32_RASTER

#include <Pt/Gfx/ImageSurface.h>

#else

#include <Pt/Gfx/Canvas.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/Gfx/Color.h>
#include <Pt/Gfx/Path.h>
#include <Pt/System/Path.h>

#include <vector>

#include <Windows.h>

#endif

namespace Pt {

namespace Hmi {

class PixmapSurface;

#ifdef PT_HMI_WIN32_RASTER

class PixmapSurfaceImpl 
{
    public:
        explicit PixmapSurfaceImpl(PixmapSurface& surface);

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

        const Gfx::Scaling& surfaceScaling() const
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

        const Gfx::Canvas* getCanvas() const
        {
            return _image.canvas();
        }

        Gfx::PaintContext* beginPaint(Gfx::PaintContext* context)
        {
            return _image.beginPaint(context);
        }

        void drawPixmap(const Gfx::PointF& toF, 
                        const PixmapSurface& surface,
                        const Gfx::CompositionMode& mode);

        void drawPixmap(const Gfx::PointF& toF, 
                        const PixmapSurface& surface, 
                        const Gfx::RectF& rect,
                        const Gfx::CompositionMode& mode);

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

class PaintContext;

class PixmapSurfaceImpl : public Gfx::Canvas
{
    public:
        PixmapSurfaceImpl(PixmapSurface& surface);

        virtual ~PixmapSurfaceImpl();
        
        void clear(const Gfx::Color& c);

        void set(const Gfx::Image& image);

        void resize(const Gfx::SizeF& size);
        
        void setScaleFactor(double scaleFactor);

        const Gfx::Scaling& surfaceScaling() const;

        const Gfx::ImageFormat& format() const;
        
        const Gfx::SizeF& size() const;

        const Canvas* getCanvas() const
        { 
            return this;
        }
           
    protected:
        virtual const Gfx::Scaling& onGetScaling() const override;

        virtual bool onBeginPaint(Gfx::PaintContext* context) override;

        virtual Gfx::PaintContext* onBeginPaint() override;

        virtual void onReleasePaint() override;

    protected:
        virtual void onApplyCompositionMode(Gfx::PaintContext& paint) override;

        virtual void onApplyPen(Gfx::PaintContext& paint) override;

        virtual void onApplyBrush(Gfx::PaintContext& paint) override;

        virtual void onApplyFont(Gfx::PaintContext& paint) override;

    protected:
        virtual void onSetClip(const Gfx::RectF& clip) override;

        virtual void onResetClip() override;

    protected:
        virtual void onDrawLine(const Gfx::PointF& from, 
                                const Gfx::PointF& to) override;

        virtual void onDrawPolyline(const Gfx::Polyline& line) override;

        virtual void onFillPolygon(const Gfx::Polyline& line) override;

        virtual void onDrawRect(const Gfx::RectF& rect) override;

        virtual void onFillRect(const Gfx::RectF& rect) override;

        virtual void onDrawEllipse(const Gfx::PointF& topLeft, 
                                   const Gfx::SizeF& size) override;

        virtual void onFillEllipse(const Gfx::PointF& topLeft, 
                                   const Gfx::SizeF& size) override;
        
        virtual void onDrawArc(const Gfx::PointF& topLeft, const Gfx::SizeF& size, 
                               float degBegin, float degEnd) override
        {}

        virtual void onFillChord(const Gfx::PointF& topLeft, const Gfx::SizeF& size, 
                                 float degBegin, float degEnd) override
        {}

        virtual void onFillPie(const Gfx::PointF& topLeft, const Gfx::SizeF& size, 
                               float degBegin, float degEnd) override
        {}

        virtual void onDrawPath(const Gfx::Path& path, float smoothness) override
        {}

        virtual void onFillPath(const Gfx::Path& path, float smoothness) override
        {}

    protected:
        virtual Gfx::FontMetrics onGetFontMetrics(const Pt::String& text) const override;

        virtual void onDrawText(const Gfx::PointF& to, const Pt::String& text, 
                                const Gfx::Transform* trans) override;

    protected:
        virtual Gfx::Image onGetImage() const override;

        virtual void onDrawImage(const Gfx::PointF& to, 
                                 const Gfx::Image& image) override;

        virtual void onDrawImage(const Gfx::PointF& to, 
                                 const Gfx::Image& image, 
                                 const Gfx::RectF& rect) override;

        virtual void onDrawCanvas(const Gfx::PointF& to, 
                                  const Gfx::Canvas& surface) override;

        virtual void onDrawCanvas(const Gfx::PointF& to, 
                                  const Gfx::Canvas& canvas, 
                                  const Gfx::RectF& pmRect) override;

    public:
        void drawPixmap(const Gfx::PointF& toF, 
                        const PixmapSurfaceImpl& surface);

        void drawPixmap(const Gfx::PointF& toF, 
                        const PixmapSurfaceImpl& surface, 
                        const Gfx::RectF& rect);

        void drawPixmap(const Gfx::PointF& toF, 
                        const PixmapSurface& surface,
                        const Gfx::CompositionMode& mode);

        void drawPixmap(const Gfx::PointF& toF, 
                        const PixmapSurface& surface, 
                        const Gfx::RectF& rect,
                        const Gfx::CompositionMode& mode);

    public:
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

        PaintContext*             _paintContext;

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
