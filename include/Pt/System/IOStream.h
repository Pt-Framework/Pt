/***************************************************************************
 *   Copyright (C) 2005-2008 Marc Boris Duerner                            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
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
            std::basic_streambuf<CharT>* current = std::basic_ios<CharT>::rdbuf();
            if(current == _buffer)
                return _buffer->speekn(buffer, n);

            if(n > 0)
                buffer[0] = current->getc();

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
            std::basic_streambuf<CharT>* current = std::basic_ios<CharT>::rdbuf();
            if(current == _buffer)
                return _buffer->speekn(buffer, n);

            if(n > 0)
                buffer[0] = current->getc();

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
        IStream(size_t bufferSize = 8192)
        : _buffer(bufferSize)
        {
            attachBuffer(&_buffer);
        }

        IStream(IODevice& device, size_t bufferSize = 8192)
        : _buffer(device, bufferSize)
        {
            attachBuffer(&_buffer);
        }

        StreamBuffer& buffer()
        { return _buffer; }

        IODevice* attachDevice(IODevice& device)
        {
            IODevice* ret = attachedDevice();
            _buffer.attach(device);
            return ret;
        }

        IODevice* attachedDevice()
        { return _buffer.device(); }

    private:
        StreamBuffer _buffer;
};


class OStream : public BasicIStream<char>
{
    public:
        OStream(size_t bufferSize = 8192)
        : _buffer(bufferSize)
        {
            attachBuffer(&_buffer);
        }

        OStream(IODevice& device, size_t bufferSize = 8192)
        : _buffer(device, bufferSize)
        {
            attachBuffer(&_buffer);
        }

        StreamBuffer& buffer()
        { return _buffer; }

        IODevice* attachDevice(IODevice& device)
        {
            IODevice* ret = attachedDevice();
            _buffer.attach(device);
            return ret;
        }

        IODevice* attachedDevice()
        { return _buffer.device(); }

    private:
        StreamBuffer _buffer;
};


class IOStream : public BasicIOStream<char>
{
    public:
        IOStream(size_t bufferSize = 8192)
        : _buffer(bufferSize)
        {
            attachBuffer(&_buffer);
        }

        IOStream(IODevice& device, size_t bufferSize = 8192)
        : _buffer(device, bufferSize)
        {
            attachBuffer(&_buffer);
        }

        StreamBuffer& buffer()
        { return _buffer; }

        IODevice* attachDevice(IODevice& device)
        {
            IODevice* ret = attachedDevice();
            _buffer.attach(device);
            return ret;
        }

        IODevice* attachedDevice()
        { return _buffer.device(); }

    private:
        StreamBuffer _buffer;
};

} // namespace System

} // !namespace Pt

#endif

