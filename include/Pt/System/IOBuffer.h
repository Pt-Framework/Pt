/***************************************************************************
 *   Copyright (C) 2005 Marc Boris Dürner                                  *
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
#ifndef Pt_System_IOBuffer_h
#define Pt_System_IOBuffer_h

#include <Pt/System/Api.h>
#include <Pt/System/IODevice.h>
#include <Pt/System/StreamBuffer.h>


namespace Pt {

namespace System {

    //! @brief a stream buffer for IODevices with linear buffer area.
    template <typename CharT>
    class BasicIOBuffer : public BasicStreamBuffer<CharT> {
        public:
            typedef typename std::basic_streambuf<CharT>::int_type int_type;
            typedef typename std::basic_streambuf<CharT>::traits_type traits_type;
            typedef typename std::basic_streambuf<CharT>::pos_type pos_type;
            typedef typename std::basic_streambuf<CharT>::off_type off_type;

        public:
            //! @brief Contructs an IOBuffer for an IODevice.
            BasicIOBuffer(BasicIODevice<CharT>& ioDevice, size_t bufferSize = 1024)
            : _ioDevice(0),
              _buffer(0),
              _bufferSize(bufferSize),
              _putbackMax(4)
            {
                _buffer = new CharT[_bufferSize];

                this->setg(0, 0, 0);
                this->setp(0, 0);
            }

            //! @brief Default constructor.
            BasicIOBuffer(size_t bufferSize = 1024)
            : _ioDevice(0),
              _buffer(0),
              _bufferSize(bufferSize),
              _putbackMax(4)
            {
                _buffer = new CharT[_bufferSize];

                this->setg(0, 0, 0);
                this->setp(0, 0);
            }

            ~BasicIOBuffer()
            { delete[] _buffer; }

            void init(BasicIODevice<CharT>& ioDevice)
            { _ioDevice = &ioDevice; }

            IODevice* device()
            { return _ioDevice; }

        protected:
            int sync();

            virtual std::streamsize _peek(CharT* buffer, std::streamsize size);

            virtual int_type underflow();

            virtual int_type overflow(int_type ch);

            virtual pos_type seekoff(off_type offset, std::ios::seekdir sd, std::ios::openmode mode);

        private:
            BasicIODevice<CharT>* _ioDevice;

            CharT* _buffer;

            const size_t _bufferSize;

            const size_t _putbackMax;
    };


    typedef BasicIOBuffer<char> IOBuffer;


    template <typename CharT>
    int BasicIOBuffer<CharT>::sync()
    {
        if(!_ioDevice)
            return 0;

        // if in writing mode flush put area
        if( this->pptr() ) {
            const int_type ch = this->overflow( traits_type::eof() );
            if( ch == traits_type::eof() ) {
                return -1;
            }

            _ioDevice->sync();
        }

        return 0;
    }


    template <typename CharT>
    std::streamsize BasicIOBuffer<CharT>::_peek(CharT* buffer, std::streamsize size)
    {
        // can not peek in writing mode
        if( this->pptr() )
            return 0;

        if( traits_type::eof() == this->underflow() )
            return 0;

        const std::streamsize avail = this->egptr() - this->gptr();
        size = std::min(avail, size);
        if(size == 0) {
            return 0;
        }

        std::memcpy(buffer, this->gptr(), sizeof(CharT) * size);
        return size;
    }


    template <typename CharT>
    typename BasicIOBuffer<CharT>::int_type
    BasicIOBuffer<CharT>::underflow()
    {
        // return EOF if in writing mode or no device set
        if( !_ioDevice || this->pptr() )
            return traits_type::eof();

        // return if input buffer is not full yet.
        if( this->gptr() < this->egptr() ) {
            return traits_type::to_int_type( *(this->gptr()) );
        }

        size_t putbackSize = _putbackMax;

        // keep chars for putback if in reading mode
        if( this->gptr() ) {
            putbackSize = std::min<size_t>(this->gptr() - this->eback(), _putbackMax);
            std::memmove(_buffer + (_putbackMax - putbackSize),
                         this->gptr() - putbackSize,
                         putbackSize * sizeof(CharT) );
        }

        size_t readSize = _ioDevice->read( _buffer + _putbackMax, _bufferSize - _putbackMax );

        // set get area, will also enter reading mode
        this->setg( _buffer + (_putbackMax - putbackSize), // start of get area
                    _buffer + _putbackMax, // gptr position
                    _buffer + _putbackMax + readSize ); // end of get area

        if( _ioDevice->eof() )
            return traits_type::eof();

        return traits_type::to_int_type( *(this->gptr()) );
    }


    template <typename CharT>
    typename BasicIOBuffer<CharT>::int_type
    BasicIOBuffer<CharT>::overflow(int_type ch)
    {
        // return EOF if we are in reading mode or no device is set
        if(!_ioDevice || this->gptr() )
            return traits_type::eof();

        size_t leftover = 0;

        // if in writing mode pptr is valid, flush out the buffer
        if( this->pptr() )
        {
            // write buffer to device
            const size_t avail = this->pptr() - this->pbase();
            size_t written = _ioDevice->write(_buffer, avail);
            // check EOF ???

            // setup put buffer area
            const size_t leftover = avail - written;
            if(leftover != 0) {
                traits_type::move(_buffer, _buffer + written, leftover);
            }
        }

        // this will also enter writing mode if pptr is not valid
        this->setp(_buffer + leftover, _buffer + _bufferSize);

        // if the overflow char is not EOF put it in buffer
        if( traits_type::eq_int_type(ch, traits_type::eof()) ==  false )
        {
            *this->pptr() = traits_type::to_char_type(ch);
            this->pbump(1);
        }

        return traits_type::not_eof(ch);
    }


    template <typename CharT>
    typename BasicIOBuffer<CharT>::pos_type
    BasicIOBuffer<CharT>::seekoff(off_type offset, std::ios::seekdir sd, std::ios::openmode mode)
    {
        if(mode == std::ios::out)
            return pos_type(-1);

        if(!_ioDevice || !_ioDevice->seekable() )
            return pos_type(-1);

        typename BasicIODevice<CharT>::SeekMode sm = BasicIODevice<CharT>::SeekCurrent;
        if(sd == std::ios_base::beg) {
            sm = BasicIODevice<CharT>::SeekBegin;
        }
        else if(sd == std::ios_base::cur) {
            sm = BasicIODevice<CharT>::SeekCurrent;
        }
        else { // ios::end
            sm = BasicIODevice<CharT>::SeekEnd;
        }

        return pos_type(-1);
    }

} // namespace Pt

} // !namespace Pt

#endif


