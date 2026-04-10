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

class PixmapImpl;

/** @brief Back buffer drawing surface.
*/
class PT_FORMS_API Pixmap : public PaintSurface
{
    public:
        Pixmap();

        virtual ~Pixmap();

        void reset();

        /** @brief Resets to an image. 
        */
        void reset(const Gfx::Image& image);

        /** @brief Resizes to a size in physical pixels. 
        */
        void reset(const Gfx::SizeF& size);

        bool empty() const;

        void getBitmap(Gfx::Bitmap& image, const Gfx::RectF& rect);

        void setScaleFactor(double v);

    protected:
        virtual void onDrawPixmap(const Gfx::PointF& to,
                                  const Pixmap& pixmap,
                                  const Gfx::Paint& paint,
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
        static std::string defaultFont();

        static void setDefaultFont(const std::string& family);

        static std::vector<std::string> fontFamilies();

        static std::vector<Gfx::FontFace> fontFaces(const std::string& family);

    public:
        PixmapImpl* impl()
        {
            return _impl;
        }

        const PixmapImpl* impl() const
        {
            return _impl;
        }
    
    private:
        PixmapImpl* _impl;
};

typedef Pixmap PixmapSurface;

} // namespace

} // namespace

#endif
