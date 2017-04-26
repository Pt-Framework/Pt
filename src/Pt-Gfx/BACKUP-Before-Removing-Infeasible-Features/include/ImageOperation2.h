/* Copyright (C) 2010-2016 Marc Boris Duerner
   Copyright (C) 2017-2017 by Aloysius Indrayanto

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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
  02110-1301 USA
*/

#ifndef PT_GFX_IMAGEOPERATION2_H
#define PT_GFX_IMAGEOPERATION2_H

#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/BasicImage.h>


namespace Pt {
namespace Gfx {


class PT_GFX_API ImageOperation2
{
    public:
        enum ImageRotateMode {
            RotateCrop,
            RotateNoCrop,
            RotateFit
        };

    public:

        //
        // Scale
        //

        template <typename ImageT>
        static inline void blockScale(const ImageT& from, ImageT& to);

        template <typename ImageT>
        static inline void bilinearScale(const ImageT& from, ImageT& to);

        //
        // Rotate
        //

        template <typename ImageT>
        static inline void blockRotate(
            const ImageT& from, ImageT& to, float deg,
            const Color& cfill = Color::fromRgb8(0, 0, 0, 255), ImageRotateMode irm = RotateCrop
        );

        template <typename ImageT>
        static inline void bilinearRotate(
            const ImageT& from, ImageT& to, float deg,
            const Color& cfill = Color::fromRgb8(0, 0, 0, 255), ImageRotateMode irm = RotateCrop
        );

    private:
        //
        // Scale 32 bits/pixel
        //

        static void blockScale32Bpp(
            const Pt::uint8_t* src, Pt::ssize_t srcS, Pt::ssize_t srcW, Pt::ssize_t srcH,
                  Pt::uint8_t* dst, Pt::ssize_t dstS, Pt::ssize_t dstW, Pt::ssize_t dstH
        );

        static void bilinearScale32Bpp(
            const Pt::uint8_t* src, Pt::ssize_t srcS, Pt::ssize_t srcW, Pt::ssize_t srcH,
                  Pt::uint8_t* dst, Pt::ssize_t dstS, Pt::ssize_t dstW, Pt::ssize_t dstH
        );

        //
        // Rotate 32 bits/pixel
        //

        static void blockRotate32Bpp(
            const Pt::uint8_t* src, Pt::ssize_t srcS, Pt::ssize_t srcW, Pt::ssize_t srcH,
                  Pt::uint8_t* dst, Pt::ssize_t dstS, Pt::ssize_t dstW, Pt::ssize_t dstH,
            float               deg,
            const Color&        cfill,
            ImageRotateMode     irm
        );

        static void bilinearRotate32Bpp(
            const Pt::uint8_t* src, Pt::ssize_t srcS, Pt::ssize_t srcW, Pt::ssize_t srcH,
                  Pt::uint8_t* dst, Pt::ssize_t dstS, Pt::ssize_t dstW, Pt::ssize_t dstH,
            float               deg,
            const Color&        cfill,
            ImageRotateMode     irm
        );
};


//
// Scale
//

template <typename ImageT>
static inline void blockScaleGeneric(const ImageT& from, ImageT& to)
{
    typename ImageT::PixelIterator fIter = const_cast<ImageT&>(from).begin();
    typename ImageT::PixelIterator tIter = to.begin();

    Pt::ssize_t dltH = 0;
    Pt::ssize_t yCtr = 0;

    while(yCtr < to.height()) {
        typename ImageT::PixelIterator sIter = fIter;
        do {
            Pt::ssize_t dltW = 0;
            for(Pt::ssize_t x = 0; x < to.width(); ++x) {
                *tIter = *fIter;
                ++tIter;
                for(dltW += from.width(); dltW >= to.width(); ++fIter, dltW -= to.width());
            }
            fIter = sIter;
            ++yCtr;
        }
        while((dltH += from.height()) < to.height());

        while(dltH >= to.height()) {
            fIter += from.width();
            dltH -= to.height();
        }
    }
}

template <typename ImageT>
inline void ImageOperation2::blockScale(const ImageT& from, ImageT& to)
{
    if(from.width() <= 0 || from.height() <= 0 || to.width() <= 0 || to.height() <= 0)
        throw std::runtime_error("invalid 'from' and/or 'to' image size");

    if(from.view().pixelStride() != 4 || to.view().pixelStride() != 4) {
        //throw std::runtime_error("block scale for images with pixel stride != 4 is not supported yet");
        blockScaleGeneric(from, to);
        return;
    }

    blockScale32Bpp(
        from.data(), from.view().stride(), from.width(), from.height(),
        to  .data(), to  .view().stride(), to  .width(), to  .height()
    );
}

template <typename ImageT>
inline void ImageOperation2::bilinearScale(const ImageT& from, ImageT& to)
{
    // ### TODO: Call the generic implementation function, if possible !!! ###

    if(from.width() <= 0 || from.height() <= 0 || to.width() <= 0 || to.height() <= 0)
        throw std::runtime_error("invalid 'from' and/or 'to' image size");

    if(from.view().pixelStride() != 4 || to.view().pixelStride() != 4)
        throw std::runtime_error("bilinear scale for images with pixel stride != 4 is not supported yet");

    bilinearScale32Bpp(
        from.data(), from.view().stride(), from.width(), from.height(),
        to  .data(), to  .view().stride(), to  .width(), to  .height()
    );
}


//
// Rotate
//

template <typename ImageT>
inline void ImageOperation2::blockRotate(const ImageT& from, ImageT& to, float deg, const Color& cfill, ImageRotateMode irm)
{
    // ### TODO: Call the generic implementation function, if possible !!! ###

    if(from.width() <= 0 || from.height() <= 0 || to.width() <= 0 || to.height() <= 0)
        throw std::runtime_error("invalid 'from' and/or 'to' image size");

    if(from.view().pixelStride() != 4 || to.view().pixelStride() != 4)
        throw std::runtime_error("block rotate for images with pixel stride != 4 is not supported yet");

    blockRotate32Bpp(
        from.data(), from.view().stride(), from.width(), from.height(),
        to  .data(), to  .view().stride(), to  .width(), to  .height(),
        deg, cfill, irm
    );
}

template <typename ImageT>
inline void ImageOperation2::bilinearRotate(const ImageT& from, ImageT& to, float deg, const Color& cfill, ImageRotateMode irm)
{
    // ### TODO: Call the generic implementation function, if possible !!! ###

    if(from.width() <= 0 || from.height() <= 0 || to.width() <= 0 || to.height() <= 0)
        throw std::runtime_error("invalid 'from' and/or 'to' image size");

    if(from.view().pixelStride() != 4 || to.view().pixelStride() != 4)
        throw std::runtime_error("bilinear rotate for images with pixel stride != 4 is not supported yet");

    bilinearRotate32Bpp(
        from.data(), from.view().stride(), from.width(), from.height(),
        to  .data(), to  .view().stride(), to  .width(), to  .height(),
        deg, cfill, irm
    );
}


} // namespace
} // namespace


#endif
