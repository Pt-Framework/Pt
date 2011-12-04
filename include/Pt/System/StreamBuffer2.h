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
#ifndef Pt_System_StreamBuffer2_h
#define Pt_System_StreamBuffer2_h

#include <Pt/System/Api.h>
#include <Pt/System/IODevice.h>
#include <streambuf>
#include <ios>
#include <cstring>

namespace Pt {

namespace System {
    class StreamBuffer2 : public std::streambuf
                        , public Connectable
    {
        private:
            friend class StreamBufferImpl;

            class PT_SYSTEM_API StreamBufferImpl {
                public:
                    StreamBufferImpl(size_t bufferSize, bool extend);
                    StreamBufferImpl(IODevice& ioDevice, size_t bufferSize, bool extend);
                    
                    IODevice* ioDevice()
                    { return _ioDevice; }

                    Signal<StreamBuffer2&> inputReady()
                    { return _inputReady; }

                    Signal<StreamBuffer2&> outputReady()
                    { return _outputReady; }

                    void streamBufferInit(StreamBuffer2& sb, size_t bufferSize, bool extend);
                    void streamBufferAttach(StreamBuffer2& sb, IODevice& ioDevice);

                    void streamBufferBeginRead(StreamBuffer2& sb);
                    void streamBufferOnRead(StreamBuffer2& sb);
                    void streamBufferEndRead(StreamBuffer2& sb);

                    size_t streamBufferBeginWrite(StreamBuffer2& sb);
                    void streamBufferOnWrite(StreamBuffer2& sb);
                    size_t streamBufferEndWrite(StreamBuffer2& sb);

                    void streamBufferDiscard(StreamBuffer2& sb);

                    std::streamsize streamBufferXspeekn(StreamBuffer2& sb, char* buffer, std::streamsize size);
                    std::streambuf::pos_type streamBufferSeekoff(StreamBuffer2& sb, std::streambuf::off_type off, std::ios::seekdir dir, std::ios::openmode);
                    std::streambuf::pos_type streamBufferSeekpos(StreamBuffer2& sb, std::streambuf::pos_type p, std::ios::openmode mode);
                    std::streamsize streamBufferShowfull(StreamBuffer2& sb);
                    std::streambuf::int_type streamBufferPbackfail(StreamBuffer2& sb, std::streambuf::int_type c);

                private:
                    IODevice*    _ioDevice;
                    size_t       _ibufferSize;
                    char*        _ibuffer;
                    std::size_t  _obufferSize;
                    char*        _obuffer;
                    const size_t _pbmax;
                    bool         _oextend;

                    Signal<StreamBuffer2&> _inputReady;
                    Signal<StreamBuffer2&> _outputReady;
            };

        public:
            explicit StreamBuffer2(size_t bufferSize = 8192, bool extend = false)
            : _impl(bufferSize, extend)
            { _impl.streamBufferInit(*this, bufferSize, extend); }
            
            explicit StreamBuffer2(IODevice& ioDevice, size_t bufferSize = 8192, bool extend = false)
            : _impl(bufferSize, extend)
            {
                _impl.streamBufferInit(*this, bufferSize, extend);
                _impl.streamBufferAttach(*this, ioDevice);
            }

            ~StreamBuffer2()
            {}

            ////////////////////////////////////////////////////////////////////////////////////////

            IODevice* device()
            { return _impl.ioDevice(); }

            Signal<StreamBuffer2&> inputReady()
            { return _impl.inputReady(); }

            Signal<StreamBuffer2&> outputReady()
            { return _impl.outputReady(); }

            std::streamsize out_avail()
            {
                if( this->pptr() )
                    return this->pptr() - this->pbase();

                return _impl.streamBufferShowfull(*this);
            }

            std::streamsize speekn(char* buffer, std::streamsize size)
            { return _impl.streamBufferXspeekn(*this, buffer, size); }

            ////////////////////////////////////////////////////////////////////////////////////////

            void attach(IODevice& ioDevice)
            { _impl.streamBufferAttach(*this, ioDevice); }

            void beginRead()
            { _impl.streamBufferBeginRead(*this); }

            void onRead(IODevice& dev)
            { _impl.streamBufferOnRead(*this); }

            void endRead()
            { _impl.streamBufferEndRead(*this); }

            void beginWrite()
            { _impl.streamBufferBeginWrite(*this); }

            void onWrite(IODevice& dev)
            { _impl.streamBufferOnWrite(*this); }

            void endWrite()
            { _impl.streamBufferEndWrite(*this); }

            void discard()
            { _impl.streamBufferDiscard(*this); }
            
            ////////////////////////////////////////////////////////////////////////////////////////

        protected:
           /* virtual int sync()
            { return StreamBufferSync(*this); }

            virtual int_type underflow()
            { return StreamBufferUnderflow(*this); }

            virtual int_type overflow(int_type ch)
            { return StreamBufferOverflow(*this, ch); }

            virtual pos_type seekoff(off_type offset, std::ios::seekdir sd, std::ios::openmode mode)
            { return StreamBufferSeekoff(*this, offset, sd, mode); }

            virtual pos_type seekpos(pos_type p, std::ios::openmode mode )
            { return StreamBufferSeekpos(*this, p, mode); }

            virtual int_type pbackfail(int_type c)
            { return StreamBufferPbackfail(*this, c); }
            */
/*

        size_t beginWrite()
        { return StreamBufferBeginWrite(*this); }

        void onWrite(IODevice& dev)
        { StreamBufferOnWrite(*this, dev); }

        size_t endWrite()
        { return StreamBufferEndWrite(*this); }

        void discard()
        { StreamBufferDiscard(*this); }
*/

        private:
            StreamBufferImpl _impl;
    };

} // namespace System

} // namespace Pt

#endif
