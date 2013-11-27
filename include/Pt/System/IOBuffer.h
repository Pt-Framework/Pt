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
#ifndef Pt_System_IOBuffer_h
#define Pt_System_IOBuffer_h

#include <Pt/System/Api.h>
#include <Pt/System/IODevice.h>
#include <Pt/StreamBuffer.h>
#include <streambuf>
#include <ios>
#include <cstring>

namespace Pt {

namespace System {

class PT_SYSTEM_API IOBufferImpl 
{
    public:
        IOBufferImpl();

        ~IOBufferImpl();

        void init(IOBuffer& sb, size_t bufferSize, bool extend);

        IODevice* ioDevice()
        { return _ioDevice; }

        Signal<IOBuffer&>& inputReady()
        { return _inputReady; }

        Signal<IOBuffer&>& outputReady()
        { return _outputReady; }

        void attach(IOBuffer& sb, IODevice& ioDevice);
        void detach(IOBuffer& sb);
        void beginRead(IOBuffer& sb);
        void onRead(IOBuffer& sb);
        size_t endRead(IOBuffer& sb);
        size_t beginWrite(IOBuffer& sb);
        void onWrite(IOBuffer& sb);
        size_t endWrite(IOBuffer& sb);
        void discard(IOBuffer& sb);
        bool isReading() const;
        bool isWriting() const;
        int sync(IOBuffer& sb);
        std::streambuf::int_type underflow(IOBuffer& sb);
        std::streambuf::int_type overflow(IOBuffer& sb, std::streambuf::int_type ch);
        std::streambuf::pos_type seekoff(IOBuffer& sb, std::streambuf::off_type off, std::ios::seekdir dir, std::ios::openmode);
        std::streambuf::pos_type seekpos(IOBuffer& sb, std::streambuf::pos_type p, std::ios::openmode mode);
        std::streamsize showmanyc(IOBuffer& sb);
        std::streamsize showfull(IOBuffer& sb);
        std::streambuf::int_type pbackfail(IOBuffer& sb, std::streambuf::int_type c);

    private:
        IODevice*    _ioDevice;
        size_t       _ibufferSize;
        char*        _ibuffer;
        std::size_t  _obufferSize;
        char*        _obuffer;
        const size_t _pbmax;
        bool         _oextend;

        Signal<IOBuffer&> _inputReady;
        Signal<IOBuffer&> _outputReady;
};

class PT_SYSTEM_API IOBuffer : public BasicStreamBuffer<char>
                             , public Connectable
{
    friend class IOBufferImpl;

    public:
        explicit IOBuffer(size_t bufferSize = 8192, bool extend = false)
        : _impl()
        {
            _impl.init(*this, bufferSize, extend);
        }

        explicit IOBuffer(IODevice& ioDevice, size_t bufferSize = 8192, bool extend = false)
        : _impl()
        {
            _impl.init(*this, bufferSize, extend);
            _impl.attach(*this, ioDevice); 
        }

        ~IOBuffer();

        IODevice* device()
        { return _impl.ioDevice(); }

        Signal<IOBuffer&>& inputReady()
        { return _impl.inputReady(); }

        Signal<IOBuffer&>& outputReady()
        { return _impl.outputReady(); }

        void attach(IODevice& ioDevice)
        { _impl.attach(*this, ioDevice); }

        void detach()
        { _impl.detach(*this); }

        void beginRead()
        { _impl.beginRead(*this); }

        void onRead(IODevice& dev)
        { _impl.onRead(*this); }

        size_t endRead()
        { return _impl.endRead(*this); }

        void beginWrite()
        { _impl.beginWrite(*this); }

        void onWrite(IODevice& dev)
        { _impl.onWrite(*this); }

        size_t endWrite()
        { return _impl.endWrite(*this); }

        void discard()
        { _impl.discard(*this); }

        bool isReading() const
        { return _impl.isReading(); }
        
        bool isWriting() const
        { return _impl.isWriting(); }

    protected:
        virtual std::streamsize showmanyc()
        { return _impl.showmanyc(*this); }

        virtual std::streamsize showfull()
        { return _impl.showfull(*this); }

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
        IOBufferImpl _impl;
};

} // namespace System

} // namespace Pt

#endif
