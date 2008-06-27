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
#ifndef Pt_System_win32_PipeImpl_h
#define Pt_System_win32_PipeImpl_h

#include <Pt/System/Api.h>
#include <Pt/System/IODevice.h>
#include "IODeviceImpl.h"
#include "ReadResult.h"
#include <windows.h>

namespace Pt {

namespace System {

class PipeIODevice : public IODevice, private IODeviceImpl
{
    public:
        PipeIODevice();

        virtual ~PipeIODevice();

        virtual void open(HANDLE handle, bool isAsync);

        virtual HANDLE deviceHandle() const;        

        virtual IODeviceImpl* impl()
        { return this; }

    protected:
        IOResult& onBeginRead(char* buffer, size_t n, bool& eof);

		size_t onEndRead(IOResult& resule, bool& eof);

        IOResult& onBeginWrite(const char* buffer, size_t n);

        size_t onEndWrite(IOResult& result);

        //! @brief Closes the I/O device
        virtual void onClose();

        //! @brief Read bytes from device
        virtual size_t onRead(char* buffer, size_t count, bool& eof);

        //! @brief Write bytes to device
        virtual size_t onWrite(const char* buffer, size_t count);

        virtual bool _waitable() const
        { return true; }

        virtual void onSync() const;

     private:
     	ReadResult _readResult;
        HANDLE     _handle;
        OVERLAPPED _readOv;
        OVERLAPPED _writeOv;
};

class PipeImpl
{
    public:
        PipeImpl(bool isAsync);

        ~PipeImpl();

        IODevice& input();

        IODevice& output();

    private:
        PipeIODevice        _inputDevice;
        PipeIODevice        _outputDevice;
        static Pt::uint32_t _nameId;
};

} // namespace System

} // namespace Pt

#endif
