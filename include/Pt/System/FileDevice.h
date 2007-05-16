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
#ifndef Pt_System_FileDevice_h
#define Pt_System_FileDevice_h

#include <Pt/NonCopyable.h>
#include <Pt/System/IODevice.h>
#include <Pt/System/Api.h>

namespace Pt {

namespace System {

class PT_SYSTEM_API FileDevice : public IODevice 
{
    private:
        class FileDeviceImpl* _impl;

    public:
        FileDevice();

        FileDevice( const char* path, std::ios_base::openmode mode, bool m = Sync );

        ~FileDevice();

        void open( const char* path, std::ios_base::openmode mode, bool m = Sync );

        const char* path() const
        { return _path.c_str(); }

        std::ios_base::openmode openMode() const
        { return _mode; }

        size_t size() const;

        virtual IODeviceImpl* impl()
        { return (IODeviceImpl*) _impl; }

    protected:
        IOResult& _beginRead(char* buffer, size_t n, bool& eof);

        size_t _endRead(IOResult& result, bool& eof);

        IOResult& _beginWrite(const char* buffer, size_t n);

        size_t _endWrite(IOResult& result);

        void _close();

        bool _waitable() const;

        bool _seekable() const
        { return true; }

        pos_type _seek(off_type offset, std::ios::seekdir sd) ;

        size_t _read(char* buffer, size_t count, bool& eof);

        size_t _write(const char* buffer, size_t count);

        size_t _peek(char* buffer, size_t count);

        void _sync() const;

    private:
        std::string             _path;
        std::ios_base::openmode _mode;
};

} // namespace System
} // namespace Pt

#endif
