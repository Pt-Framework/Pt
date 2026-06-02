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

#include <CoreGraphics/CGBitmapContext.h>

namespace Pt {

namespace Forms {

class Pixmap;
class PixmapCanvas;


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

        void invalidateImage();

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
