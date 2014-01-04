/***************************************************************************
 *   Copyright (C) 2006-2007 Laurentiu-Gheorghe Crisan                     *
 *   Copyright (C) 2006-2007 Marc Boris Duerner                            *
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
#ifndef Pt_System_win32_PipeImpl_h
#define Pt_System_win32_PipeImpl_h

#include "Pt/WinVer.h"
#include "Pt/System/Api.h"
#include <Pt/System/IODevice.h>
#include "IODeviceImpl.h"
#include <windows.h>

namespace Pt {

namespace System {

#ifdef _WIN32_WCE

class PipeIODevice : public IODevice, private IODeviceImpl
{
    public:
        enum Mode {Read, Write};
    
        PipeIODevice(Mode mode);
    
        ~PipeIODevice();
    
        void open(HANDLE handle);
    
    protected:
        void onSetTimeout(size_t timeout);
    
        void onCancel();
    
        //! @brief Closes the I/O device
        void onClose();
    
        bool onRun();
    
        size_t onBeginRead(EventLoop& loop, char* buffer, size_t n, bool& eof);
    
        size_t onEndRead(EventLoop& loop, char* buffer, size_t n, bool& eof);
    
        size_t onBeginWrite(EventLoop& loop, const char* buffer, size_t n);
    
        size_t onEndWrite(EventLoop& loop, const char* buffer, size_t n);
    
        //! @brief Read bytes from device
        size_t onRead(char* buffer, size_t count, bool& eof);
    
        //! @brief Write bytes to device
        size_t onWrite(const char* buffer, size_t count);
    
        void onSync() const;
    
    protected:
        void writeMessage(const char* buffer, size_t count);
    
    private:
        IOHandle    _ioh;
        std::size_t _timeout;
        Mode        _mode;
        DWORD       _msgSize;
        size_t      _bufferSize;
        std::vector<char> _buffer;
};

class PipeImpl
{
    public:
        PipeImpl();

        ~PipeImpl();

        IODevice& out();

        IODevice& in();

    private:
        PipeIODevice  _out;
        PipeIODevice  _in;
};

#else // normal WIN32

class PipeIODevice : public IODevice
{
    public:
        PipeIODevice();

        virtual ~PipeIODevice();

        virtual void open(HANDLE handle);

        HANDLE handle() const
        { return _impl.deviceHandle(); }

    protected:
        bool onRun();

        void onSetTimeout(size_t timeout);

        size_t onBeginRead(EventLoop& loop, char* buffer, size_t n, bool& eof);

        size_t onEndRead(EventLoop& loop, char* buffer, size_t n, bool& eof);

        size_t onBeginWrite(EventLoop& loop, const char* buffer, size_t n);

        size_t onEndWrite(EventLoop& loop, const char* buffer, size_t n);

        //! @brief Closes the I/O device
        virtual void onClose();

        //! @brief Read bytes from device
        virtual size_t onRead(char* buffer, size_t count, bool& eof);

        //! @brief Write bytes to device
        virtual size_t onWrite(const char* buffer, size_t count);

        virtual void onSync() const;

        virtual void onCancel() ;

     private:
        OverlappedIODeviceImpl _impl;
};

class PipeImpl
{
    public:
        PipeImpl();

        ~PipeImpl();

        PipeIODevice& in();

        PipeIODevice& out();

    private:
        PipeIODevice        _in;
        PipeIODevice        _out;
        static LONG _nameId;
};

#endif

} // namespace System

} // namespace Pt

#endif
