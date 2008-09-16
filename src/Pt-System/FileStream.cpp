/***************************************************************************
 *   Copyright (C) 2005 Marc Boris Duerner                                 *
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
#include "Pt/System/FileStream.h"
#include "Pt/System/Selector.h"

namespace Pt {

namespace System {

FileBuffer::FileBuffer(const char* s, IODevice::OpenMode mode)
: _file(s, mode)
{
    this->attach(_file);
}


FileBuffer::~FileBuffer()
{
}


void FileBuffer::setSelector(SelectorBase* sel)
{
    _file.setSelector(sel);
}


void FileBuffer::open(const char* s, IODevice::OpenMode mode)
{
    _file.open(s, mode);
}


void FileBuffer::close()
{
    _file.close();
}


bool FileBuffer::is_open()
{
    return _file.enabled();
}



FileStream::FileStream(const char* s, IODevice::OpenMode mode)
: _buffer(s, mode)
{
    this->setBuffer(&_buffer);
}


FileStream::~FileStream()
{
    try {
        IOStream::sync();
    }
    catch(...) {}
}


void FileStream::open(const char* s, IODevice::OpenMode mode)
{
    _buffer.open(s, mode);
}


void FileStream::close()
{
    _buffer.close();
}


bool FileStream::is_open()
{
    return _buffer.is_open();
}


FileBuffer* FileStream::rdbuf()
{
    return &_buffer;
}

} // namespace System

} // namespace Pt


