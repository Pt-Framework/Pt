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

#ifndef PT_FORMS_NANOVG_NANOVGPIXMAPIMPL_H
#define PT_FORMS_NANOVG_NANOVGPIXMAPIMPL_H

#include <Pt/Forms/Api.h>
#include <Pt/Gfx/Bitmap.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/ImageFormat.h>
#include <Pt/Gfx/Scaling.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Rect.h>

#include "PaintCommand.h"

#include <Pt/Forms/Pixmap.h>

#include <string>
#include <vector>

namespace Pt {

namespace Forms {

class Pixmap;
class NanoVGPixmapCanvas;

// Back buffer for the nanovg renderer. Each pixmap is backed by a single
// nanovg image (GLES2 texture). Drawing commands are recorded into a
// PaintCommand buffer by PixmapCanvas and replayed lazily inside one
// nvgBeginFrame/nvgEndFrame via flush(), using the shared render-target FBO
// owned by NanoVGDevice. This eliminates nested nvg frames.
class NanoVGPixmapImpl : public IPixmapImpl
{
    public:
        NanoVGPixmapImpl();

        virtual ~NanoVGPixmapImpl();

        void reset(const Gfx::Image& image) override;

        void reset(const Gfx::SizeF& size) override;

        void reset() override;

        Gfx::Image toImage() const;

        void getBitmap(Gfx::Bitmap& bitmap, const Gfx::RectF& rect) const override;

        void setScaleFactor(double scaleFactor) override;

        // nanovg image handle of the backing texture, or -1 if empty.
        // Used by windows to composite the pixmap and by drawPixmap.
        int framebufferImage() const
        { return _image; }

        int width() const
        { return _width; }

        int height() const
        { return _height; }

        // Replay all pending PaintCommands into the backing texture using the
        // shared render-target FBO. No-op when the command buffer is empty.
        void flush();

        // Access to the command buffer for PixmapCanvas.
        std::vector<PaintCommand>& commands()
        { return _commands; }

        void drawPixmap(Gfx::Canvas& canvas,
                        const Gfx::PointF& to,
                        const Pixmap& pm,
                        const Gfx::RectF* rect) override;

        const Gfx::ImageFormat& format() const override;

        const Gfx::SizeF& size() const override;

        const Gfx::Scaling& scaling() const override;

        Gfx::Canvas* getCanvas(Gfx::Canvas* reuse) override
        {
            return 0;
        }

        Gfx::Canvas* createCanvas(Gfx::Canvas* reuse) override;

        void releaseCanvas() override;

        void sync() override
        {}

        void finish() override
        { flush(); }

    private:
        void createTexture(int width, int height);

        void destroyTexture();

    private:
        Gfx::SizeF                _physicalSize;
        Gfx::Scaling              _scaling;

        int                       _width;
        int                       _height;
        int                       _image;    // nanovg image handle, -1 if empty

        std::vector<PaintCommand> _commands;

        NanoVGPixmapCanvas*               _canvas;
};

} // namespace

} // namespace

#endif
