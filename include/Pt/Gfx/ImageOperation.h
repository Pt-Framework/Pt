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

#ifndef PT_GFX_IMAGEOPERATION_H
#define PT_GFX_IMAGEOPERATION_H

#include <Pt/Gfx/BlockScale.h>

#include <Pt/Gfx/ImageScale32Bpp.h>
#include <Pt/Gfx/ImageRotate32Bpp.h>


namespace Pt {
namespace Gfx {


//
// API with image iterators
//

/** @brief Block scale for images.
*/
template<typename InIterT, typename OutIterT>
inline void blockScaleImage(
    InIterT  from, Pt::ssize_t fromWidth, Pt::ssize_t fromHeight,
    OutIterT to,   Pt::ssize_t toWidth,   Pt::ssize_t toHeight
)
{
    if(fromWidth <= 0 || fromHeight <= 0 || toWidth <= 0 || toHeight <= 0)
        throw std::runtime_error("invalid fromWidth/fromHeight/toWidth/toHeight");

    if(from.pixelStride() != 4 || to.pixelStride() != 4) {
        // Use the generic implementation
        blockScale(
            from, fromWidth, fromHeight,
            to,   toWidth,   toHeight
        );
        return;
    }

    // Use the specialized implementation
    blockScaleImage32Bpp(
        from, fromWidth, fromHeight,
        to,   toWidth,   toHeight
    );
}

/** @brief Bilinear scale for images.
*/
template<typename InIterT, typename OutIterT>
inline void bilinearScaleImage(
    InIterT  from, Pt::ssize_t fromWidth, Pt::ssize_t fromHeight,
    OutIterT to,   Pt::ssize_t toWidth,   Pt::ssize_t toHeight
)
{
    if(fromWidth <= 0 || fromHeight <= 0 || toWidth <= 0 || toHeight <= 0)
        throw std::runtime_error("invalid fromWidth/fromHeight/toWidth/toHeight");

    if(from.pixelStride() != 4 || to.pixelStride() != 4)
        throw std::runtime_error("bilinear scale for images with pixel stride != 4 is not supported yet");

    bilinearScaleImage32Bpp(
        from, fromWidth, fromHeight,
        to,   toWidth,   toHeight
    );
}

/** @brief Block rotate for images.
*/
template<typename InIterT, typename OutIterT>
inline void blockRotateImage(
    InIterT  from, Pt::ssize_t fromWidth, Pt::ssize_t fromHeight,
    OutIterT to,   Pt::ssize_t toWidth,   Pt::ssize_t toHeight,
    float                     deg,
    const Color&              colorFill = Color::fromRgb8(0, 0, 0, 255),
    bool                      fullFit   = false
)
{
    if(fromWidth <= 0 || fromHeight <= 0 || toWidth <= 0 || toHeight <= 0)
        throw std::runtime_error("invalid fromWidth/fromHeight/toWidth/toHeight");

    if(from.pixelStride() != 4 || to.pixelStride() != 4)
        throw std::runtime_error("block rotate for images with pixel stride != 4 is not supported yet");

    if(fullFit) {
        blockRotateImage32Bpp<true>(
            from, fromWidth, fromHeight,
            to,   toWidth,   toHeight,
            deg,  colorFill
        );
    }
    else {
        blockRotateImage32Bpp<false>(
            from, fromWidth, fromHeight,
            to,   toWidth,   toHeight,
            deg,  colorFill
        );
    }
}

/** @brief Bilinear rotate for images.
*/
template<typename InIterT, typename OutIterT>
inline void bilinearRotateImage(
    InIterT  from, Pt::ssize_t fromWidth, Pt::ssize_t fromHeight,
    OutIterT to,   Pt::ssize_t toWidth,   Pt::ssize_t toHeight,
    float                     deg,
    const Color&              colorFill = Color::fromRgb8(0, 0, 0, 255),
    bool                      fullFit   = false
)
{
    if(fromWidth <= 0 || fromHeight <= 0 || toWidth <= 0 || toHeight <= 0)
        throw std::runtime_error("invalid fromWidth/fromHeight/toWidth/toHeight");

    if(from.pixelStride() != 4 || to.pixelStride() != 4)
        throw std::runtime_error("bilinear rotate for images with pixel stride != 4 is not supported yet");

    if(fullFit) {
        bilinearRotateImage32Bpp<true>(
            from, fromWidth, fromHeight,
            to,   toWidth,   toHeight,
            deg,  colorFill
        );
    }
    else {
        bilinearRotateImage32Bpp<false>(
            from, fromWidth, fromHeight,
            to,   toWidth,   toHeight,
            deg,  colorFill
        );
    }
}


//
// API with image objects
//

/** @brief Block scale for images.
*/
template<typename InImageT, typename OutImageT>
inline void blockScaleImage(const InImageT& from, OutImageT& to)
{
    blockScaleImage(
        from.begin(), from.width(), from.height(),
        to  .begin(), to  .width(), to  .height()
    );
}

/** @brief Bilinear scale for images.
*/
template<typename InImageT, typename OutImageT>
inline void bilinearScaleImage(const InImageT& from, OutImageT& to)
{
    bilinearScaleImage(
        from.begin(), from.width(), from.height(),
        to  .begin(), to  .width(), to  .height()
    );
}

/** @brief Block rotate for images.
*/
template<typename InImageT, typename OutImageT>
inline void blockRotateImage(
    const InImageT& from, OutImageT& to, float deg,
    const Color& colorFill = Color::fromRgb8(0, 0, 0, 255), bool fullFit = false
)
{
    blockRotateImage(
        from.begin(), from.width(), from.height(),
        to  .begin(), to  .width(), to  .height(),
        deg,          colorFill,    fullFit
    );
}

/** @brief Bilinear rotate for images.
*/
template<typename InImageT, typename OutImageT>
inline void bilinearRotateImage(
    const InImageT& from, OutImageT& to, float deg,
    const Color& colorFill = Color::fromRgb8(0, 0, 0, 255), bool fullFit = false
)
{
    bilinearRotateImage(
        from.begin(), from.width(), from.height(),
        to  .begin(), to  .width(), to  .height(),
        deg,          colorFill,    fullFit
    );
}


} // namespace
} // namespace


#endif
