/*
 * Copyright (C) 2005-2013 Marc Boris Duerner
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
#include <Pt/Signal.h>
#include <Pt/StreamBuffer.h>

namespace Pt {

namespace System {

/** @brief Implements std::streambuf for I/O devices.
*/
class PT_SYSTEM_API IOBuffer : public BasicStreamBuffer<char>
                             , public Connectable
{
    public:
        /** @brief Construct with buffer size.
        */
        explicit IOBuffer(std::size_t bufferSize = 8192, bool extend = false);

        /** @brief Construct with I/O device.
        */
        explicit IOBuffer(IODevice& ioDevice, std::size_t bufferSize = 8192, bool extend = false);

        /** @brief Destructor.
        */
        ~IOBuffer();

        /** @brief Returns the I/O device.
        */
        IODevice* device()
        { return _ioDevice; }

        /** @brief Notifies when input was read.
        */
        Signal<IOBuffer&>& inputReady()
        { return _inputReady; }

        /** @brief Notifies when a part of the buffer was written.
        */
        Signal<IOBuffer&>& outputReady()
        { return _outputReady; }

        /** @brief Attach to I/O device.
        */
        void attach(IODevice& ioDevice);

        /** @brief Detach from I/O device.
        */
        void detach();
        
        /** @brief Discards and detaches.
        */
        void reset();

        /** @brief Discards the buffer.
        */
        void discard();
        
        /** @brief Begins to read from the I/O device into the buffer.
        */
        void beginRead();

        //! @internal
        void onRead(IODevice& dev);

        /** @brief Ends to read.
        */
        std::size_t endRead();

        /** @brief Begins to write buffered data to the I/O device.
        */
        void beginWrite();

        //! @internal
        void onWrite(IODevice& dev);

        std::size_t endWrite();

        /** @brief Returns true if a read operation is running.
        */
        bool isReading() const;
        
        /** @brief Returns true if a write operation is running.
        */
        bool isWriting() const;

    protected:
        //! @internal
        void init(std::size_t bufferSize, bool extend);

        // inherit docs
        virtual std::streamsize showmanyc();

        // inherit docs
        virtual std::streamsize showfull();

        // inherit docs
        virtual int sync();

        // inherit docs
        virtual int_type underflow();

        // inherit docs
        virtual int_type overflow(int_type ch);

        // inherit docs
        virtual pos_type seekoff(off_type offset, std::ios::seekdir sd, std::ios::openmode mode);

        // inherit docs
        virtual pos_type seekpos(pos_type p, std::ios::openmode mode );

        // inherit docs
        virtual int_type pbackfail(int_type c);

    private:
        Signal<IOBuffer&> _inputReady;
        Signal<IOBuffer&> _outputReady;
        IODevice*    _ioDevice;
        std::size_t       _ibufferSize;
        char*        _ibuffer;
        std::size_t  _obufferSize;
        char*        _obuffer;
        bool         _oextend;

        static const int _pbmax = 4;
};

} // namespace System

} // namespace Pt

#endif // Pt_System_IOBuffer_h
