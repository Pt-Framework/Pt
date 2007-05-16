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
#include "Pt/Api.h"
#include "Pt/System/IOError.h"
#include "Pt/System/FileDevice.h"
#include "Pt/System/IODevice.h"
#include "IODeviceImpl.h"

namespace Pt {
namespace System {

class FileDeviceImpl : public IODeviceImpl
{
    public:
        typedef FileDevice::pos_type pos_type;
        typedef FileDevice::off_type off_type;

    public:
        FileDeviceImpl();
        ~FileDeviceImpl();

        void open(const char* path, std::ios_base::openmode mode, bool isAsync );

        bool seekable() const;

        pos_type seek(off_type offset, std::ios::seekdir sd);

        void resize(off_type size);

        size_t size();

        size_t peek(char* buffer, size_t count);

        int fd() const
        { return _fd; }

    private:
        int _fd;
};

} //namespace System
} //namespace Pt
