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

#include <Pt/Gfx/Paint.h>
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
        explicit PixmapSurfaceImpl();

        void clear(const Gfx::Color& c)
        { }

        void set(const Gfx::Image& image)
        {
            _image.reset(image);
        }

        const Gfx::Image& image() const
        {
            return _image.image();
        }

        const Gfx::SizeF& pixmapSize() const
        {
            return _image.size();
        }

        void resize(const Gfx::SizeF& size)
        {
            _image.resize(size);
        }

        void setScaleFactor(double scaleFactor)
        {
            _image.setScaleFactor(scaleFactor);
        }

        const Gfx::PaintInfo& info() const
        {
            return _image.info();
        }

        Gfx::Canvas* getCanvas()
        {
            return _image.canvas();
        }

        const Gfx::Canvas* getCanvas() const
        {
            return _image.canvas();
        }

        Gfx::PaintContext* getPaint(Gfx::PaintContext* context)
        {
            return _image.getPaint(context);
        }

        void drawPixmap(const Gfx::PointF& toF, 
                        const PixmapSurface& surface,
                        const Gfx::CompositionMode& mode);

        void drawPixmap(const Gfx::PointF& toF, 
                        const PixmapSurface& surface, 
                        const Gfx::RectF& rect,
                        const Gfx::CompositionMode& mode);

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

class PixmapSurfaceImpl : public Gfx::PaintInfo
                        , public Gfx::Canvas
{
    public:
        PixmapSurfaceImpl();

        virtual ~PixmapSurfaceImpl();
        
        void clear(const Gfx::Color& c);

        void set(const Gfx::Image& image);

        const Gfx::SizeF& pixmapSize() const;

        void resize(const Gfx::SizeF& size);
        
        void setScaleFactor(double scaleFactor);

        Canvas* getCanvas()
        { 
            return this;
        }

        const Canvas* getCanvas() const
        { 
            return this;
        }
           
    protected:
        virtual const Gfx::ImageFormat& onGetFormat() const override;

        virtual const Gfx::SizeF& onGetSize() const override;

        virtual const Gfx::Scaling& onGetScaling() const override;

    protected:
        virtual bool onGetPaint(Gfx::PaintContext* context) override;

        virtual Gfx::PaintContext* onGetPaint() override;

        virtual void onReleasePaint() override;

    protected:
        virtual void onCompositionModeChanged() override;

        virtual void onPenChanged() override;

        virtual void onBrushChanged() override;

        virtual void onFontChanged() override;

        virtual void onClipChanged() override;

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
        Gfx::SizeF     _physicalSize;
        Gfx::SizeF     _infoSize;
        Gfx::Scaling   _infoScaling;
        LONG           _width;
        LONG           _height;
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
