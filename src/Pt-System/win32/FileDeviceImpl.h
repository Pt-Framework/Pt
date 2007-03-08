/***************************************************************************
 *   Copyright (C) 2005-2006 by Marc Boris Dürner                          *
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
#include "Pt/Api.h"
#include "Pt/System/IOError.h"
#include "Pt/System/FileDevice.h"

#include <windows.h>
#include <ios>


namespace Pt {

namespace System {

    class FileDeviceImpl {
        public:
            typedef FileDevice::pos_type pos_type;
            typedef FileDevice::off_type off_type;

        public:
            FileDeviceImpl();

            FileDeviceImpl(const char* path, std::ios_base::openmode mode) throw(IOError);

            ~FileDeviceImpl() throw();

            void open(const char* path, std::ios_base::openmode mode) throw(IOError);

            void close() throw(IOError);

            pos_type seek(off_type offset, IODevice::SeekMode mode) throw(IOError);

            size_t size() throw(IOError);

            size_t read(char* buffer, size_t count, bool& eof) throw(IOError);

            size_t write(const char* buffer, size_t count) throw(IOError);

            size_t peek(char* buffer, size_t count) throw(IOError);

            void sync() const throw(IOError);

            bool wait(IODevice::WaitMode mode, unsigned int msec) throw(IOError);

        private:
            HANDLE _handle;
            OVERLAPPED _readOv;
            OVERLAPPED _writeOv;
    };

}

}
