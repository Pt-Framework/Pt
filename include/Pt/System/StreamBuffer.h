/*
 * Copyright (C) 2005-2011 Marc Boris Duerner
 * Copyright (C) 2011      Aloysius Indayanto
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
#ifndef Pt_System_StreamBuffer_h
#define Pt_System_StreamBuffer_h

#include <Pt/System/Api.h>
#include <Pt/System/IODevice.h>
#include <streambuf>
#include <ios>
#include <cstring>

namespace Pt {

namespace System {

class PT_SYSTEM_API StreamBufferImpl {
    public:
        StreamBufferImpl(StreamBuffer& sb, size_t bufferSize, bool extend);

        IODevice* ioDevice()
        { return _ioDevice; }

        Signal<StreamBuffer&>& inputReady()
        { return _inputReady; }

        Signal<StreamBuffer&>& outputReady()
        { return _outputReady; }

        void attach(StreamBuffer& sb, IODevice& ioDevice);

        void beginRead(StreamBuffer& sb);
        void onRead(StreamBuffer& sb);
        void endRead(StreamBuffer& sb);

        size_t beginWrite(StreamBuffer& sb);
        void onWrite(StreamBuffer& sb);
        size_t endWrite(StreamBuffer& sb);

        void discard(StreamBuffer& sb);
        int sync(StreamBuffer& sb);

        std::streambuf::int_type underflow(StreamBuffer& sb);
        std::streambuf::int_type overflow(StreamBuffer& sb, std::streambuf::int_type ch);

        std::streamsize xspeekn(StreamBuffer& sb, char* buffer, std::streamsize size);
        std::streambuf::pos_type seekoff(StreamBuffer& sb, std::streambuf::off_type off, std::ios::seekdir dir, std::ios::openmode);
        std::streambuf::pos_type seekpos(StreamBuffer& sb, std::streambuf::pos_type p, std::ios::openmode mode);
        std::streamsize showfull(StreamBuffer& sb);
        std::streambuf::int_type pbackfail(StreamBuffer& sb, std::streambuf::int_type c);

    private:
        IODevice*    _ioDevice;
        size_t       _ibufferSize;
        char*        _ibuffer;
        std::size_t  _obufferSize;
        char*        _obuffer;
        const size_t _pbmax;
        bool         _oextend;

        Signal<StreamBuffer&> _inputReady;
        Signal<StreamBuffer&> _outputReady;
};

class StreamBuffer : public std::streambuf
                   , public Connectable
{
    friend class StreamBufferImpl;

    public:
        explicit StreamBuffer(size_t bufferSize = 8192, bool extend = false)
        : _impl(*this, bufferSize, extend)
        {}

        explicit StreamBuffer(IODevice& ioDevice, size_t bufferSize = 8192, bool extend = false)
        : _impl(*this, bufferSize, extend)
        { _impl.attach(*this, ioDevice); }

        ~StreamBuffer()
        {}

        IODevice* device()
        { return _impl.ioDevice(); }

        Signal<StreamBuffer&>& inputReady()
        { return _impl.inputReady(); }

        Signal<StreamBuffer&>& outputReady()
        { return _impl.outputReady(); }

        std::streamsize out_avail()
        {
            if( this->pptr() )
                return this->pptr() - this->pbase();

            return _impl.showfull(*this);
        }

        std::streamsize speekn(char* buffer, std::streamsize size)
        { return _impl.xspeekn(*this, buffer, size); }

        void attach(IODevice& ioDevice)
        { _impl.attach(*this, ioDevice); }

        void beginRead()
        { _impl.beginRead(*this); }

        void onRead(IODevice& dev)
        { _impl.onRead(*this); }

        void endRead()
        { _impl.endRead(*this); }

        void beginWrite()
        { _impl.beginWrite(*this); }

        void onWrite(IODevice& dev)
        { _impl.onWrite(*this); }

        void endWrite()
        { _impl.endWrite(*this); }

        void discard()
        { _impl.discard(*this); }

    protected:
        virtual int sync()
        { return _impl.sync(*this); }

        virtual int_type underflow()
        { return _impl.underflow(*this); }

        virtual int_type overflow(int_type ch)
        { return _impl.overflow(*this, ch); }

        virtual pos_type seekoff(off_type offset, std::ios::seekdir sd, std::ios::openmode mode)
        { return _impl.seekoff(*this, offset, sd, mode); }

        virtual pos_type seekpos(pos_type p, std::ios::openmode mode )
        { return _impl.seekpos(*this, p, mode); }

        virtual int_type pbackfail(int_type c)
        { return _impl.pbackfail(*this, c); }

    private:
        StreamBufferImpl _impl;
};

} // namespace System

} // namespace Pt

#endif
