 /* Copyright (C) 2015 Marc Boris Duerner 
  
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

#ifndef PT_HMI_COCOA_PIXMAP_IMPL_H
#define PT_HMI_COCOA_PIXMAP_IMPL_H

#include <Pt/Gfx/PaintSurface.h>
#include <Pt/Gfx/Canvas.h>

#include <Pt/Gfx/Path.h>
#include <Pt/System/Path.h>

#include <CoreGraphics/CGBitmapContext.h>

#ifdef __OBJC__
    #import <Foundation/NSGeometry.h>
    #import <AppKit/NSGraphicsContext.h>
    #import <AppKit/NSBezierPath.h>
    #import <AppKit/NSImage.h>
    #import <AppKit/NSColor.h>
    #import <AppKit/NSFontManager.h>
#else
    struct NSBezierPath;
    struct NSImage;
#endif

namespace Pt {

namespace Hmi {

class PixmapCanvas : public Gfx::Canvas
{
    public:
        PixmapCanvas(Gfx::PaintSurface& surface);            

        virtual ~PixmapCanvas();

        CGContextRef context() const
        { return _context; }

        void set(const Gfx::Image& image);
        
        Gfx::Image toImage() const;

        const Gfx::SizeF& physicalSize() const;

        const Gfx::SizeF& logicalSize() const;

        void resize(const Gfx::SizeF& size);

        void setScaleFactor(double scaleFactor);
    
    protected:
        virtual const Gfx::ImageFormat& onGetFormat() const override;

        virtual const Gfx::SizeF& onGetSize() const override;

        virtual const Gfx::Scaling& onGetScaling() const override;

    protected:
        virtual bool onSetPaint(Gfx::PaintContext* context) override;

        virtual Gfx::PaintContext* onCreatePaint() override;

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
        virtual void onDrawImage(const Gfx::PointF& to, 
                                 const Gfx::Image& image, 
                                 const Gfx::RectF* rect) override;

        virtual bool onDrawLayer(const Gfx::PointF& to, 
                                 const Gfx::PaintLayer& layer,
                                 const Gfx::RectF* rect) override;

    private:
        void create();
    
        void destroy();

        void beginClip();

        void endClip();

        Pt::Gfx::PointF transform(const Pt::Gfx::PointF& p);
};


class PixmapImpl : public Gfx::PaintSurface
{
    public:
        PixmapImpl();

        virtual ~PixmapImpl();

        void set(const Gfx::Image& image);

        Gfx::Image toImage() const;
        
        void clear(const Gfx::Color& c);

        const Gfx::SizeF& size() const;

        void resize(const Gfx::SizeF& size);
        
        void setScaleFactor(double scaleFactor);
           
        Gfx::PaintSurface* surface()
        {
            return this;
        }

        void draw(Gfx::PaintSurface& surface, 
                  const Gfx::Paint& paint,
                  const Gfx::PointF& to,
                  const Gfx::RectF* rect) const;

        CGContextRef context() const;

    public:
        static const std::string& defaultFont();

        static void setDefaultFont(const std::string& name);

        static std::vector<std::string> fontNames();

        static void setFontDir(const System::Path& path);

    private: 
        static std::string& getDefaultFont();

        static std::string getSystemFont();

    private:
        PixmapCanvas*  _canvas;
};


class PixmapSurfaceImpl 
{
    public:
        const Gfx::SizeF& size() const;

        void resize(const Pt::Gfx::SizeF& size);

        void begin(Gfx::Painter& painter);  
        
        void finish();

        const Gfx::ImageFormat& format() const;

        void setScaleFactor(double scaling)
        {
        }

        void setClip(const Gfx::RectF& clip);

        void resetClip();

        void setCompositionMode(const Gfx::CompositionMode& mode);

        void setPen(const Gfx::Pen& pen);

        void setBrush(const Gfx::Brush& brush);

        void setFont(const Gfx::Font& font);

        Gfx::FontMetrics fontMetrics(const Pt::String& text) const;

        void drawLine(const Gfx::PointF& from, const Gfx::PointF& to);

        void drawText(const Gfx::PointF& to, const Pt::String& text, 
                      const Gfx::Transform& trans);

        void drawRect(const Gfx::RectF& rect);

        void fillRect(const Gfx::RectF& rect);

        void drawEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size);

        void fillEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size);

        void drawPolyline(const Gfx::PointF* points, size_t pointCount);

        void fillPolygon(const Gfx::PointF* points, size_t pointCount);

        void drawPath(const Gfx::Path& path, float smoothness)
        {}

        void fillPath(const Gfx::Path& path, float smoothness)
        {}

        void drawSurface(const Gfx::PointF& to, const PixmapSurface& surface);

        void drawSurface(const Gfx::PointF& to, 
                         const PixmapSurface& pm,
                         const Gfx::RectF& pmRect);

        void drawImage(const Gfx::PointF& to, const Gfx::Image& image);

        void drawImage(const Gfx::PointF& to, 
                       const Gfx::Image& image, 
                       const Gfx::RectF& imgRect);

        Pt::Gfx::PointF transform(const Pt::Gfx::PointF& p);

        Gfx::Image toImage() const;

        void set(const Gfx::Image& image);

        static const std::string& defaultFont();

        static void setDefaultFont(const std::string& name);

        static std::string& getDefaultFont();

        static std::vector<std::string> fontNames();

        static void setFontDir(const System::Path& path);

        CGContextRef context() const
        { return _context; }

    private:
        void create();
    
        void destroy();

        void beginClip();

        void endClip();
    
    private:
        Gfx::SizeF     _size;
        PaintData*     _paintData;
        Gfx::Painter*  _painter;
        CGContextRef   _context;
        CGRect         _clipRect;
};

} // namespace

} // namespace

#endif // include guard
