/*
 * Copyright (C) 2005-2012 Marc Boris Duerner
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

#ifndef Pt_System_IOStream_h
#define Pt_System_IOStream_h

#include <Pt/System/Api.h>
#include <Pt/System/IOBuffer.h>
#include <Pt/IOStream.h>
#include <cstddef>

namespace Pt {

namespace System {

class IStream : public BasicIStream<char>
{
    public:
        explicit IStream(std::size_t bufferSize = 8192, bool extend = false);
        
        explicit IStream(IODevice& device, std::size_t bufferSize = 8192, bool extend = false);
        
        ~IStream()
        {}

        IOBuffer& ioBuffer()
        { return _buffer; }
        
        IODevice* device()
        { return _buffer.device(); }
        
        void attach(IODevice& dev)
        { _buffer.attach(dev); }

        void detach()
        { _buffer.detach(); }

        void discard()
        { _buffer.discard(); }

        void reset()
        { _buffer.reset(); }

    private:
        IOBuffer _buffer;
};


class OStream : public BasicOStream<char>
{
    public:
        explicit OStream(std::size_t bufferSize = 8192, bool extend = false);

        explicit OStream(IODevice& device, std::size_t bufferSize = 8192, bool extend = false);

        ~OStream()
        {}

        IOBuffer& ioBuffer()
        { return _buffer; }

        IODevice* device()
        { return _buffer.device(); }

        void attach(IODevice& dev)
        { _buffer.attach(dev); }

        void detach()
        { _buffer.detach(); }

        void discard()
        { _buffer.discard(); }

        void reset()
        { _buffer.reset(); }
    
    private:
        IOBuffer _buffer;
};


class IOStream : public BasicIOStream<char>
{
    public:
        explicit IOStream(std::size_t bufferSize = 8192, bool extend = false);

        explicit IOStream(IODevice& device, std::size_t bufferSize = 8192, bool extend = false);

        ~IOStream()
        {}

        IOBuffer& ioBuffer()
        { return _buffer; }

        IODevice* device()
        { return _buffer.device(); }

        void attach(IODevice& dev)
        { _buffer.attach(dev); }

        void detach()
        { _buffer.detach(); }

        void discard()
        { _buffer.discard(); }

        void reset()
        { _buffer.reset(); }
    
    private:
        IOBuffer _buffer;
};


inline IStream::IStream(std::size_t bufferSize, bool extend)
: BasicIStream<char>(0)
, _buffer(bufferSize, extend)
{
    this->setBuffer(&_buffer);
}


inline IStream::IStream(IODevice& device, std::size_t bufferSize, bool extend)
: BasicIStream<char>(0)
, _buffer(device, bufferSize, extend)
{
    this->setBuffer(&_buffer);
}


inline OStream::OStream(std::size_t bufferSize, bool extend)
: BasicOStream<char>(0)
, _buffer(bufferSize, extend)
{
    this->setBuffer(&_buffer);
}


inline OStream::OStream(IODevice& device, std::size_t bufferSize, bool extend)
: BasicOStream<char>(0)
, _buffer(device, bufferSize, extend)
{
    this->setBuffer(&_buffer);
}


inline IOStream::IOStream(std::size_t bufferSize, bool extend)
: BasicIOStream<char>(0)
, _buffer(bufferSize, extend)
{
    this->setBuffer(&_buffer);
}


inline IOStream::IOStream(IODevice& device, std::size_t bufferSize, bool extend)
: BasicIOStream<char>(0)
, _buffer(device, bufferSize, extend)
{
    this->setBuffer(&_buffer);
}

} // namespace System

} // namespace Pt

#endif // Pt_System_IOStream_h
