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

#ifndef PT_GFX_BITMAP_H
#define PT_GFX_BITMAP_H

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/FontFace.h>
#include <Pt/Gfx/PaintSurface.h>
#include <Pt/Gfx/Paint.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/System/Path.h>

#include <string>
#include <vector>
#include <cstddef>

namespace Pt {

namespace Gfx {

class BitmapSurface;

/** @brief Off-screen drawing surface backed by an image.
    @ingroup Pt-Gfx-Drawing

    %Bitmap is the standard in-memory %PaintSurface implementation. It can be
    used as a rendering target for %Painter and also exposes the rendered image
    so that drawing results can be copied, inspected or reused by image APIs.
*/
class PT_GFX_API Bitmap : public PaintSurface
{
    public:
        /** @brief Constructs an empty bitmap.
        */
        Bitmap();

        /** @brief Constructs a bitmap with the given size and optional stride.
        */
        Bitmap(const Gfx::SizeF& size, std::size_t stride = 0);

        /** @brief Destroys the bitmap.
        */
        virtual ~Bitmap();

        /** @brief Resets to empty state.
        */
        void reset();

        /** @brief Replaces the bitmap with an existing image.
        */
        void reset(const Gfx::Image& image);

        /** @brief Resets the bitmap to a new size in physical pixels.
        */
        void reset(const Gfx::SizeF&, std::size_t stride = 0);

        /** @brief Returns true if no image data is available.
        */
        bool empty() const;

        /** @brief Returns the underlying image.
        */
        const Gfx::Image& image() const;

        /** @brief Sets the target scale factor.
        */
        void setScaleFactor(double scaleFactor);

        /** @brief Draws another bitmap into this bitmap.
        */
        void drawBitmap(const Pt::Gfx::PointF& to, const Bitmap& image,
                        const Gfx::Paint& paint, const Gfx::RectF* rect = 0);

    protected:
        virtual const Gfx::ImageFormat& onGetFormat() const;

        virtual const Gfx::SizeF& onGetSize() const;

        virtual const Scaling& onGetScaling() const;

    protected:
        virtual Gfx::Canvas* onCreateCanvas(Gfx::Canvas* reuse) override;

        virtual void onReleaseCanvas() override;

        virtual void onSync() override;

        virtual void onFinish() override;

    public:
        /** @brief Returns the default font family used by the backend.
        */
        static const std::string& defaultFont();

        /** @brief Sets the default font family used by the backend.
        */
        static void setDefaultFont(const std::string& family);

        /** @brief Returns the available font families.
        */
        static std::vector<std::string> fontFamilies();

        /** @brief Returns the font faces of a family.
        */
        static std::vector<FontFace> fontFaces(const std::string& family);

    private:
        BitmapSurface* _surface;
};

} // namespace

} // namespace

#endif

