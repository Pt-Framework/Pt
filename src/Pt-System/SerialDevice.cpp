/*
 * Copyright (C) 2007 Marc Boris Drner
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
#include "Pt/System/SerialDevice.h"
#include "SerialDeviceImpl.h"

namespace Pt {

namespace System {

SerialDevice::SerialDevice()
: _impl( 0 )
{
    _impl = new SerialDeviceImpl(*this);
}


SerialDevice::SerialDevice( const std::string& file, OpenMode mode)
: _impl( 0 )
{
    _impl = new SerialDeviceImpl(*this);
    this->open( file, mode);
}


SerialDevice::~SerialDevice()
{
    try
    {
        close();
    }
    catch( ... )
    { }

    delete _impl;
}


void SerialDevice::open( const std::string& file, OpenMode mode)
{
    if( this->enabled() ) {
        this->close();
    }

    _impl->open( file, mode);

    IODevice::setEnabled(true);
    IODevice::setEof(false);
    IODevice::setAsync( mode & IODevice::Async);
}


size_t SerialDevice::onBeginRead(char* buffer, size_t n, bool& eof)
{
    return _impl->beginRead(buffer, n, eof);
}


size_t SerialDevice::onEndRead(bool& eof)
{
    return _impl->endRead(eof);
}


size_t SerialDevice::onBeginWrite(const char* buffer, size_t n)
{
    return _impl->beginWrite(buffer, n);
}


size_t SerialDevice::onEndWrite()
{
    return _impl->endWrite();
}


void SerialDevice::setBaudRate( BaudRate rate )
{
    _impl->setBaudRate( rate );
}


SerialDevice::BaudRate SerialDevice::baudRate() const
{
    return _impl->baudRate();
}


void SerialDevice::setCharSize( int size )
{
    _impl->setCharSize( size );
}


int SerialDevice::charSize() const
{
    return _impl->charSize();
}


void SerialDevice::setStopBits( StopBits bits )
{
    _impl->setStopBits( bits );
}


SerialDevice::StopBits SerialDevice::stopBits() const
{
    return _impl->stopBits();
}


void SerialDevice::setParity( Parity parity )
{
    _impl->setParity( parity );
}


SerialDevice::Parity SerialDevice::parity() const
{
    return _impl->parity();
}


void SerialDevice::setFlowControl( FlowControl flowControl )
{
    _impl->setFlowControl(  flowControl );
}


SerialDevice::FlowControl SerialDevice::flowControl() const
{
    return _impl->flowControl();
}


void SerialDevice::setTimeout( size_t msec )
{
    if ( !this->async() )
        _impl->setTimeout( msec );
}


size_t SerialDevice::timeout() const
{
    return _impl->timeout();
}


void SerialDevice::onClose()
{
    _impl->close();
}


bool SerialDevice::onWait(std::size_t n)
{
    return _impl->wait(n);
}


size_t SerialDevice::onRead(char* buffer, size_t count, bool& eof)
{
    return _impl->read( buffer, count, eof );
}


size_t SerialDevice::onWrite(const char* buffer, size_t count)
{
    return _impl->write( buffer, count );
}


void SerialDevice::flush()
{
    _impl->flush();
}

IODeviceImpl& SerialDevice::ioimpl()
{ 
    return *_impl; 
}


SelectableImpl& SerialDevice::simpl()
{ 
    return *_impl; 
}


void SerialDevice::onAttach(SelectorBase& s)
{
    _impl->attach(s);
}


void SerialDevice::onDetach(SelectorBase& s)
{
    _impl->detach(s);
}

}//namespace System

}//namespace Pt
