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
#include "Pt/System/IODevice.h"
#include <string>
#include <iostream>

namespace Pt {

namespace System {

    class IODeviceImpl : public SelectableImpl
    {
        public:
            IODeviceImpl(IODevice& device);

            virtual ~IODeviceImpl();

            int fd() const
            { return _fd; }

            virtual void open(const std::string& path, std::ios_base::openmode mode, bool isAsync);

            virtual void open(int fd, bool isAsync);

            virtual void close();

            virtual bool wait(unsigned int msecs);

            virtual void beginRead(char* buffer, size_t n, bool& eof);

            virtual size_t endRead( bool& eof);

            virtual size_t read( char* buffer, size_t count, bool& eof );

            virtual void beginWrite(const char* buffer, size_t n);

            virtual size_t endWrite();

            virtual size_t write( const char* buffer, size_t count );

            virtual void sync() const;

            virtual int initSelect(fd_set& rfds, fd_set& wfds, fd_set& efds);

            virtual void exitSelect(fd_set& rfds, fd_set& wfds, fd_set& efds);

            virtual int checkEvent(fd_set& rfds, fd_set& wfds, fd_set& efds);

            void attach(SelectorBase& s)
            {}

            void detach(SelectorBase& s)
            {}

        protected:
            IODevice& _device;
            int _fd;
    };

} //namespace System

} //namespace Pt

#endif
