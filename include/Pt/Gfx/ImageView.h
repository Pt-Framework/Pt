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
#include <Pt/Gfx/ImageFormat.h>
#include <Pt/Gfx/BasicView.h>
#include <Pt/Types.h>

namespace Pt {

namespace Gfx {

typedef BasicPixel<ImageFormat, 
                   ImageFormat::Pixel> Pixel;

typedef BasicConstPixel<ImageFormat, 
                        ImageFormat::ConstPixel> ConstPixel;

/** @brief View on image data.
*/
class ImageView : public BasicView<ImageFormat>
{
    public:
        ImageView()
        : BasicView( ImageFormat::argb32() )
        { }

        explicit ImageView(const ImageFormat& format)
        : BasicView(format)
        { }

        ImageView(const ImageFormat& format, Pt::uint8_t* data,
                  Pt::ssize_t width, Pt::ssize_t height, Pt::ssize_t padding)
        : BasicView(format, data, width, height, padding)
        { 
        }

        virtual ~ImageView()
        { }

        void getSpan(Pt::ssize_t x, Pt::ssize_t y, 
                     Pt::uint32_t* to, Pt::ssize_t n)
        {
            format().getSpan(*this, Pixel(*this, x, y), to, n);
        }

        //void getPixels(Pt::ssize_t x, Pt::ssize_t y, 
        //               Pt::ssize_t w, Pt::ssize_t h, 
        //               Pt::uint32_t* to)
        //{
        //    //Pt::ssize_t left  = std::max( Pt::ssize_t(0), x );
        //    //Pt::ssize_t top  = std::max( Pt::ssize_t(0), y );
        //    //Pt::ssize_t right  = std::min( width(), x + w );
        //    //Pt::ssize_t bottom  = std::min( height(), y + h );

        //    //if (right <= left || bottom <= top)
        //    //    return;

        //    //Pt::ssize_t length = right - left;

        //    Pt::ssize_t left = x;
        //    Pt::ssize_t top = y;
        //    Pt::ssize_t bottom = y + h;
        //    Pt::ssize_t length = w;
        //    
        //    for(Pt::ssize_t n = top; n < bottom; ++n)
        //    {
        //        format().getSpan(*this, Pixel(*this, left, n), to, length);
        //        to += ???;
        //    }
        //}
};

} // namespace

} // namespace

#endif
