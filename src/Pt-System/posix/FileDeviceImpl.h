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
#include <cstdint>

namespace Pt {

namespace System {

class FileDeviceImpl : public IODeviceImpl
{
    public:
        typedef FileDevice::pos_type pos_type;
        typedef FileDevice::off_type off_type;

    public:
        FileDeviceImpl(FileDevice& device);

        ~FileDeviceImpl();

        void open(const Path& path, std::ios::openmode mode);

        bool beginOpen(EventLoop& loop, const Path& path, std::ios::openmode mode);

        bool runOpen(EventLoop& loop);

        void endOpen(EventLoop& loop);

        bool seekable() const;

        pos_type seek(off_type offset, std::ios::seekdir sd);

        void resize(off_type size);

        size_t peek(char* buffer, size_t count);

    protected:
        std::size_t onBeginRead(EventLoop& loop, char* buffer,
                                std::size_t count, bool& eof);

        std::size_t onEndRead(EventLoop& loop, char* buffer,
                              std::size_t count, bool& eof);

        std::size_t onRead(char* buffer, std::size_t count, bool& eof);

        std::size_t onBeginWrite(EventLoop& loop, const char* buffer,
                                 std::size_t count);

        std::size_t onEndWrite(EventLoop& loop, const char* buffer,
                               std::size_t count);

        std::size_t onWrite(const char* buffer, std::size_t count);

    private:
        std::uint64_t advance(std::size_t count);

        void setPosition(std::uint64_t position);

    private:
        std::uint64_t _pos;
        std::uint64_t _readPos;
        std::uint64_t _writePos;
};

} //namespace System

} //namespace Pt
