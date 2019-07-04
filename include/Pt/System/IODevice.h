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
    and seeking. I/O buffers and I/O streams within the %Pt framework use
    IODevices as endpoints and therefore fully feaured standard C++ compliant
    IOStreams can be constructed at runtime.
    Examples of IODevices are the SerialDevice, the endpoints of a Pipe
    or the FileDevice. An EventLoop can be used to wait on activity on an
    %IODevice. The signals inputReady or outputReady of the %IODevice  indicate
    that an I/O operation has finished.
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

        /** @brief Closes the device.
        */
        void close();

        /** @brief Sets the timeout for blocking I/O in milliseconds.
        */
        void setTimeout(std::size_t timeout);

        /** @brief Begins to read data.
        */
        void beginRead(char* buffer, std::size_t n);

        /** @brief Ends reading data.
        */
        std::size_t endRead();

        /** @brief Read data from I/O device

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

        /** @brief Begins to write data.
        */
        void beginWrite(const char* buffer, std::size_t n);

        /** @brief Ends writing data.
        */
        std::size_t endWrite();

        /** @brief Write data to I/O device

            Writes @a n bytes from @a buffer to this I/O device. Returns the
            number of bytes written, which may be less than requested. In case
            of EOF the %IODevice is set to eof.

            @throw IOError
         */
        std::size_t write(const char* buffer, std::size_t n);

        /** @brief Returns true if device is seekable
        */
        bool seekable() const;

        /** @brief Moves the read position to the given offset.

            @throw IOError
        */
        pos_type seek(off_type offset, seekdir sd);

        /** @brief Peek data from I/O device without consuming them.

            @todo deprecate this method
            @throw IOError
        */
        std::size_t peek(char* buffer, std::size_t n);

        /** @brief Synchronize device.

            Commits written data to physical device.

            @throw IOError
        */
        void sync();

        /** @brief Returns the current I/O position.

            The current I/O position is returned or an IOError
            is thrown if the device is not seekable. Seekability
            can be tested with seekable().

            @throw IOError
        */
        pos_type position();

        /** @brief Returns if the device has reached EOF.
        */
        bool isEof() const;

        /** @brief Notifies about available data.

            This signal is send when the IODevice is monitored
            in an EventLoop and data becomes available.
        */
        Signal<IODevice&>& inputReady()
        { return _inputReady; }

        /** @brief Notifies when data can be written.

            This signal is send when the IODevice is monitored
            in an EventLoop and the device is ready
            to write data.
        */
        Signal<IODevice&>& outputReady()
        { return _outputReady; }

        /** @brief Returns true if the device is reading.
        */
        bool isReading() const
        { return _rbuf != 0; }

        /** @brief Returns true if the device is writing.
        */
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

        /** @brief Returns the used event loop.
        */
        EventLoop* loop() const
         { return _loop; }

    protected:
        //! @brief Default Constructor
        IODevice();

        virtual void onClose() = 0;

        virtual void onSetTimeout(std::size_t timeout) = 0;

        virtual std::size_t onBeginRead(EventLoop& loop, char* buffer, std::size_t n, bool& eof) = 0;

        virtual std::size_t onEndRead(EventLoop& loop, char* buffer, std::size_t n, bool& eof) = 0;

        virtual std::size_t onRead(char* buffer, std::size_t count, bool& eof) = 0;

        virtual std::size_t onBeginWrite(EventLoop& loop, const char* buffer, std::size_t n) = 0;

        virtual std::size_t onEndWrite(EventLoop& loop, const char* buffer, std::size_t n) = 0;

        virtual std::size_t onWrite(const char* buffer, std::size_t count) = 0;

        virtual std::size_t onPeek(char*, std::size_t)
        { return 0; }

        virtual bool onSeekable() const
        { return false; }

        virtual pos_type onSeek(off_type, std::ios::seekdir)
        { throw IOError("Could not seek on device"); }

        virtual void onSync() const
        { }

        void setEof(bool eof);

        virtual void onAttach(EventLoop& loop);

        virtual void onDetach(EventLoop& loop);

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
