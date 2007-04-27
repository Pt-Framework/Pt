/***************************************************************************
 *   Copyright (C) 2006-2007 Marc Boris Duerner                            *
 *   Copyright (C) 2006-2007 Laurentiu-Gheorghe Crisan                     *
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
#ifndef PT_SYSTEM_FILEDEVICEIMPL_H
#define PT_SYSTEM_FILEDEVICEIMPL_H

#include "Pt/Api.h"
#include "Pt/System/IOError.h"
#include "Pt/System/FileDevice.h"
#include "Pt/System/IODevice.h"
#include "IODeviceImpl.h"

#include <windows.h>
#include <ios>

namespace Pt {
namespace System {

class FileDeviceImpl  : public IODeviceImpl
{
    public:
        typedef FileDevice::pos_type pos_type;
        typedef FileDevice::off_type off_type;

    public:
        FileDeviceImpl();

        FileDeviceImpl( const char* path, std::ios_base::openmode mode, IODevice::ReadWriteMode rwMode );

        ~FileDeviceImpl();

        IOResult& beginRead(char* buffer, size_t n, bool& eof);

		size_t endRead(IOResult& result, bool& eof);

        void open( const char* path, std::ios_base::openmode mode, IODevice::ReadWriteMode rwMode );

        void close();

        pos_type seek( off_type offset, IODevice::SeekMode mode );

        size_t size();

        size_t read( char* buffer, size_t count, bool& eof );

        size_t write( const char* buffer, size_t count );

        size_t peek( char* buffer, size_t count );

        bool waitable() const;

        void sync() const;


        HANDLE deviceHandle() const
        { return _handle; }

        void eventHandles( std::vector<HANDLE>& handles, size_t waitMode );

        WaitResult waitResult( HANDLE handle );

    private:
        enum { Reading, Writing, Idle } _state;

        HANDLE          _handle;
        OVERLAPPED      _readOv;
        OVERLAPPED      _writeOv;
        IOResultImpl    _result;
};

}//namespace System
}//namespace Pt
#endif
