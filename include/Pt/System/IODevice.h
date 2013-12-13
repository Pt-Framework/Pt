/*
 * Copyright (C) 2006-2013 Marc Boris Duerner
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

#ifndef Pt_System_IODevice_h
#define Pt_System_IODevice_h

#include <Pt/System/Api.h>
#include <Pt/System/IOError.h>
#include <Pt/System/Selectable.h>
#include <Pt/Types.h>
#include <Pt/Signal.h>
#include <ios>

namespace Pt {

namespace System {

/** @brief Endpoint for I/O operations

    This class serves as the base class for all kinds of I/O devices. The
    interface supports synchronous and asynchronous I/O operations, peeking
    and seeking. I/O buffers and I/O streams within the Pt framework use
    IODevices as endpoints and therefore fully feaured standard C++ compliant
    IOStreams can be constructed at runtime.
    Examples of %IODevices are the SerialDevice, the endpoints of a Pipe
    or the FileDevice. An EventLoop can be used to wait on activity on an
    %IODevice, which will send the %Signal inputReady or outputReady of the
    %IODevice that is ready to perform I/O.
*/
class PT_SYSTEM_API IODevice : public Selectable
{
    public:
        typedef std::char_traits<char>::pos_type pos_type;
        typedef std::char_traits<char>::off_type off_type;
        typedef std::ios_base::seekdir seekdir;

    public:
        //! @brief Destructor
        virtual ~IODevice();

        void close();

        void setTimeout(std::size_t timeout);

        void beginRead(char* buffer, std::size_t n);

        std::size_t endRead();

        //! @brief Read data from I/O device
        /*!
            Reads up to n bytes and stores them in buffer. Returns the number
            of bytes read, which may be less than requested and even 0 if the
            device operates in asynchronous (non-blocking) mode. In case of
            EOF the IODevice is set to eof.

            \param buffer buffer where to place the data to be read.
            \param n number of bytes to read
            \return number of bytes read, which may be less than requested.
            \throw IOError
         */
        std::size_t read(char* buffer, std::size_t n);

        void beginWrite(const char* buffer, std::size_t n);

        std::size_t endWrite();

        //! @brief Write data to I/O device
        /**
            Writes n bytes from buffer to this I/O device. Returns the number
            of bytes written, which may be less than requested and even 0 if the
            device operates in asynchronous (non-blocking) mode. In case of
            EOF the IODevice is set to eof.

            \param buffer buffer containing the data to be written.
            \param n number of bytes that should be written.
            \return number of bytes written, which may be less than requested.
            \throw IOError
         */
        std::size_t write(const char* buffer, std::size_t n);

        //! @brief Returns true if device is seekable
        /**
            Tests if the device is seekable.

            \return true if the device is seekable, false otherwise.
        */
        bool seekable() const;

        /** @brief Move the next read position to the given offset

            Tries to move the current read position to the given offset.
            SeekMode determines the relative starting point of offset.

            \param offset Offset the pointer should be moved by.
            \param sd The seek mode.
            \return New abosulte read positing.
            \throw IOError
        */
        pos_type seek(off_type offset, seekdir sd);

        //! @brief Read data from I/O device without consuming them
        /**
            TODO: deprecate this method

            Tries to extract up to n bytes from this object
            without consuming them. The bytes are stored in
            buffer, and the number of bytes peeked is returned.

            \param buffer buffer where to place the data to be read.
            \param n number of bytes to peek
            \return number of bytes peek.
            \throw IOError
        */
        std::size_t peek(char* buffer, std::size_t n);

        //! @brief Synchronize device
        /**
            Commits written data to physical device.

            \throw IOError
        */
        void sync();

        //! @brief Returns the current I/O position
        /**
            The current I/O position is returned or an IOError
            is thrown if the device is not seekable. Seekability
            can be tested with BasicIODevice::seekable().

            \throw IOError
        */
        pos_type position();

        //! @brief Returns if the device has reached EOF
        /*!
            Test if the I/O device has reached eof.

            \return true if the I/O device is usable, false otherwise.
        */
        bool isEof() const;

        /** @brief Notifies about available data

            This signal is send when the IODevice is monitored
            in a Selector or EventLoop and data becomes available.
        */
        Signal<IODevice&>& inputReady()
        { return _inputReady; }

        /** @brief Notifies when data can be written

            This signal is send when the IODevice is monitored
            in a Selector or EventLoop and the device is ready
            to write data.
        */
        Signal<IODevice&>& outputReady()
        { return _outputReady; }

        bool isReading() const
        { return _rbuf != 0; }

        bool isWriting() const
        { return _wbuf != 0; }

        char* rbuf() const
        { return _rbuf; }

        std::size_t rbuflen() const
        { return _rbuflen; }

        std::size_t ravail() const
        { return _ravail; }

        const char* wbuf() const
        { return _wbuf; }

        std::size_t wbuflen() const
        { return _wbuflen; }

        std::size_t wavail() const
        { return _wavail; }

        //! @internal
        EventLoop* loop() const
         { return _loop; }

    protected:
        //! @brief Default Constructor
        IODevice();

        //! @brief Closes all resources and cancels any outstanding operations
        virtual void onClose() = 0;

        virtual void onSetTimeout(std::size_t timeout) = 0;

        virtual std::size_t onBeginRead(EventLoop& loop, char* buffer, std::size_t n, bool& eof) = 0;

        virtual std::size_t onEndRead(EventLoop& loop, char* buffer, std::size_t n, bool& eof) = 0;

        //! @brief Read bytes from device
        virtual std::size_t onRead(char* buffer, std::size_t count, bool& eof) = 0;

        virtual std::size_t onBeginWrite(EventLoop& loop, const char* buffer, std::size_t n) = 0;

        virtual std::size_t onEndWrite(EventLoop& loop, const char* buffer, std::size_t n) = 0;

        //! @brief Write bytes to device
        virtual std::size_t onWrite(const char* buffer, std::size_t count) = 0;

        //! @brief Read data from I/O device without consuming them
        virtual std::size_t onPeek(char* buffer, std::size_t)
        { return 0; }

        //! @brief Returns true if device is seekable
        virtual bool onSeekable() const
        { return false; }

        //! @brief Move the next read position to the given offset
        virtual pos_type onSeek(off_type, std::ios::seekdir)
        { throw IOError("Could not seek on device"); }

        //! @brief Synchronize device
        virtual void onSync() const
        { }

        //! @brief Sets or unsets the device to eof
        void setEof(bool eof);

        virtual void onAttach(EventLoop& loop);

        virtual void onDetach(EventLoop& loop);

        //! @brief Cancel all I/O operations.
        virtual void onCancel();

    protected:
        EventLoop* _loop;
        char* _rbuf;
        std::size_t _rbuflen;
        std::size_t _ravail;
        const char* _wbuf;
        std::size_t _wbuflen;
        std::size_t _wavail;
        Signal<IODevice&> _inputReady;
        Signal<IODevice&> _outputReady;
        Pt::varint_t _reserved;

    private:
        bool _eof;
};

} // namespace System

} // namespace Pt

#endif // Pt_System_IODevice_h
