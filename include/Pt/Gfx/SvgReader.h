/* Copyright (C) 2016-2017 Marc Boris Duerner
 * Copyright (C) 2017-2017 Aloysius Indrayanto
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA  02110-1301  USA
 */

#ifndef PT_GFX_SVGREADER_H
#define PT_GFX_SVGREADER_H

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/Transform.h>

#include <iosfwd>


namespace Pt {
namespace Gfx {


class Image;


/** @brief Reader for SVG images.
*/
class PT_GFX_API SvgReader
{
    public:
        /** @brief Set the PPI of the display/monitor (zero = use default)
        */
        static void setDisplayPPI(Pt::uint32_t ppi = 0);

    public:
        /** @brief Default Constructor.
        */
        SvgReader();

        /** @brief Construct with target stream, image, and starting rendering coordinate.
        */
        SvgReader(std::istream& is, Image& image, const PointF& topLeft = PointF(0, 0));

        /** @brief Destructor.
        */
        ~SvgReader();

        /** @brief Attach to target stream, image, and starting rendering coordinate.
        */
        void attach(std::istream& is, Image& image, const PointF& topLeft = PointF(0, 0));

        /** @brief Detach from target stream.
        */
        void detach();

        /** @brief Reset to begin new compression/decompression.
        */
        void reset();

        /** @brief Reads image data from the target stream.
        */
        Image* advance();

        /** @brief Reads the whole image from the stream.
        */
        Image& get();

        // ### TODO: Add support for animated SVG! ###

    private:
        class SvgReaderImpl* _impl;
};


} // namespace
} // namespace

#endif
