/*
 * Copyright (C) 2015 Marc Boris Duerner
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

#ifndef Pt_ZStream_h
#define Pt_ZStream_h

#include <Pt/Api.h>
#include <Pt/IOStream.h>
#include <Pt/ZBuffer.h>

namespace Pt {

/** @brief Input stream for zlib compression.
*/
class ZIStream : public BasicIStream<char>
{
    public:
        /** @brief Construct with target stream.
        */
        ZIStream()
        : BasicIStream<char>(0)
        { 
            this->setBuffer(&_buffer);
        }
        
        /** @brief Construct with target stream.
        */
        explicit ZIStream(std::istream& is)
        : BasicIStream<char>(0)
        , _buffer(is)
        {
            this->setBuffer(&_buffer);
        }

        //! @brief Destructor.
        ~ZIStream()
        {}

        /** @brief Returns the compression buffer.
        */
        ZBuffer& zBuffer()
        { return _buffer; }

        /** @brief Attach to target stream.
        */
        void attach(std::istream& is)
        {
            _buffer.attach(is);
        }
        
        /** @brief Detach from target stream.
        */
        void detach()
        {
            _buffer.detach();
        }
        
        /** @brief Reset to begin new compression/decompression.
        */
        void reset()
        {
            _buffer.reset();
        }
        
        /** @brief Reset to begin new compression/decompression.
        */
        void reset(std::istream& is)
        {
            _buffer.reset(is);
        }

        /** @brief Finish and flush remaining data to the target stream.
        */
        void finish()
        {
            _buffer.finish();
        }

    private:
        ZBuffer _buffer;
};

/** @brief Output stream for zlib compression.
*/
class ZOStream : public BasicOStream<char>
{
    public:
        /** @brief Construct with target stream.
        */
        ZOStream()
        : BasicOStream<char>(0)
        { 
            this->setBuffer(&_buffer);
        }
        
        /** @brief Construct with target stream.
        */
        explicit ZOStream(std::ostream& os)
        : BasicOStream<char>(0)
        , _buffer(os)
        {
            this->setBuffer(&_buffer);
        }

        //! @brief Destructor.
        ~ZOStream()
        {}

        /** @brief Returns the compression buffer.
        */
        ZBuffer& zBuffer()
        { return _buffer; }

        /** @brief Attach to target stream.
        */
        void attach(std::ostream& os)
        {
            _buffer.attach(os);
        }
        
        /** @brief Detach from target stream.
        */
        void detach()
        {
            _buffer.detach();
        }
        
        /** @brief Reset to begin new compression/decompression.
        */
        void reset()
        {
            _buffer.reset();
        }
        
        /** @brief Reset to begin new compression/decompression.
        */
        void reset(std::ostream& os)
        {
            _buffer.reset(os);
        }

        /** @brief Finish and flush remaining data to the target stream.
        */
        void finish()
        {
            _buffer.finish();
        }

    private:
        ZBuffer _buffer;
};

/** @brief I/O stream for zlib compression.
*/
class ZIOStream : public BasicIOStream<char>
{
    public:
        /** @brief Construct with target stream.
        */
        ZIOStream()
        : BasicIOStream<char>(0)
        { 
            this->setBuffer(&_buffer);
        }
        
        /** @brief Construct with target stream.
        */
        explicit ZIOStream(std::iostream& ios)
        : BasicIOStream<char>(0)
        , _buffer(ios)
        {
            this->setBuffer(&_buffer);
        }

        //! @brief Destructor.
        ~ZIOStream()
        {}

        /** @brief Returns the compression buffer.
        */
        ZBuffer& zBuffer()
        { return _buffer; }

        /** @brief Attach to target stream.
        */
        void attach(std::iostream& ios)
        {
            _buffer.attach(ios);
        }
        
        /** @brief Detach from target stream.
        */
        void detach()
        {
            _buffer.detach();
        }
        
        /** @brief Reset to begin new compression/decompression.
        */
        void reset()
        {
            _buffer.reset();
        }
        
        /** @brief Reset to begin new compression/decompression.
        */
        void reset(std::iostream& ios)
        {
            _buffer.reset(ios);
        }

        /** @brief Finish and flush remaining data to the target stream.
        */
        void finish()
        {
            _buffer.finish();
        }

    private:
        ZBuffer _buffer;
};

} // namespace Pt

#endif // Pt_ZStream_h
