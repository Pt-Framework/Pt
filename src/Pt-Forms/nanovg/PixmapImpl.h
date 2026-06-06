/* Copyright (C) 2026 Marc Boris Duerner

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
#include <Pt/Gfx/FontFace.h>
#include <Pt/Gfx/Bitmap.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/ImageFormat.h>
#include <Pt/Gfx/Scaling.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Rect.h>

#include <string>
#include <vector>

struct NVGLUframebuffer;

namespace Pt {

namespace Forms {

class Pixmap;
class PixmapCanvas;

// Back buffer for the nanovg renderer. Pixels live in an offscreen GLES2
// framebuffer object that is rendered into by a PixmapCanvas and composited
// onto windows with the shared nanovg context.
class PixmapImpl
{
    public:
        PixmapImpl();

        ~PixmapImpl();

        void reset(const Gfx::Image& image);

        void reset(const Gfx::SizeF& size);

        void reset();

        Gfx::Image toImage() const;

        void getBitmap(Gfx::Bitmap& bitmap, const Gfx::RectF& rect) const;

        void setScaleFactor(double scaleFactor);

        // nanovg image handle of the framebuffer, or -1 if empty. Used by
        // windows to composite the pixmap.
        int framebufferImage() const;

        // Framebuffer object for binding by the canvas, or 0 if empty.
        NVGLUframebuffer* framebuffer() const
        { return _fb; }

        int width() const
        { return _width; }

        int height() const
        { return _height; }

        void drawPixmap(Gfx::Canvas& canvas,
                        const Gfx::PointF& to,
                        const Pixmap& pm,
                        const Gfx::RectF* rect);

        const Gfx::ImageFormat& format() const;

        const Gfx::SizeF& size() const;

        const Gfx::Scaling& scaling() const;

        Gfx::Canvas* getCanvas(Gfx::Canvas* reuse)
        {
            return 0;
        }

        Gfx::Canvas* createCanvas(Gfx::Canvas* reuse);

        void releaseCanvas();

        void sync()
        {}

        void finish()
        {}

    public:
        static const std::string& defaultFont()
        {
            return Gfx::Bitmap::defaultFont();
        }

        static void setDefaultFont(const std::string& family)
        {
            Gfx::Bitmap::setDefaultFont(family);
        }

        static std::vector<std::string> fontFamilies()
        {
            return Gfx::Bitmap::fontFamilies();
        }

        static std::vector<Gfx::FontFace> fontFaces(const std::string& family)
        {
            return Gfx::Bitmap::fontFaces(family);
        }

    private:
        void createFramebuffer(int width, int height);

        void destroyFramebuffer();

    private:
        Gfx::SizeF        _physicalSize;
        Gfx::Scaling      _scaling;

        int               _width;
        int               _height;
        NVGLUframebuffer* _fb;

        PixmapCanvas*     _canvas;
};

} // namespace

} // namespace

#endif
