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

#ifndef Pt_Forms_PixmapImpl_h
#define Pt_Forms_PixmapImpl_h

#include <Pt/Forms/Api.h>

#ifndef PT_FORMS_X11_CORE
#define PT_FORMS_X11_RASTER 1
#endif

#ifdef PT_FORMS_X11_RASTER

#include <Pt/Gfx/Bitmap.h>

namespace Pt {

namespace Forms {

class Pixmap;

class PixmapImpl
{
    public:
        PixmapImpl()
        { }

        void reset(const Gfx::Image& image)
        {
            _bitmap.reset(image);
        }

        void reset(const Gfx::SizeF& size)
        {
            _bitmap.reset(size);
        }

        const Gfx::Bitmap& bitmap() const 
        {
            return _bitmap;
        }

        void getBitmap(Gfx::Bitmap& bitmap, const Gfx::RectF& rect) const
        {
            bitmap.reset( rect.size() );

            Gfx::Paint paint;
            bitmap.drawBitmap(Gfx::PointF(0, 0), _bitmap, paint, &rect);
        }

        const Gfx::SizeF& size() const
        {
            return _bitmap.size();
        }

        void setScaleFactor(double scaleFactor)
        {
            _bitmap.setScaleFactor(scaleFactor);
        }

        void drawPixmap(const Gfx::PointF& to,
                        const Pixmap& pixmap,
                        const Gfx::Paint& paint,
                        const Gfx::RectF* rect);
        
        const Gfx::ImageFormat& format() const
        {
            return _bitmap.format();
        }

        const Gfx::Scaling& scaling() const
        {
            return _bitmap.scaling();
        }

        Gfx::Canvas* getCanvas(Gfx::Canvas* reuse)
        {
            return _bitmap.getCanvas(reuse);
        }

        Gfx::Canvas* createCanvas(Gfx::Canvas* reuse)
        {
            return 0;
        }

        void releaseCanvas()
        {
        }

        void sync()
        {
            _bitmap.sync();
        }

        void finish()
        {
            _bitmap.finish();
        }

    public:
        static const std::string& defaultFont()
        {
            return Gfx::Bitmap::defaultFont();
        }

        static void setDefaultFont(const std::string& name)
        {
            Gfx::Bitmap::setDefaultFont(name);
        }

        static std::vector<std::string> fontNames()
        {
            return Gfx::Bitmap::fontNames();
        }
        
        static void setFontDir(const System::Path& path)
        {
            Gfx::Bitmap::setFontDir(path);
        }
    
    private:
        Gfx::Bitmap _bitmap;
};

} // namespace

} // namespace

#endif // PT_FORMS_X11_RASTER

#ifdef PT_FORMS_X11_CORE

#include <Pt/Gfx/Painter.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/Gfx/Font.h>
#include <Pt/Gfx/TextMetrics.h>
#include <Pt/Gfx/CompositionMode.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/Gfx/Point.h>
#include <Pt/System/Path.h>
#include <Pt/String.h>

#include <X11/X.h>
#include <X11/Xlib.h>

struct _XftFont;
struct _XftDraw;

namespace Pt {

namespace Forms {

class PaintData : public Gfx::PaintData
{
    public:
        PaintData();

        ~PaintData();

        void setPen(const Gfx::Pen& pen);

        GC& pen();

        void setBrush(const Gfx::Brush& brush);

        GC& brush();

        void setClip(const Gfx::RectF& rectF);

        void resetClip();

        void setFont(const Gfx::Font& font);

        _XftFont* font();

    protected:
        void create();
        
        void destroy();

        long toXColor(const Gfx::Color& color);

    private:
        GC         _penGc;
        GC         _brushGc;
        _XftFont*  _xftFont;
};

class PixmapSurface;

class PixmapSurfaceImpl
{
    public:
        PixmapSurfaceImpl();
        
        virtual ~PixmapSurfaceImpl();

        void clear(const Gfx::Color& c);

        void resize(const Pt::Gfx::SizeF& size);

        const Pt::Gfx::SizeF& size() const;

        void begin(Gfx::Painter& painter);  
        
        void finish();

        void setScaleFactor(double scaling)
        {
        }

        void setClip( const Gfx::RectF& clip);

        void resetClip();
         
        void setCompositionMode(const Gfx::CompositionMode& mode);

        const Gfx::ImageFormat& format() const;

        void setPen(const Gfx::Pen& pen);

        void setBrush(const Gfx::Brush& brush);

        void setFont(const Gfx::Font& font);

        Gfx::TextMetrics textMetrics(const Pt::String& text) const;

        void drawLine(const Gfx::PointF& from, const Gfx::PointF& to);

        void drawText(const Gfx::PointF& to, const Pt::String& text);

        void drawText(const Gfx::PointF& to, const Pt::String& text, const Gfx::Transform& trans)
        {
            drawText(to, text);
        }

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

        void drawImage(const Gfx::PointF& to, const Gfx::Image& image, const Gfx::RectF& imgRect);

        Gfx::Image toImage() const;

        void set(const Gfx::Image& image);

        static std::string defaultFont();

        static void setDefaultFont(const std::string& name);

        static std::string& getDefaultFont();

        static std::vector<std::string> fontNames();

        static void setFontDir(const System::Path& path);

        ::Drawable drawable()
        {
            return _drawable;
        }

    private:
        void create(const Pt::Gfx::SizeF& size);
        
        void destroy();

    private:
        Gfx::SizeF     _size;
        PaintData*     _paintData;
        Gfx::Painter*  _painter;
        ::Drawable     _drawable;
        _XftDraw*      _xftDraw;
};

} // namespace

} // namespace

#endif // PT_FORMS_X11_CORE

#endif
