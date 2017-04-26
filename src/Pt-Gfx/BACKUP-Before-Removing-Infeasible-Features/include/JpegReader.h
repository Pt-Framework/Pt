/* Copyright (C) 2016 Marc Boris Duerner
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

#ifndef PT_GFX_JEPGREADER_H
#define PT_GFX_JEPGREADER_H

#include <Pt/Gfx/Api.h>
#include <iosfwd>

namespace Pt {

namespace Gfx {

class Image;

// TODO: ImageIOHandler used by ImageReader and ImageWriter

/** @brief Reader for JPEG images.
*/
class PT_GFX_API JpegReader
{
    public:
        /** @brief Default Constructor.
        */
        JpegReader();

        /** @brief Construct with target stream and image.
        */
        JpegReader(std::istream& is, Image& image);

        /** @brief Destructor.
        */
        ~JpegReader();

        /** @brief Attach to target stream and image.
        */
        void attach(std::istream& is, Image& image);

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

    private:
        class JpegReaderImpl* _impl;
};

} // namespace

} // namespace

#endif
