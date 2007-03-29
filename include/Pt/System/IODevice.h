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

#include <Pt/System/Api.h>
#include <Pt/Types.h>
#include <Pt/NonCopyable.h>
#include <Pt/System/IOError.h>

#include <limits>
#include <ios>


namespace Pt {

namespace System {

class IODeviceImpl;

/** @brief I/O Device base class

    This class serves as the base class for all kinds of I/O devices. The
    interface supports synchronous and asynchronous I/O operations. Some
    I/O buffers and I/O streams within use IODevices as endpoints. At
    least _read(), _write() and _close() must be implemented for an IODevice
    to be used.
*/
template <typename CharT>
class BasicIODevice : public NonCopyable {
    public:
        typedef typename std::char_traits<CharT>::pos_type pos_type;
        typedef typename std::char_traits<CharT>::off_type off_type;

    public:

        enum SeekMode {
            SeekCurrent,
            SeekBegin,
            SeekEnd
        };

        enum ReadWriteMode
        {
            Synchronous = 1,
            Asynchronous
        };
        
    public:
        //! @brief Default Constructor
        BasicIODevice()
        : _valid(false)
        , _eof(false)
        { }

        //! @brief Destructor
        virtual ~BasicIODevice()
        { }

        //! @brief Closes the I/O device
        /*!
           Frees any resources associated with this object, like I/O handles.
        */
        void close()
        {
            if( this->valid() ) {
                _close();
                _valid = false;
            }
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
        size_t read(CharT* buffer, size_t n)
        {
            size_t ret = this->_read(buffer, n, _eof);
            return ret;
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
        size_t write(const CharT* buffer, size_t n)
        { return this->_write(buffer, n); }

        //! @brief Returns true if device is seekable
        /**
            Tests if the device is seekable.

            \return true if the device is seekable, false otherwise.
        */
        bool seekable() const
        { return _seekable(); }

        //! @brief Returns true if device accesses remote data.
        /**
            Tests if the device is on a remote location i.e. a network device.

            \return true if the device is remote, false otherwise.
        */
        bool remote() const
        { return _remote(); }

        //! @brief Move the next read position to the given offset
        /**
            Tries to move the current read position to the given offset.
            SeekMode determines the relative starting point of offset.

            \param offset the offset the pointer should be moved by.
            \param mode determines the relative starting offset.
            \return the new abosulte read positing.
            \throw IOError
        */
        pos_type seek(off_type offset, SeekMode mode)
        {
            off_type ret = this->_seek(offset, mode);
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
        size_t peek(CharT* buffer, size_t n)
        { return this->_peek(buffer, n); }

        //! @brief Synchronize device
        /**
            Commits written data to physical device.

            \throw IOError
        */
        void sync()
        { return this->_sync(); }

        //! @brief Returns the current I/O position
        /**
            The current I/O position is returned or an IOError
            is thrown if the device is not seekable. Seekability
            can be tested with BasicIODevice::seekable().

            \throw IOError
        */
        pos_type position()
        { return this->seek(0, SeekCurrent); }

        //! @brief Test if the I/O device object is valid
        /*!
            Test if the I/O device object is valid i.e. open and ready
            to perform I/O operations

            \return true if the I/O device is usable, false otherwise.
        */
        bool valid() const
        { return _valid; }

        //! @brief Returns if the device has reached EOF
        /*!
            Test if the I/O device has reached eof.

            \return true if the I/O device is usable, false otherwise.
        */
        bool eof() const
        { return _eof; }


        virtual IODeviceImpl* impl() = 0;

    protected:
        //! @brief Closes the I/O device
        virtual void _close() = 0;

        //! @brief Read bytes from device
        virtual size_t _read(CharT* buffer, size_t count, bool& eof) = 0;

        //! @brief Write bytes to device
        virtual size_t _write(const CharT* buffer, size_t count) = 0;

        //! @brief Read data from I/O device without consuming them
        virtual size_t _peek(CharT*, size_t)
        { return 0; }

        //! @brief Returns true if device is seekable
        virtual bool _seekable() const
        { return false; }

        //! @brief Returns true if device is remote
        virtual bool _remote() const
        { return false; }

        //! @brief Move the next read position to the given offset
        virtual pos_type _seek(off_type, SeekMode)
        { throw IOError("Could not seek on device.", PT_SOURCEINFO); }

        //! @brief Synchronize device
        virtual void _sync() const
        { }

        //! @brief Returns the size of the device
        virtual size_t _size() const
        { return 0; }

        //! @brief Sets or unsets the device invalid
        void setValid(bool v)
        { _valid = v; }

        //! @brief Sets or unsets the device to eof
        void setEof(bool eof)
        { _eof = eof; }

    private:
        bool _valid;
        bool _eof;
};

typedef BasicIODevice<char> IODevice;

} // namespace System

} // namespace Pt

#endif
