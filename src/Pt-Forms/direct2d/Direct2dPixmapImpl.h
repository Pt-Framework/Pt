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

#ifndef PT_FORMS_DIRECT2D_DIRECT2DPIXMAPIMPL_H
#define PT_FORMS_DIRECT2D_DIRECT2DPIXMAPIMPL_H

#include <Pt/Forms/Api.h>
#include <Pt/Forms/Pixmap.h>
#include <Pt/Forms/PaintSurface.h>
#include <Pt/Gfx/FontFace.h>
#include <Pt/Gfx/PaintSurface.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/Gfx/Color.h>
#include <Pt/Gfx/Paint.h>
#include <Pt/Gfx/Path.h>
#include <Pt/System/Path.h>

#include <vector>

#include <Windows.h>
#include <d2d1_1.h>

namespace Pt {

namespace Forms {

class Direct2dPixmapCanvas;

class Direct2dPixmapImpl : public IPixmapImpl
{
    public:
        Direct2dPixmapImpl();

        virtual ~Direct2dPixmapImpl();

        void reset(const Gfx::Image& image) override;

        void reset(const Gfx::SizeF& size) override;

        void reset() override;

        Gfx::Image toImage() const;

        void getBitmap(Gfx::Bitmap& bitmap, const Gfx::RectF& rect) const override;

        void setScaleFactor(double scaleFactor) override;

        ID2D1Bitmap1* bitmap() const
        { return _d2dBitmap; }

        LONG width() const
        { return _width; }

        LONG height() const
        { return _height; }

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

        void sync() override;

        void finish() override;

    private:
        void createBitmap(LONG width, LONG height);

        void destroyBitmap();

    private:
        Gfx::SizeF            _physicalSize;
        Gfx::Scaling          _scaling;

        LONG                  _width;
        LONG                  _height;
        ID2D1Bitmap1*         _d2dBitmap;

        Direct2dPixmapCanvas* _canvas;
};

} // namespace

} // namespace

#endif
