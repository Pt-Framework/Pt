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
#ifndef Pt_System_posix_PipeImpl_h
#define Pt_System_posix_PipeImpl_h

#include "IODeviceImpl.h"
#include <Pt/System/Api.h>
#include <Pt/System/IODevice.h>
#include <unistd.h>

namespace Pt {

namespace System {

class PipeIODevice : public Pt::System::IODevice, private IODeviceImpl
{
    public:
        PipeIODevice();

        ~PipeIODevice();

        void open(int fd, bool isAsync);

    protected:
        void _close()
        { IODeviceImpl::close(); }

        IOResult& _beginRead(char* buffer, size_t n, bool& eof);

        size_t _endRead(IOResult& result, bool& eof);

        virtual size_t _read(char* buffer, size_t count, bool& eof);

        virtual IOResult& _beginWrite(const char* buffer, size_t n);

        virtual size_t _endWrite(IOResult& result);

        virtual size_t _write(const char* buffer, size_t count);

        virtual void _sync() const;

		virtual IODeviceImpl* impl(){ return this; }        
};


class PipeImpl
{
    public:
        PipeImpl(bool isAsync);

        ~PipeImpl();

        IODevice& input();

        IODevice& output();

    private:
        PipeIODevice _input;
        PipeIODevice _output;
};

} // namespace System

} // namespace Pt

#endif
