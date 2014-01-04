/*
 * Copyright (C) 2012 Marc Boris Duerner
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef Pt_Xml_ByteoderMark_h
#define Pt_Xml_ByteoderMark_h

#include <Pt/Xml/Api.h>

namespace Pt {

namespace Xml {

/** @brief XML byte order mark.
*/
class ByteorderMark
{
    public:
        /** @brief Text encoding endianess.
        */
        enum Endianess
        {
            None = 0,
            BigEndian = 1,
            LittleEndian = 2
        };

        /** @brief Text encoding type.
        */
        enum Encoding
        {
            Generic = 0,
            Unicode = 1
        };

    public:
        /** @brief Default constructor.
        */
        ByteorderMark()
        : _endianess(None)
        , _width(1)
        , _encoding(Generic)
        {}

        /** @brief Clears all content.
        */
        void clear()
        {
            _endianess = None;
            _width = 1;
            _encoding = Generic;
        }

        /** @brief The endianess of the document encoding.
        */
        Endianess endianess() const
        { return static_cast<Endianess>(_endianess); }

        /** @brief Sets the endianess of the document encoding.
        */
        void setEndianess(Endianess e)
        { _endianess = static_cast<unsigned char>(e); }

        /** @brief Returns the word width of the encoding.
        */
        int width() const
        { return _width; }

        /** @brief Sets word width of the encoding.
        */
        void setWidth(unsigned char w)
        { _width = w; }

        /** @brief Returns the type of encoding.
        */
        Encoding encoding() const
        { return static_cast<Encoding>(_encoding); }

        /** @brief Sets the type of encoding.
        */
        void setEncoding(Encoding e)
        { _encoding = static_cast<unsigned char>(e); }

    private:
        unsigned char _endianess;
        unsigned char _width;
        unsigned char _encoding;
};

} // namespace Xml

} // namespace Pt

#endif // Pt_Xml_ByteoderMark_h
