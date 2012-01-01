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
#ifndef PT_SYSTEM_IODEVICEIMPL_H
#define PT_SYSTEM_IODEVICEIMPL_H

#include "SelectableImpl.h"
#include <windows.h>

namespace Pt {

namespace System {

class IODevice;

class IODeviceImpl
{
    public:
        IODeviceImpl();

        virtual ~IODeviceImpl();

        void setHandle(HANDLE h);

        HANDLE deviceHandle() const
        { return _handle; }	

        HANDLE handle() const
        { return _handle; }	

        virtual void setWaitHandle(HANDLE h) {}

        virtual void close();

    private:
        HANDLE _handle;
};


/*class OverlappedIODeviceImpl : public IODeviceImpl
{
    public:
        OverlappedIODeviceImpl();

        virtual ~OverlappedIODeviceImpl();

        virtual void open(HANDLE handle, bool isAsync);

        //virtual bool checkEvent();

        virtual bool setWaitHandle(HANDLE h, bool& avail);

        virtual IODeviceImpl& ioimpl()
        { return *this; }

        void redirect(int newFd, bool close = true);

    protected:
        void onAttach(EventLoop& loop);

        void onDetach(EventLoop& loop);

        bool onAvail();

        size_t onBeginRead(char* buffer, size_t n, bool& eof);

        size_t onEndRead(bool& eof);

        size_t onBeginWrite(const char* buffer, size_t n);

        size_t onEndWrite();

        //bool onWait(std::size_t msecs);

        //! @brief Closes the I/O device
        virtual void onClose();

        //! @brief Read bytes from device
        virtual size_t onRead(char* buffer, size_t count, bool& eof);

        //! @brief Write bytes to device
        virtual size_t onWrite(const char* buffer, size_t count);

        virtual void onSync() const;

        virtual void onCancel() ;

     private:
        HANDLE _waitHandle;
        OVERLAPPED _readOv;
        OVERLAPPED _writeOv;
};*/

} //namespace System

} //namespace Pt

#endif
