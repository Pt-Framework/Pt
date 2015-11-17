/* Copyright (C) 2013 Laurentiu-Gheorghe Crisan
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
#include <iosfwd>

struct png_struct_def;
struct png_info_def;

namespace Pt {

namespace Gfx {

class Image;

class PT_GFX_API PngReader
{
    public:
        /** @brief Default Constructor.
        */
        PngReader();

        /** @brief Construct with target stream.
        */
        PngReader(std::istream& is);

        /** @brief Destructor.
        */
        ~PngReader();

        /** @brief Attach to target stream.
        */
        void attach(std::istream& is);

        /** @brief Detach from target stream.
        */
        void detach();

        /** @brief Reset to begin new compression/decompression.
        */
        void reset();

        /** @brief Reads image data from the target stream.
        */
        bool advance();

        /** @brief Sets the target image.
        */
        void setImage(Image& image)
        { _image = &image; }

        /** @brief Returns the target image.
        */
        Image* image()
        { return _image; }

        ///! @internal
        static void onInfo(PngReader& reader, png_struct_def* png, png_info_def* info);

        ///! @internal
        static void onRow(PngReader& reader, unsigned char* data, std::size_t row, int pass);

        ///! @internal
        static void onEnd(PngReader& reader, png_struct_def* png, png_info_def* info);

    private:
        enum State 
        {
            OnBegin = 0,
            OnSignature = 1,
            OnEnd = 2
        };

    private:
        std::ios* _target;
        State _state;
        png_struct_def* _pngRead;
        png_info_def* _pngInfo;
        char _buffer[2048];
        Image* _image;
        std::size_t _width;
        std::size_t _height;
        std::size_t _depth;
        std::size_t _channels;
};

} // namespace

} // namespace

#endif
