/* Copyright (C) 2015 Laurentiu-Gheorghe Crisan
   Copyright (C) 2015-2024 Marc Boris Duerner

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

#ifndef Pt_Forms_Pixmap_h
#define Pt_Forms_Pixmap_h

#include <Pt/Forms/Api.h>
#include <Pt/Forms/PaintSurface.h>
#include <Pt/Gfx/FontFace.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/Bitmap.h>
#include <Pt/System/Path.h>

namespace Pt {

namespace Forms {

/** @brief Platform pixmap implementation.

    %GraphicsBackend creates this object. Ordinary applications do not
    call this API.

    @ingroup Pt-Forms-Updating
*/
class PT_FORMS_API IPixmapImpl
{
    public:
        /** @brief Destructor.
        */
        virtual ~IPixmapImpl() {}

        /** @brief Resets the pixmap from @a image.
        */
        virtual void reset(const Gfx::Image& image) = 0;

        /** @brief Resets the pixmap to @a size in physical pixels.
        */
        virtual void reset(const Gfx::SizeF& size) = 0;

        /** @brief Clears the pixmap.
        */
        virtual void reset() = 0;

        /** @brief Copies @a rect into @a bitmap.
        */
        virtual void getBitmap(Gfx::Bitmap& bitmap, const Gfx::RectF& rect) const = 0;

        /** @brief Sets the scale factor to @a scaleFactor.
        */
        virtual void setScaleFactor(double scaleFactor) = 0;

        /** @brief Draws @a pm onto @a canvas at @a to.

            When @a rect is not null, only that region of the pixmap is
            drawn.
        */
        virtual void drawPixmap(Gfx::Canvas& canvas, const Gfx::PointF& to,
                                const Pixmap& pm, const Gfx::RectF* rect) = 0;

        /** @brief Returns the image format.
        */
        virtual const Gfx::ImageFormat& format() const = 0;

        /** @brief Returns the size in physical pixels.
        */
        virtual const Gfx::SizeF& size() const = 0;

        /** @brief Returns the scaling from logical to physical pixels.
        */
        virtual const Gfx::Scaling& scaling() const = 0;

        /** @brief Returns a canvas, reusing @a reuse when possible.
        */
        virtual Gfx::Canvas* getCanvas(Gfx::Canvas* reuse) = 0;

        /** @brief Creates a canvas, reusing @a reuse when possible.
        */
        virtual Gfx::Canvas* createCanvas(Gfx::Canvas* reuse) = 0;

        /** @brief Releases the current canvas.
        */
        virtual void releaseCanvas() = 0;

        /** @brief Synchronizes pending drawing operations.
        */
        virtual void sync() = 0;

        /** @brief Finishes painting on the pixmap.
        */
        virtual void finish() = 0;
};


/** @brief Off-screen Forms paint surface.

    A %Pixmap is a %PaintSurface used as a back buffer or as cached image
    content. Reset it from an image or a size in physical pixels.

    @ingroup Pt-Forms-Updating
*/
class PT_FORMS_API Pixmap : public PaintSurface
{
    public:
        /** @brief Creates an empty pixmap.
        */
        Pixmap();

        /** @brief Destructor.
        */
        virtual ~Pixmap();

        /** @brief Clears the pixmap.
        */
        void reset();

        /** @brief Resets the pixmap from @a image.
        */
        void reset(const Gfx::Image& image);

        /** @brief Resets the pixmap to @a size in physical pixels.
        */
        void reset(const Gfx::SizeF& size);

        /** @brief Returns true when the pixmap has no size.
        */
        bool empty() const;

        /** @brief Copies @a rect into @a image.
        */
        void getBitmap(Gfx::Bitmap& image, const Gfx::RectF& rect);

        /** @brief Sets the scale factor to @a v.
        */
        void setScaleFactor(double v);

    protected:
        virtual void onDrawPixmap(Gfx::Canvas& canvas,
                                  const Gfx::PointF& to,
                                  const Pixmap& pixmap,
                                  const Gfx::RectF* rect = 0) override;

    protected:
        virtual const Gfx::ImageFormat& onGetFormat() const override;

        virtual const Gfx::SizeF& onGetSize() const override;

        virtual const Gfx::Scaling& onGetScaling() const override;

        virtual Gfx::Canvas* onGetCanvas(Gfx::Canvas* reuse) override;

        virtual Gfx::Canvas* onCreateCanvas(Gfx::Canvas* reuse) override;

        virtual void onReleaseCanvas() override;

        virtual void onSync() override;

        virtual void onFinish() override;

    public:
        /** @brief Returns the platform pixmap implementation.

            This is an implementation API for Forms backends.
        */
        IPixmapImpl* impl()
        {
            return _impl;
        }

        /** @brief Returns the platform pixmap implementation.

            This is an implementation API for Forms backends.
        */
        const IPixmapImpl* impl() const
        {
            return _impl;
        }

    private:
        IPixmapImpl* _impl;
};

} // namespace

} // namespace

#endif
