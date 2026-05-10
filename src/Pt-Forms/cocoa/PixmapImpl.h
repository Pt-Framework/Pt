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

#ifndef PT_FORMS_COCOA_PIXMAP_IMPL_H
#define PT_FORMS_COCOA_PIXMAP_IMPL_H

#include <Pt/Gfx/FontFace.h>
#include <Pt/Gfx/PaintSurface.h>
#include <Pt/Gfx/Canvas.h>

#include <Pt/Gfx/Path.h>
#include <Pt/System/Path.h>

#include <CoreGraphics/CGBitmapContext.h>
#include <CoreText/CoreText.h>

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

namespace Forms {

class Canvas;
class Pixmap;
class PixmapImpl;

class PixmapCanvas : public Gfx::Canvas
{
    public:
        PixmapCanvas();            

        virtual ~PixmapCanvas();

        void setPixmap(PixmapImpl& pixmap);

        void suspend();

        void resume();

        void drawPixmap(const Gfx::PointF& to,
                        const Pixmap& pm,
                        const Gfx::RectF* rect);
    
    protected:
        virtual void onBeginPaint(const Gfx::Paint& paint) override;

        virtual void onFinishPaint() override;

    protected:
        virtual void onSetTransform(const Gfx::Transform& tx) override;

        virtual void onApplyTransform() override;

        virtual void onSetCompositionMode(const Gfx::CompositionMode& mode) override;
        
        virtual void onApplyCompositionMode() override;

        virtual void onSetPen(const Gfx::Pen& pen) override;

        virtual void onApplyPen() override;

        virtual void onSetBrush(const Gfx::Brush& brush) override;

        virtual void onApplyBrush() override;

        virtual void onSetFont(const Gfx::Font& font) override;

        virtual void onApplyFont() override;

        virtual void onSetClip(const Gfx::RectF* clip) override;

        virtual void onApplyClip() override;

    protected:
        virtual void onDrawLine(const Gfx::PointF& from, 
                                const Gfx::PointF& to) override;

        virtual void onDrawPolyline(const Gfx::PointF* pts, const size_t n) override;

        virtual void onFillPolygon(const Gfx::PointF* pts, const size_t n) override;

        virtual void onDrawRect(const Gfx::RectF& rect) override;

        virtual void onFillRect(const Gfx::RectF& rect) override;

        virtual void onDrawEllipse(const Gfx::PointF& topLeft, 
                                   const Gfx::SizeF& size) override;

        virtual void onFillEllipse(const Gfx::PointF& topLeft, 
                                   const Gfx::SizeF& size) override;

    protected:
        virtual Gfx::TextMetrics onGetTextMetrics(const Pt::String& text) const override;

        virtual const Gfx::FontMetrics& onGetFontMetrics() const override;

        virtual void onDrawText(const Gfx::PointF& to, const Pt::String& text, 
                                const Gfx::Transform* trans) override;

    protected:
        virtual void onDrawImage(const Gfx::PointF& to, 
                                 const Gfx::Image& image, 
                                 const Gfx::RectF* rect) override;

    protected:
        virtual void onSetPath(const Gfx::Path& path) override;

        virtual void onDrawPath() override;

        virtual void onFillPath() override;

        virtual void onDrawPath(const Gfx::Path& path) override;

        virtual void onFillPath(const Gfx::Path& path) override;

    private:
        CGMutablePathRef makePath(const Gfx::Path& path);

    private:
        PixmapImpl*             _pixmap;
        Gfx::Transform          _transform;
        Gfx::CompositionMode    _compositionMode;
        CGRect                  _clipRect;

        CGColorRef              _penColor;
        CGFloat                 _penSize;
        CGLineCap               _penCap;
        CGLineJoin              _penJoin;
        std::vector<CGFloat>    _dashes;

        CGColorRef              _brushColor;
        Gfx::Brush::FillStyle   _brushStyle;

        CTFontRef                     _font;
        Gfx::FontMetrics              _fontMetrics;
        CFMutableDictionaryRef        _fontAttributes;
        CFMutableAttributedStringRef  _attributedString;
        
        CGMutablePathRef        _cgPath;
};


class PixmapImpl
{
    public:
        PixmapImpl();

        virtual ~PixmapImpl();

        void reset(const Gfx::Image& image);

        void reset(const Gfx::SizeF& size);

        void reset();
        
        void getBitmap(Gfx::Bitmap& bitmap, const Gfx::RectF& rect) const;
        
        void setScaleFactor(double scaleFactor);
    
        const Gfx::ImageFormat& format() const;

        const Gfx::SizeF& size() const;

        const Gfx::Scaling& scaling() const;

        void drawPixmap(Gfx::Canvas& canvas,
                        const Gfx::PointF& to,
                        const Pixmap& pm,
                        const Gfx::RectF* rect);

        Gfx::Canvas* getCanvas(Gfx::Canvas* reuse)
        {
            return 0;
        }

        Gfx::Canvas* createCanvas(Gfx::Canvas* reuse);

        void releaseCanvas();

        void sync();

        void finish();

    public:
        CGContextRef context() const;

        CGImageRef getCGImage() const;

    public:
        static const std::string& defaultFont();

        static void setDefaultFont(const std::string& family);

        static std::vector<std::string> fontFamilies();

        static std::vector<Gfx::FontFace> fontFaces(const std::string& family);

        void create();
    
        void destroy();

    private:
        Gfx::SizeF          _physicalSize;
        Gfx::Scaling        _scaling;
        size_t              _width;
        size_t              _height;

        CGContextRef        _context;
        mutable CGImageRef  _image;

        PixmapCanvas*       _canvas;
};

} // namespace

} // namespace

#endif // include guard
