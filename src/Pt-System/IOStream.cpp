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
#include "Pt/System/IOStream.h"

namespace Pt {

namespace System {

IStream::IStream(size_t bufferSize)
: _buffer(bufferSize)
{
    attachBuffer(&_buffer);
}


IStream::IStream(IODevice& device, size_t bufferSize)
: _buffer(device, bufferSize)
{
    attachBuffer(&_buffer);
}


IStream::~IStream()
{
}


StreamBuffer& IStream::buffer()
{
return _buffer;
}


IODevice* IStream::attachDevice(IODevice& device)
{
    IODevice* ret = attachedDevice();
    _buffer.attach(device);
    return ret;
}


IODevice* IStream::attachedDevice()
{
return _buffer.device();
}


OStream::OStream(size_t bufferSize)
: _buffer(bufferSize)
{
    attachBuffer(&_buffer);
}


OStream::OStream(IODevice& device, size_t bufferSize)
: _buffer(device, bufferSize)
{
    attachBuffer(&_buffer);
}


OStream::~OStream()
{
}


StreamBuffer& OStream::buffer()
{
    return _buffer;
}


IODevice* OStream::attachDevice(IODevice& device)
{
    IODevice* ret = attachedDevice();
    _buffer.attach(device);
    return ret;
}


IODevice* OStream::attachedDevice()
{
    return _buffer.device();
}


IOStream::IOStream(size_t bufferSize)
: _buffer(bufferSize)
{
    attachBuffer(&_buffer);
}


IOStream::~IOStream()
{
}


IOStream::IOStream(IODevice& device, size_t bufferSize)
: _buffer(device, bufferSize)
{
    attachBuffer(&_buffer);
}


StreamBuffer& IOStream::buffer()
{
    return _buffer;
}


IODevice* IOStream::attachDevice(IODevice& device)
{
    IODevice* ret = attachedDevice();
    _buffer.attach(device);
    return ret;
}


IODevice* IOStream::attachedDevice()
{
    return _buffer.device();
}

} // namespace System

} // namespace Pt


