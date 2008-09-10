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

FileBuffer::FileBuffer(const char* name, std::ios_base::openmode omode, bool async)
: _file(name, omode, async)
{
    IOBuffer::attach(_file);
}


FileBuffer::~FileBuffer()
{
}


void FileBuffer::setSelector(SelectorBase* sel)
{
    _file.setSelector(sel);
}


void FileBuffer::close()
{
    _file.close();
}


FileStream::FileStream(const char* path, std::ios_base::openmode omode, bool async)
: _buffer(path, omode, async)
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


void FileStream::setSelector(SelectorBase* sel)
{
    _buffer.setSelector(sel);
}


void FileStream::close()
{
    _buffer.close();
}

} // namespace System

} // namespace Pt


