/*
 * Copyright (C) 2015 by Marc Boris Duerner
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

#ifndef PT_ZBUFFER_H
#define PT_ZBUFFER_H

#include <Pt/Api.h>
#include <Pt/StreamBuffer.h>
#include <ios>
#include <cstddef>

typedef struct z_stream_s z_stream;

namespace Pt {

/** @brief Stream buffer for zlib compression.
*/
class PT_API ZBuffer : public BasicStreamBuffer<char>
{
    public:
        /** @brief Default Constructor.
        */
        ZBuffer();
        
        /** @brief Construct with target stream.
        */
        ZBuffer(std::ios& ios);

        /** @brief Destructor.
        */
        virtual ~ZBuffer();
        
        /** @brief Attach to target stream.
        */
        void attach(std::ios& target);
        
        /** @brief Detach from target stream.
        */
        void detach();
        
        /** @brief Discards the buffer content and resets the state.
        */
        void discard();

        /** @brief Detach from target stream, discard the buffer and reset the state.
        */
        void reset();

        /** @brief Attach to target stream, discard the buffer and reset the state.
        */
        void reset(std::ios& target);

        /** @brief Finish and flush remaining data to the target stream.
        */
        void finish();

        /** @brief Import data from the target stream.

            Returns the number of bytes consumed from the underlyig stream.
        */
        std::streamsize import(std::streamsize maxImport = 0);

        std::streamsize import(const char* data, std::streamsize size);

    protected:
        // inheritdoc
        virtual std::streamsize showmanyc();

        // inheritdoc
        virtual std::streamsize showfull();

        // inheritdoc
        virtual int sync();
        
        // inheritdoc
        virtual int_type underflow();
        
        // inheritdoc
        virtual int_type overflow(int_type ch);
    
    private:
        void inflateBuffer();

    private:
        std::ios* _target;
        z_stream* _zstr;

        static const int _pbmax = 4;
        static const int _bufmax = 1024;
        char _buf[_bufmax];

        static const int _zbufmax = 1024;
        char _zbuf[_zbufmax];
        int _zbufsize;
};

} // namespace Pt

#endif // PT_ZBUFFER_H
