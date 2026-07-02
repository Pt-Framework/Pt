/* Copyright (C) 2017 Marc Duerner
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

#ifndef PT_GFX_PNGREADER_H
#define PT_GFX_PNGREADER_H

#include <Pt/Gfx/Api.h>
#include <ios>

namespace Pt {

namespace Gfx {

/** @brief Reader for PNG images.
*/
class PT_GFX_API PngReader
{
    public:
        /** @brief Default Constructor.
        */
        PngReader();

        /** @brief Construct with target stream and image.
        */
        PngReader(std::istream& is, Image& image);

        /** @brief Destructor.
        */
        ~PngReader();

        /** @brief Attach to target stream and image.
        */
        void attach(std::istream& is, Image& image);

        /** @brief Detach from target stream.
        */
        void detach();

        /** @brief Reset to begin new decompression.
        */
        void reset();

        /** @brief Reads image data from the target stream.

            Consumes bytes currently available in the stream buffer and feeds
            them to the PNG decoder.

            When @a importSize is 0 (default), only bytes already available via
            in_avail() are consumed (non-blocking, suitable for event-loop use).
            When @a importSize is greater than 0, up to that many bytes are read
            from the underlying stream via sgetn(), which may block until data
            arrives (suitable for file or thread use).

            @returns Pointer to the completed %Image once all PNG data has been
                     decoded, nullptr when more data is needed.
        */
        Image* advance(std::streamsize importSize = 0);

        /** @brief Reads the whole image from the stream.
        */
        Image& get();

    private:
        class PngReaderImpl* _impl;
};

} // namespace

} // namespace

#endif
