/***************************************************************************
 *   Copyright (C) 2007 Marc Boris Drner                                  *
 *   Copyright (C) 2007 Laurentiu-Gheorghe Crisan                          *
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
#include "Pt/System/SerialDevice.h"
#include "SerialDeviceImpl.h"


namespace Pt {

namespace System {

SerialDevice::SerialDevice()
: _impl( 0 )
{
    _impl = new SerialDeviceImpl();
}


SerialDevice::SerialDevice( const std::string& file, std::ios_base::openmode mode, bool isAsync)
: _impl( 0 )
{
    _impl = new SerialDeviceImpl();
    this->open( file, mode, isAsync );
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


void SerialDevice::open( const std::string& file, std::ios_base::openmode mode, bool isAsync )
{
    if( this->valid() ) {
        this->close();
    }

    _impl->open( file, mode, isAsync );

    IODevice::setValid(true);
    IODevice::setEof(false);
    IODevice::setAsync(isAsync);
}


IOResult& SerialDevice::onBeginRead(char* buffer, size_t n, bool& eof)
{ 
    return _impl->beginRead(buffer, n, eof); 
}


size_t SerialDevice::onEndRead(IOResult& result, bool& eof)
{
    return _impl->endRead(result, eof); 
}


IOResult& SerialDevice::onBeginWrite(const char* buffer, size_t n)
{
    return _impl->beginWrite(buffer, n);
}


size_t SerialDevice::onEndWrite(IOResult& result)
{
    return _impl->endWrite(result);
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


}//namespace System
}//namespace Pt
