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

#ifndef PT_GFX_COMPOSE_H
#define PT_GFX_COMPOSE_H

#include <Pt/Gfx/Api.h>
#include <cstddef>

namespace Pt {

namespace Gfx {

/** @brief Tag type for source-copy composition.

    Replaces the destination pixel with the source pixel.
*/
struct SourceCopy {};

/** @brief Tag type for source-over composition.

    Blends the source pixel over the destination using alpha.
*/
struct SourceOver {};

constexpr SourceCopy sourceCopy{};
constexpr SourceOver sourceOver{};

/** @brief Compose a single source pixel onto a destination pixel.

    Overloads for concrete pixel types are provided in the format
    headers (e.g. Rgb32.h, Argb32.h). Calling compose for a pixel
    type without a matching overload is a compile-time error.
*/
template <typename Pixel, typename Src>
void compose(SourceCopy, Pixel& to, const Src& from) = delete;

template <typename Pixel, typename Src>
void compose(SourceOver, Pixel& to, const Src& from) = delete;

/** @brief Fill n destination pixels with a single source pixel.
*/
template <typename Pixel, typename Src>
void compose(SourceCopy, Pixel& to, std::size_t n, const Src& from) = delete;

template <typename Pixel, typename Src>
void compose(SourceOver, Pixel& to, std::size_t n, const Src& from) = delete;

/** @brief Compose n source pixels onto n destination pixels.
*/
template <typename Pixel, typename Src>
void compose(SourceCopy, Pixel& to, const Src& from, std::size_t n) = delete;

template <typename Pixel, typename Src>
void compose(SourceOver, Pixel& to, const Src& from, std::size_t n) = delete;

} // namespace

} // namespace

#endif
