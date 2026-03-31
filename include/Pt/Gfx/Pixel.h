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

#ifndef PT_GFX_PIXEL_H
#define PT_GFX_PIXEL_H

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/ViewBase.h>
#include <Pt/Types.h>

#ifdef __cpp_concepts
#include <concepts>
#endif

namespace Pt {

namespace Gfx {

#ifdef __cpp_concepts

/** @brief Requirements for a mutable pixel type.

    A PixelLike provides read/write access to pixel data in an image.
    It supports traversal, single and bulk color read/write, same-type
    pixel copy, and fill operations. The ColorType determines the
    intermediate color representation (e.g. Color for 8-bit, ColorF
    for 16-bit).

    The view constructor and reset are template members that accept
    any view type (BasicView or BasicImage). The concept can only
    verify the signature weakly using ViewBase as a witness type.
*/
template <typename P>
concept PixelLike = std::destructible<P> && requires {
    typename P::FormatType;
    typename P::ColorType;
} && requires(P p, const P cp,
              const typename P::ColorType& color,
              typename P::ColorType* colors,
              ViewBase& view,
              Pt::ssize_t n, std::size_t len)
{
    P(cp);
    P(view, n, n);

    { p = color } -> std::same_as<P&>;

    p.reset(cp);
    p.reset(view, n, n);

    { cp.view()  } -> std::same_as<const ViewBase&>;
    { cp.base()  } -> std::same_as<const Pt::uint8_t*>;
    { p.base()   } -> std::same_as<Pt::uint8_t*>;

    p.advance();
    p.advance(n);
    p.skipPadding();
    p.advanceLines(n);

    { cp.getColor() } -> std::same_as<typename P::ColorType>;
    cp.getColors(colors, len);

    p.assign(cp);
    p.assign(cp, len);
    p.assign(colors, len);

    p.fill(len, color);

    { cp.equals(cp) } -> std::same_as<bool>;
};

/** @brief Requirements for a const pixel type.

    A ConstPixelLike provides read-only access to pixel data in an image.
    It supports traversal and single/bulk color reading, but no writing.

    The view constructor and reset are template members that accept
    any view type (BasicView or BasicImage). The concept can only
    verify the signature weakly using ViewBase as a witness type.
*/
template <typename P>
concept ConstPixelLike = std::destructible<P> && requires {
    typename P::FormatType;
    typename P::ColorType;
} && requires(P p, const P cp,
              typename P::ColorType* colors,
              const ViewBase& cview, ViewBase& view,
              Pt::ssize_t n, std::size_t len)
{
    P(cp);
    P(cview, n, n);
    P(view, n, n);
    
    p.reset(cp);
    p.reset(cview, n, n);
    p.reset(view, n, n);

    { cp.view()  } -> std::same_as<const ViewBase&>;
    { cp.base()  } -> std::same_as<const Pt::uint8_t*>;

    p.advance();
    p.advance(n);
    p.skipPadding();
    p.advanceLines(n);

    { cp.getColor() } -> std::same_as<typename P::ColorType>;
    cp.getColors(colors, len);

    { cp.equals(cp) } -> std::same_as<bool>;
};

#endif // __cpp_concepts


template <typename T>
typename T::Traits::Pixel pixel(T& source, 
                                Pt::ssize_t x, Pt::ssize_t y)
{
    return typename T::Traits::Pixel(source, x, y);
}


template <typename T>
typename T::Traits::ConstPixel pixel(const T& source, 
                                    Pt::ssize_t x, Pt::ssize_t y)
{
    return typename T::Traits::ConstPixel(source, x, y);
}

} // namespace

} // namespace

#endif
