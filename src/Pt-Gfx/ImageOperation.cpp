/* Copyright (C) 2017-2017 Aloysius Indrayanto
   Copyright (C) 2006-2015 Marc Boris Duerner
   Copyright (C) 2006-2015 Laurentiu-Gheorghe Crisan

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

#include <Pt/Gfx/ImageOperation.h>

#include "ImageScale4.h"
#include "ImageRotate4.h"


namespace Pt {
namespace Gfx {


void blockScale(Image& to, const Image& from)
{
    if(from.format().pixelStride() != 4)
        throw std::runtime_error("images with pixel stride != 4 are not supported yet");
    if(from.padding())
        throw std::runtime_error("images with padding != 0 are not supported yet");

    if(to.format() != from.format())
        throw std::runtime_error("source and destination image formats are different");
    if(to.empty())
        throw std::runtime_error("destination image is empty (not initialized)");

    blockScale4(
        from.begin(), from.width(), from.height(),
        to  .begin(), to  .width(), to  .height()
    );
}

void bilinearScale(Image& to, const Image& from)
{
    if(from.format().pixelStride() != 4)
        throw std::runtime_error("images with pixel stride != 4 are not supported yet");
    if(from.padding())
        throw std::runtime_error("images with padding != 0 are not supported yet");

    if(to.format() != from.format())
        throw std::runtime_error("source and destination image formats are different");
    if(to.empty())
        throw std::runtime_error("destination image is empty (not initialized)");

    bilinearScale4(
        from.begin(), from.width(), from.height(),
        to  .begin(), to  .width(), to  .height()
    );
}

void blockRotate(Image& to, const Image& from, float deg, const Color& colorFill, bool fullScale)
{
    if(from.format().pixelStride() != 4)
        throw std::runtime_error("images with pixel stride != 4 are not supported yet");
    if(from.padding())
        throw std::runtime_error("images with padding != 0 are not supported yet");

    if(to.format() != from.format())
        throw std::runtime_error("source and destination image formats are different");
    if(to.empty())
        throw std::runtime_error("destination image is empty (not initialized)");

    if(fullScale) {
        blockRotate4<true>(
            from.begin(), from.width(), from.height(),
            to  .begin(), to  .width(), to  .height(),
            deg,          colorFill
        );
    }
    else {
        blockRotate4<false>(
            from.begin(), from.width(), from.height(),
            to  .begin(), to  .width(), to  .height(),
            deg,          colorFill
        );
    }
}

void bilinearRotate(Image& to, const Image& from, float deg, const Color& colorFill, bool fullScale)
{
    if(from.format().pixelStride() != 4)
        throw std::runtime_error("images with pixel stride != 4 are not supported yet");
    if(from.padding())
        throw std::runtime_error("images with padding != 0 are not supported yet");

    if(to.format() != from.format())
        throw std::runtime_error("source and destination image formats are different");
    if(to.empty())
        throw std::runtime_error("destination image is empty (not initialized)");

    if(fullScale) {
        bilinearRotate4<true>(
            from.begin(), from.width(), from.height(),
            to  .begin(), to  .width(), to  .height(),
            deg,          colorFill
        );
    }
    else {
        bilinearRotate4<false>(
            from.begin(), from.width(), from.height(),
            to  .begin(), to  .width(), to  .height(),
            deg,          colorFill
        );
    }
}


} // namespace
} // namespace
