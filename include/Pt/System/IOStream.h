/***************************************************************************
 *   Copyright (C) 2005 Marc Boris D�rner                                  *
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
#include <Pt/System/IODevice.h>
#include <Pt/System/IOBuffer.h>

#include <iostream>
#include <memory>


namespace Pt {

namespace System {

    //! @brief An istream with peeking capability.
    template <typename CharT>
    class BasicIStream : public std::basic_istream<CharT> {
        public:
            ~BasicIStream() throw()
            { }

            //! @brief Access to the underlying buffer.
            BasicStreamBuffer<CharT>* rdbuf()
            { return _buffer; }

            //! @brief Peeks bytes in the stream buffer.
            /**
               The number of bytes that can be peeked depends on the current
               stream buffer get area and maybe less than requested,
               similar to istream::readsome().
            */
            std::streamsize peek(char* buffer, std::streamsize size) throw(IOError)
            { return _buffer->peek(buffer, size); }

            explicit BasicIStream(BasicStreamBuffer<CharT>* buffer) throw(IOError)
            : std::basic_istream<CharT>( buffer ),
              _buffer(buffer)
            { }

        private:
            BasicStreamBuffer<CharT>* _buffer;
    };


    //! @brief An ostream with peeking capability.
    template <typename CharT>
    class BasicOStream : public std::basic_ostream<CharT> {
        public:
            ~BasicOStream() throw()
            {}

            //! @brief Access to the underlying buffer.
            BasicStreamBuffer<CharT>* rdbuf()
            { return _buffer; }

        protected:
            explicit BasicOStream(BasicStreamBuffer<CharT>* buffer) throw(IOError)
            : std::basic_ostream<CharT>( buffer ),
              _buffer(buffer)
            { }

        private:
            BasicStreamBuffer<CharT>* _buffer;
    };


    //! @brief An iostream with peeking capability.
    template <typename CharT>
    class BasicIOStream : public std::basic_iostream<CharT> {
        public:
            ~BasicIOStream() throw()
            { }

            //! @brief Access to the underlying buffer.
            BasicStreamBuffer<CharT>* rdbuf()
            { return _buffer; }

            //! @brief Peeks bytes in the stream buffer.rb
            /**
               The number of bytes that can be peeked depends on the current
               stream buffer get area and maybe less than requested,
               similar to istream::readsome().
            */
            std::streamsize peeksome(char* buffer, std::streamsize size) throw(IOError)
            { return _buffer->peeksome(buffer, size); }

    protected:
            explicit BasicIOStream(BasicStreamBuffer<CharT>* buffer) throw(IOError)
            : std::basic_iostream<CharT>( buffer ),
              _buffer(buffer)
            { }

        private:
            BasicStreamBuffer<CharT>* _buffer;
};


    typedef BasicIStream<char> IStream;
    typedef BasicOStream<char> OStream;
    typedef BasicIOStream<char> IOStream;

} // namespace System

} // !namespace Pt

#endif


