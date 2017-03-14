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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
  02110-1301 USA
*/

#include <Pt/Gfx/Argb32Format.h>
#include <Pt/Gfx/Argb32Image.h>
#include <Pt/Gfx/ImageView.h>

namespace Pt {
namespace Gfx {


Argb32Format::Argb32Format()
: ImageFormat(4)
{
}


std::size_t Argb32Format::onImageSize(const Size& size, Pt::ssize_t padding) const
{
    std::size_t l = (size.width() * 4) + padding;
    std::size_t n = l * size.height();
    return n;
}


void Argb32Format::onSetPixel(Pixel& to, const Pixel& from,
                              CompositionMode mode) const
{
    Argb32Model::assign(to.base(), from.base(), mode);
}


void Argb32Format::onSetPixel(Pixel& to, const ConstPixel& from,
                                CompositionMode mode) const
{
    Argb32Model::assign(to.base(), from.base(), mode);
}


void Argb32Format::onSetPixel(Pixel& pixel, const Color& c,
                              CompositionMode mode) const
{
    Argb32Model::assign(pixel.base(), c, mode);
}


void Argb32Format::onSetPixel(Pixel& to, const Pixel& from,
                              CompositionMode mode, Pt::uint8_t blendingAlpha) const
{
    Argb32Model::assign(to.base(), from.base(), mode, blendingAlpha);
}


void Argb32Format::onSetPixel(Pixel& to, const ConstPixel& from,
                                CompositionMode mode, Pt::uint8_t blendingAlpha) const
{
    Argb32Model::assign(to.base(), from.base(), mode, blendingAlpha);
}


void Argb32Format::onSetPixel(Pixel& pixel, const Color& c,
                              CompositionMode mode, Pt::uint8_t blendingAlpha) const
{
    Argb32Model::assign(pixel.base(), c, mode, blendingAlpha);
}


void Argb32Format::onSetPixels(Pixel& to, const Pixel& from, size_t length,
                               CompositionMode mode) const
{
    Argb32Model::assign(to.base(), from.base(), length, mode);
}


void Argb32Format::onSetPixels(Pixel& to, const ConstPixel& from, size_t length,
                               CompositionMode mode) const
{
    Argb32Model::assign(to.base(), from.base(), length, mode);
}


void Argb32Format::onSetPixels(Pixel& pixel, const Color& c, size_t length,
                               CompositionMode mode) const
{
    Argb32Model::assign(pixel.base(), c, length, mode);
}

Color Argb32Format::onGetColor(const Pixel& pixel) const
{
    return Argb32Model::toColor( pixel.base() );
}


Color Argb32Format::onGetColor(const ConstPixel& pixel) const
{
    return Argb32Model::toColor( pixel.base() );
}


void Argb32Format::onCopy(Pixel& to, const Pixel& from, size_t length,
                          CompositionMode mode) const
{
    Pt::uint8_t* dst = to.base();;
    const Pt::uint8_t* src = from.base();

    switch(mode)
    {
        default:
        case CompositionMode::SourceCopy: {
#ifdef SSE2
            const size_t   len4     = length / 4;
            const __m128i* srcvARGB = reinterpret_cast<const __m128i*>(src);
                  __m128i* dstvARGB = reinterpret_cast<      __m128i*>(dst);
            for(size_t i = 0; i < len4; ++i) {
                _mm_prefetch(srcvARGB + 1, _MM_HINT_T0);
                _mm_storeu_si128(dstvARGB, _mm_loadu_si128(srcvARGB));
                ++srcvARGB;
                ++dstvARGB;
            }
            length -= (len4 * 4);

            memcpy(dstvARGB, srcvARGB, length * 4);

#else
            memcpy(dst, src, length * 4);
#endif
            break;
        }

        case CompositionMode::SourceOver:
            for(size_t i = 0; i < length; ++i) {
                Argb32Model::sourceOver(dst, src);
                src += 4;
                dst += 4;
            }
            break;
    }
}


void Argb32Format::onCopy(Pixel& to, const ConstPixel& from, size_t length,
                          CompositionMode mode) const
{
          Pt::uint8_t* dst = to  .base();
    const Pt::uint8_t* src = from.base();

    switch(mode)
    {
        default:
        case CompositionMode::SourceCopy: {
#ifdef SSE2
            const size_t   len4     = length / 4;
            const __m128i* srcvARGB = reinterpret_cast<const __m128i*>(src);
                  __m128i* dstvARGB = reinterpret_cast<      __m128i*>(dst);
            for(size_t i = 0; i < len4; ++i) {
                _mm_prefetch(srcvARGB + 1, _MM_HINT_T0);
                _mm_storeu_si128(dstvARGB, _mm_loadu_si128(srcvARGB));
                ++srcvARGB;
                ++dstvARGB;
            }
            length -= (len4 * 4);

            memcpy(dstvARGB, srcvARGB, length * 4);

#else
            memcpy(dst, src, length * 4);
#endif
            break;
        }

        case CompositionMode::SourceOver:
#ifdef SSE2_CUK
            const size_t   len4     = length / 4;
          //  const __m128i* srcvARGB = reinterpret_cast<__m128i*>(src);
           //       __m128i* dstvARGB = reinterpret_cast<__m128i*>(dst);
            for(size_t i = 0; i < len4; ++i) {
                // Load 4 pixels
              //  srcvARGB = _mm_loadu_si128 (srcvARGB          );
              //  dstvABGR = _mm_loadu_si128 (dstvARGB          );
                // Increment the pointers
            //    ++srcvARGB;
              //  ++dstvARGB;
            }
            length -= (len4 * 4);
            /*
            const size_t   len4     = length / 4;
            const __m128i  srcvAGAG = _mm_set_epi16(srcA, srcG, srcA, srcG, srcA, srcG, srcA, srcG);
            const __m128i  srcvRBRB = _mm_set_epi16(srcR, srcB, srcR, srcB, srcR, srcB, srcR, srcB);
            const __m128i  srcv0A0A = _mm_set_epi16(0, blendInv, 0, blendInv, 0, blendInv, 0, blendInv);
                  __m128i* dstvARGB = reinterpret_cast<__m128i*>(to);
                  __m128i  dstv4PIX;
                  __m128i  dstv0A0G;
                  __m128i  dstv0R0B;
            for(size_t i = 0; i < len4; ++i) {
                // Load 4 pixels
                dstv4PIX = _mm_loadu_si128 (dstvARGB          );
                // Process A and G
                dstv0A0G = _mm_and_si128   (dstv4PIX, maskA0G0);
                dstv0A0G = _mm_srli_epi16  (dstv0A0G, 8       );
                dstv0A0G = _mm_mullo_epi16 (dstv0A0G, srcv0A0A);
                dstv0A0G = _mm_add_epi16   (dstv0A0G, srcvAGAG);
                dstv0A0G = _mm_and_si128   (dstv0A0G, maskA0G0);
                // Prefecth the next 4 pixels
                _mm_prefetch(dstvARGB + 1, _MM_HINT_T0);
                // Process R and B
                dstv0R0B = _mm_and_si128   (dstv4PIX, mask0B0R);
                dstv0R0B = _mm_mullo_epi16 (dstv0R0B, srcv0A0A);
                dstv0R0B = _mm_add_epi16   (dstv0R0B, srcvRBRB);
                dstv0R0B = _mm_srli_epi16  (dstv0R0B, 8       );
                dstv0R0B = _mm_and_si128   (dstv0R0B, mask0B0R);
                // Store 4 pixels
                dstv4PIX = _mm_or_si128    (dstv0A0G, dstv0R0B);
                           _mm_storeu_si128(dstvARGB, dstv4PIX);
                // Increment the destination pointer
                ++dstvARGB;
            }
            length -= (len4 * 4);
            Pt::uint8_t* dst = reinterpret_cast<Pt::uint8_t*>(dstvARGB);
            */
#else
            for(size_t i = 0; i < length; ++i) {
                Argb32Model::sourceOver(dst, src);
                src += 4;
                dst += 4;
            }
#endif
            break;
    }
}


void Argb32Format::onCopy(ImageView& to, const Point& toPoint,
                          const ImageView& from, const Rect& fromRect,
                          CompositionMode mode) const
{
    Pt::ssize_t pixelSize = 4;

    // TODO: equals to toInfo.pitch()
    Pt::ssize_t toStride = (to.width() * pixelSize) + to.padding();
    Pt::ssize_t fromStride = (from.width() * pixelSize) + from.padding();

    Pt::ssize_t toBegin = (toPoint.y() * toStride) + (toPoint.x() * pixelSize);
    Pt::ssize_t fromBegin = (fromRect.y() * fromStride) + (fromRect.x() * pixelSize);

    Pt::uint8_t* toLine = to.data() + toBegin;
    const Pt::uint8_t* fromLine = from.data() + fromBegin;

    switch(mode)
    {
        default:
        case CompositionMode::SourceCopy:
        {
            Pt::ssize_t n = fromRect.width() * pixelSize;

            for(Pt::ssize_t y = 0; y < fromRect.height(); ++y)
            {
                memcpy(toLine, fromLine, n);

                toLine += toStride;
                fromLine += fromStride;
            }

            break;
        }

        case CompositionMode::SourceOver:
        {
            for(int y = 0; y < fromRect.height(); ++y)
            {
                Pt::uint8_t* to = toLine;
                const Pt::uint8_t* from = fromLine;

                for(int x = 0; x < fromRect.width() ; ++x )
                {
                    Argb32Model::sourceOver(to, from);
                    to += 4;
                    from += 4;
                }

                toLine += toStride;
                fromLine += fromStride;
            }

            break;
        }
    }
}


} // namespace
} // namespace
