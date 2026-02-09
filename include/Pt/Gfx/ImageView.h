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

#ifndef PT_GFX_IMAGE_VIEW_H
#define PT_GFX_IMAGE_VIEW_H

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/ImageFormat.h>
#include <Pt/Gfx/BasicPixelView.h>
#include <Pt/Types.h>

namespace Pt {

namespace Gfx {

class ImageView;
class ConstImageView;
class PixelView;
class ColorView;

class PixelView : public BasicPixelView<ImageFormat, 
                                        Pixel<Argb32Color>,
                                        ConstPixel<Argb32Color> >
{
    public:
        PixelView()
        : BasicPixelView( ImageFormat::argb32() )
        { }

        explicit PixelView(Image& image)
        : BasicPixelView(image)
        { }

        template <typename OtherT>
        explicit PixelView(BasicImage<OtherT>& image)
        : BasicPixelView(image)
        { }
};


class ConstPixelView : public BasicConstPixelView<ImageFormat,
                                                  Pixel<Argb32Color>,
                                                  ConstPixel<Argb32Color> >
{
    public:
        ConstPixelView()
        : BasicConstPixelView( ImageFormat::argb32() )
        { }

        explicit ConstPixelView(const Image& image)
        : BasicConstPixelView(image)
        { }

        explicit ConstPixelView(const ConstImage& image)
        : BasicConstPixelView(image)
        { }
};


class ColorView : public BasicPixelView<ImageFormat, 
                                        Pixel<Color>,
                                        ConstPixel<Color> >
{
    public:
        ColorView()
        : BasicPixelView( ImageFormat::argb32() )
        { }

        explicit ColorView(Image& image)
        : BasicPixelView(image)
        { }
};


class ConstColorView : public BasicConstPixelView<ImageFormat,
                                                  Pixel<Color>,
                                                  ConstPixel<Color> >
{
    public:
        ConstColorView()
        : BasicConstPixelView( ImageFormat::argb32() )
        { }

        explicit ConstColorView(const Image& image)
        : BasicConstPixelView(image)
        { }

        explicit ConstColorView(const ConstImage& image)
        : BasicConstPixelView(image)
        { }
};

} // namespace

} // namespace

#endif
