/*
 * Copyright (C) 2007 Marc Boris D�rner
 * Copyright (C) 2007 Laurentiu-Gheorghe Crisan
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#include "SerialDeviceImpl.h"
#include "Pt/System/IOError.h"

namespace Pt {

namespace System {

SerialDeviceImpl::SerialDeviceImpl(IODevice&)
{ 
}


SerialDeviceImpl::~SerialDeviceImpl()
{ 
}


void SerialDeviceImpl::open( const std::string& port, std::ios::openmode mode)
{
    throw AccessFailed(port);
}


void SerialDeviceImpl::close()
{
}

void SerialDeviceImpl::sync()
{

}

void SerialDeviceImpl::cancel(EventLoop& loop)
{
}


bool SerialDeviceImpl::runRead(EventLoop& loop)
{
    return false;
}


bool SerialDeviceImpl::runWrite(EventLoop& loop)
{
    return false;
}


std::size_t SerialDeviceImpl::beginRead(EventLoop& loop, char* buffer, std::size_t n, bool& eof) 
{
    eof = true; 
    return 0;
}


std::size_t SerialDeviceImpl::endRead(EventLoop& loop, char* buffer, std::size_t n, bool& eof)
{
    eof = true;
    return 0;
}


std::size_t SerialDeviceImpl::read( char* buffer, std::size_t count, bool& eof )
{
    eof = true;
    return 0;
}


std::size_t SerialDeviceImpl::beginWrite(EventLoop& loop, const char* buffer, std::size_t n)
{
    return n;
}


std::size_t SerialDeviceImpl::endWrite(EventLoop& loop, const char* buffer, std::size_t n)
{
    return n;
}   


std::size_t SerialDeviceImpl::write(const char* buffer, std::size_t n)
{
    return n;
}


void SerialDeviceImpl::setTimeout( std::size_t msec )
{
}


std::size_t SerialDeviceImpl::timeout() const
{
    return 0;
}


void SerialDeviceImpl::setBaudRate( unsigned rate )
{

}


unsigned SerialDeviceImpl::baudRate() const
{
    return 0;
}


void SerialDeviceImpl::setCharSize( int size )
{
}


int SerialDeviceImpl::charSize() const
{
    return 0;
}


void SerialDeviceImpl::setStopBits( SerialDevice::StopBits bits )
{
}


SerialDevice::StopBits SerialDeviceImpl::stopBits() const
{
    return SerialDevice::OneStopBit;
}


void SerialDeviceImpl::setParity( SerialDevice::Parity parity )
{
}


SerialDevice::Parity SerialDeviceImpl::parity() const
{
    return SerialDevice::ParityEven;
}


bool SerialDeviceImpl::setSignal(SerialDevice::Signal signal)
{
    return false;
}


void SerialDeviceImpl::setFlowControl( SerialDevice::FlowControl flowControl )
{
}


SerialDevice::FlowControl SerialDeviceImpl::flowControl() const
{
    return SerialDevice::FlowControlBoth;
}

}//namespace System

}//namespace Pt
