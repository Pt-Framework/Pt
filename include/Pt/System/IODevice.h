/***************************************************************************
 *   Copyright (C) 2006-2007 Laurentiu-Gheorghe Crisan                     *
 *   Copyright (C) 2006-2007 Marc Boris Duerner                            *
 *   Copyright (C) 2006-2007 PTV AG                                        *
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
#ifndef Pt_System_IODevice_h
#define Pt_System_IODevice_h

#include <Pt/Types.h>
#include <Pt/Signal.h>
#include <Pt/System/Api.h>
#include <Pt/System/IOError.h>
#include <Pt/System/Selectable.h>
#include <limits>
#include <ios>

namespace Pt {

namespace System {

class IODeviceImpl;


struct IO
{
    enum OpenMode {
        Sync   = 0x0000,
        Async  = 0x0001,
        Read   = 0x0002,
        Write  = 0x0004,
        AtEnd  = 0x0008,
        Append = 0x0016,
        Trunc  = 0x0032
    };

    virtual ~IO()
    {}
};


/** @brief Endpoint for I/O operations

    This class serves as the base class for all kinds of I/O devices. The
    interface supports synchronous and asynchronous I/O operations, peeking
    and seeking. I/O buffers and I/O streams within the Pt framework use
    IODevices as endpoints and therefore fully feaured standard C++ compliant
    IOStreams can be constructed at runtime.
    Examples of %IODevices are the SerialDevice, the endpoints of a Pipe
    or the FileDevice. A Selector can be used to wait on activity on an
    %IODevice, which will send the %Singal inputReady or outputReady of the
    %IODevice that is ready to perform I/O.
*/
class IODevice : public IO, public Selectable
{
    friend class IODeviceImpl;

    public:
        typedef std::char_traits<char>::pos_type pos_type;
        typedef std::char_traits<char>::off_type off_type;

    public:
        //! @brief Destructor
        virtual ~IODevice()
        { }

        void beginRead(char* buffer, size_t n)
        {
            if ( ! async() )
                throw std::logic_error("Device not in async mode." + PT_SOURCEINFO);

            size_t r = this->onBeginRead(buffer, n, _eof);

            if(r > 0 || _eof || _wavail)
                this->setState(Selectable::Avail);
            else
                this->setState(Selectable::Busy);

            _rbuf = buffer;
            _rbuflen = n;
            _ravail = r;
        }

        size_t endRead()
        {
            size_t n = this->onEndRead(_eof);
            
            if(_wavail > 0)
                this->setState(Selectable::Avail);
            else if(_wbuf)
                this->setState(Selectable::Busy);
            else
                this->setState(Selectable::Idle);

            _rbuf = 0;
            _rbuflen = 0;
            _ravail = 0;
            return n;
        }

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
        size_t read(char* buffer, size_t n)
        {
            if ( async() )
            {
                this->beginRead(buffer, n);
                this->wait();
                return endRead();
            }

            return this->onRead(buffer, n, _eof);
        }

        void beginWrite(const char* buffer, size_t n)
        {
            if ( ! async() )
                throw std::logic_error("Device not in async mode." + PT_SOURCEINFO);
            
            size_t r = this->onBeginWrite(buffer, n);
            
            if(r > 0 || _ravail)
                this->setState(Selectable::Avail);
            else
                this->setState(Selectable::Busy);
            
            _wbuf = buffer;
            _wbuflen = n;
            _wavail = r;
        }

        size_t endWrite()
        {   
            size_t n =  onEndWrite();
            
            if(_ravail > 0 || (_rbuf && _eof) )
                this->setState(Selectable::Avail);
            else if(_rbuf)
                this->setState(Selectable::Busy);
            else
                this->setState(Selectable::Idle);

            _wbuf = 0;
            _wbuflen = 0;
            _wavail = 0;
            return n;
        }

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
        size_t write(const char* buffer, size_t n)
        {
            if ( async() )
            {
                this->beginWrite(buffer, n);
                this->wait();
                return endWrite();
            }

            return this->onWrite(buffer, n);
        }

        //! @brief Returns true if device is seekable
        /**
            Tests if the device is seekable.

            \return true if the device is seekable, false otherwise.
        */
        bool seekable() const
        { return onSeekable(); }

