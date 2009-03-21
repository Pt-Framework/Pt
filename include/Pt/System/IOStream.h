/*
 * Copyright (C) 2005-2008 Marc Boris Duerner
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
#include <Pt/System/StreamBuffer.h>
#include <iostream>
#include <algorithm>

namespace Pt {

namespace System {

//! @brief An istream with peeking capability.
template <typename CharT>
class BasicIStream : public std::basic_istream<CharT>
{
    public:
        explicit BasicIStream(BasicStreamBuffer<CharT>* buffer = 0)
        : std::basic_istream<CharT>( buffer ),
          _buffer(buffer)
        { }

        ~BasicIStream()
        { }

        //! @brief Access to the underlying buffer.
        BasicStreamBuffer<CharT>* attachedBuffer()
        { return _buffer; }

        BasicStreamBuffer<CharT>* attachBuffer(BasicStreamBuffer<CharT>*  buffer)
        {
            BasicStreamBuffer<CharT>* tmp = _buffer;
            _buffer = buffer;
            rdbuf(buffer);
            return tmp;
        }

        //! @brief Peeks bytes in the stream buffer.
        /**
            The number of bytes that can be peeked depends on the current
            stream buffer get area and maybe less than requested,
            similar to istream::readsome().
        */
        std::streamsize peeksome(CharT* buffer, std::streamsize n)
        {
            if(this->rdbuf() == _buffer)
                return _buffer->speekn(buffer, n);

            if(n > 0)
			{
                buffer[0] = this->peek();
				return 1;
			}

            return 0;
        }

    private:
        BasicStreamBuffer<CharT>* _buffer;
};


//! @brief An ostream with peeking capability.
template <typename CharT>
class BasicOStream : public std::basic_ostream<CharT>
{
    public:
        explicit BasicOStream(BasicStreamBuffer<CharT>* buffer = 0)
        : std::basic_ostream<CharT>( buffer ),
          _buffer(buffer)
        { }

        ~BasicOStream()
        {}

        //! @brief Access to the underlying buffer.
        BasicStreamBuffer<CharT>* attachedBuffer()
        { return _buffer; }

        BasicStreamBuffer<CharT>* attachBuffer(BasicStreamBuffer<CharT>*  buffer)
        {
            BasicStreamBuffer<CharT>* tmp = _buffer;
            _buffer = buffer;
            rdbuf(buffer);
            return tmp;
        }

        std::streamsize writesome(CharT* buffer, std::streamsize n)
        {
            std::basic_streambuf<CharT>* current = std::basic_ios<CharT>::rdbuf();
            if(current != _buffer)
                return 0;

            std::streamsize avail = _buffer->out_avail();
            if(avail == 0)
            {
                return 0;
            }

            n = std::min(avail, n);
            return _buffer->sputn(buffer, n);
        }

    private:
        BasicStreamBuffer<CharT>* _buffer;
};


//! @brief An iostream with peeking capability.
template <typename CharT>
class BasicIOStream : public std::basic_iostream<CharT>
{
    public:
        explicit BasicIOStream(BasicStreamBuffer<CharT>* buffer = 0)
        : std::basic_iostream<CharT>( buffer ),
          _buffer(buffer)
        { }

        ~BasicIOStream()
        { }

        //! @brief Access to the underlying buffer.
        BasicStreamBuffer<CharT>* attachedBuffer()
        { return _buffer; }

        BasicStreamBuffer<CharT>* attachBuffer(BasicStreamBuffer<CharT>*  buffer)
        {
            BasicStreamBuffer<CharT>* tmp = _buffer;
            _buffer = buffer;
            rdbuf(buffer);
            return tmp;
        }

        //! @brief Peeks bytes in the stream buffer.
        /**
            The number of bytes that can be peeked depends on the current
            stream buffer get area and maybe less than requested,
            similar to istream::readsome().
        */
        std::streamsize peeksome(CharT* buffer, std::streamsize n)
        {
            if(this->rdbuf() == _buffer)
                return _buffer->speekn(buffer, n);

            if(n > 0)
			{
                buffer[0] = this->peek();
				return 1;
			}

            return 0;
        }

        std::streamsize writesome(CharT* buffer, std::streamsize n)
        {
            std::basic_streambuf<CharT>* current = std::basic_ios<CharT>::rdbuf();
            if(current != _buffer)
                return 0;

            std::streamsize avail = _buffer->out_avail();
            if(avail == 0)
            {
                return 0;
            }

            n = std::min(avail, n);
            return _buffer->sputn(buffer, n);
        }

    private:
        BasicStreamBuffer<CharT>* _buffer;
};


class IStream : public BasicIStream<char>
{
    public:
        IStream(size_t bufferSize = 8192);

        ~IStream();

        IStream(IODevice& device, size_t bufferSize = 8192);

        StreamBuffer& buffer();

        IODevice* attachDevice(IODevice& device);

        IODevice* attachedDevice();

    private:
        StreamBuffer _buffer;
};


class OStream : public BasicIStream<char>
{
    public:
        OStream(size_t bufferSize = 8192);

        OStream(IODevice& device, size_t bufferSize = 8192);

        ~OStream();

        StreamBuffer& buffer();

        IODevice* attachDevice(IODevice& device);

        IODevice* attachedDevice();

    private:
        StreamBuffer _buffer;
};


class IOStream : public BasicIOStream<char>
{
    public:
        IOStream(size_t bufferSize = 8192);

        IOStream(IODevice& device, size_t bufferSize = 8192);

        ~IOStream();

        StreamBuffer& buffer();

        IODevice* attachDevice(IODevice& device);

        IODevice* attachedDevice();

    private:
        StreamBuffer _buffer;
};


inline IStream::IStream(size_t bufferSize)
: _buffer(bufferSize)
{
    attachBuffer(&_buffer);
}


inline IStream::IStream(IODevice& device, size_t bufferSize)
: _buffer(device, bufferSize)
{
    attachBuffer(&_buffer);
}


inline IStream::~IStream()
{
}


inline StreamBuffer& IStream::buffer()
{
return _buffer;
}


inline IODevice* IStream::attachDevice(IODevice& device)
{
    IODevice* ret = attachedDevice();
    _buffer.attach(device);
    return ret;
}


inline IODevice* IStream::attachedDevice()
{
return _buffer.device();
}


inline OStream::OStream(size_t bufferSize)
: _buffer(bufferSize)
{
    attachBuffer(&_buffer);
}


inline OStream::OStream(IODevice& device, size_t bufferSize)
: _buffer(device, bufferSize)
{
    attachBuffer(&_buffer);
}


inline OStream::~OStream()
{
}


inline StreamBuffer& OStream::buffer()
{
    return _buffer;
}


inline IODevice* OStream::attachDevice(IODevice& device)
{
    IODevice* ret = attachedDevice();
    _buffer.attach(device);
    return ret;
}


inline IODevice* OStream::attachedDevice()
{
    return _buffer.device();
}


inline IOStream::IOStream(size_t bufferSize)
: _buffer(bufferSize)
{
    attachBuffer(&_buffer);
}


inline IOStream::~IOStream()
{
}


inline IOStream::IOStream(IODevice& device, size_t bufferSize)
: _buffer(device, bufferSize)
{
    attachBuffer(&_buffer);
}


inline StreamBuffer& IOStream::buffer()
{
    return _buffer;
}


inline IODevice* IOStream::attachDevice(IODevice& device)
{
    IODevice* ret = attachedDevice();
    _buffer.attach(device);
    return ret;
}


inline IODevice* IOStream::attachedDevice()
{
    return _buffer.device();
}


} // namespace System

} // !namespace Pt

#endif

