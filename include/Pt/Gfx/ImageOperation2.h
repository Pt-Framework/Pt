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
        static inline void blockScale(const Image& from, Image& to);

        template <typename ModelT, typename ViewT>
        static inline void blockScale(const BasicImage<ModelT, ViewT>& from, BasicImage<ModelT, ViewT>& to);

    private:
        static void blockScale32Bpp(
                  Pt::uint32_t* dst, Pt::ssize_t dstS, Pt::ssize_t dstW, Pt::ssize_t dstH,
            const Pt::uint32_t* src, Pt::ssize_t srcS, Pt::ssize_t srcW, Pt::ssize_t srcH
        );

        static void bilinearScale32Bpp(
                  Pt::uint32_t* dst, Pt::ssize_t dstS, Pt::ssize_t dstW, Pt::ssize_t dstH,
            const Pt::uint32_t* src, Pt::ssize_t srcS, Pt::ssize_t srcW, Pt::ssize_t srcH
        );

        static void blockRotate32Bpp(
                  Pt::uint32_t* dst, Pt::ssize_t dstS, Pt::ssize_t dstW, Pt::ssize_t dstH,
            const Pt::uint32_t* src, Pt::ssize_t srcS, Pt::ssize_t srcW, Pt::ssize_t srcH,

            float deg, const Color& cfill, ImageRotateMode rm
        );

};


inline void ImageOperation2::blockScale(const Image& from, Image& to)
{
}

template <typename ModelT, typename ViewT>
inline void ImageOperation2::blockScale(const BasicImage<ModelT, ViewT>& from, BasicImage<ModelT, ViewT>& to)
{
}


} // namespace
} // namespace


#endif