        //! @brief Move the next read position to the given offset
        /**
            Tries to move the current read position to the given offset.
            SeekMode determines the relative starting point of offset.

            \param offset the offset the pointer should be moved by.
            \param mode determines the relative starting offset.
            \return the new abosulte read positing.
            \throw IOError
        */
        pos_type seek(off_type offset, std::ios::seekdir sd)
        {
            off_type ret = this->onSeek(offset, sd);
            if( ret != off_type(-1) )
                setEof(false);

            return ret;
        }

        //! @brief Read data from I/O device without consuming them
        /**
            Tries to extract up to n bytes from this object
            without consuming them. The bytes are stored in
            buffer, and the number of bytes peeked is returned.

            \param buffer buffer where to place the data to be read.
            \param n number of bytes to peek
            \return number of bytes peek.
            \throw IOError
        */
        size_t peek(char* buffer, size_t n)
        { return this->onPeek(buffer, n); }

        //! @brief Synchronize device
        /**
            Commits written data to physical device.

            \throw IOError
        */
        void sync()
        { return this->onSync(); }

        //! @brief Returns the current I/O position
        /**
            The current I/O position is returned or an IOError
            is thrown if the device is not seekable. Seekability
            can be tested with BasicIODevice::seekable().

            \throw IOError
        */
        pos_type position()
        { return this->seek(0, std::ios::cur); }

        //! @brief Returns if the device has reached EOF
        /*!
            Test if the I/O device has reached eof.

            \return true if the I/O device is usable, false otherwise.
        */
        bool eof() const
        { return _eof; }

        /** @brief Returns true if the device operates in asynchronous mode
        */
        bool async() const
        { return _async; }

        /** @brief Notifies about availavle data

            This signal is send when the IODevice is monitored
            in a Selector or EventLoop and data becomes available.
        */
        Signal<IODevice&> inputReady;

        /** @brief Notifies when data can be written

            This signal is send when the IODevice is monitored
            in a Selector or EventLoop and the device is ready
            to write data.
        */
        Signal<IODevice&> outputReady;
 
        /** @brief Notifies when an error occured

            This signal is send when the device is monitored
            in a Selector or EventLoop and an error occured 
            on the device.
        */
        Signal<IODevice&> errorOccured;

        virtual IODeviceImpl& ioimpl() = 0;

    protected:
        //! @brief Default Constructor
        IODevice()
        : _eof(false)
        , _async(false)
        , _rbuf(0)
        , _rbuflen(0)
        , _ravail(0)
        , _wbuf(0)
        , _wbuflen(0)
        , _wavail(0)
        { }

        virtual size_t onBeginRead(char* buffer, size_t n, bool& eof) = 0;

        virtual size_t onEndRead(bool& eof) = 0;

        //! @brief Read bytes from device
        virtual size_t onRead(char* buffer, size_t count, bool& eof) = 0;

        virtual size_t onBeginWrite(const char* buffer, size_t n) = 0;

        virtual size_t onEndWrite() = 0;

        //! @brief Write bytes to device
        virtual size_t onWrite(const char* buffer, size_t count) = 0;

        //! @brief Read data from I/O device without consuming them
        virtual size_t onPeek(char*, size_t)
        { return 0; }

        //! @brief Returns true if device is seekable
        virtual bool onSeekable() const
        { return false; }

        //! @brief Move the next read position to the given offset
        virtual pos_type onSeek(off_type, std::ios::seekdir)
        { throw IOError("Could not seek on device.", PT_SOURCEINFO); }

        //! @brief Synchronize device
        virtual void onSync() const
        { }

        //! @brief Returns the size of the device
        virtual size_t onSize() const
        { return 0; }

        //! @brief Sets or unsets the device to eof
        void setEof(bool eof)
        { _eof = eof; }

        //! @brief Sets or unsets the device to eof
        void setAsync(bool async)
        { _async = async; }

    private:
        bool _eof;
        bool _async;
        
    protected:
        char* _rbuf;
        size_t _rbuflen;
        size_t _ravail;
        const char* _wbuf;
        size_t _wbuflen;
        size_t _wavail;
};

//! @internal provide import information for linking DLLs
class PT_SYSTEM_API DummyIODevice : public IODevice
{
    public:
      DummyIODevice();
};

} // namespace System

} // namespace Pt

#endif
