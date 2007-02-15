/***************************************************************************
 *   Copyright (C) 2007 Marc Boris Dürner                                  *
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

namespace Pt{
namespace System{

SerialDevice::SerialDevice( const std::string& file, std::ios_base::openmode mode )  throw(IO::IOError)
: _impl( 0 )
{
    _impl = new SerialDeviceImpl( file, mode );
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

void SerialDevice::setBaudRate( BaudRate rate )
{
    _impl->setBaudRate( rate );
}

BaudRate SerialDevice::baudRate() const
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
      
void SerialDevice::setStopBits( short bits )
{
    _impl->setStopBits( bits );
}

short SerialDevice::stopBits()
{
    return _impl->setStopBits();
}
        
void SerialDevice::setParity( Parity parity )
{
    _impl->setParity( parity );
}

Parity SerialDevice::parity() const
{
    _impl->parity();
}
        
void SerialDevice::setFlowControl( FlowControl flowControl )
{
    _impl->setFlowControl(  flowControl );
}

FlowControl SerialDevice::flowControl() const
{
    return _impl->flowControl();
}

void SerialDevice::setInputMode( InputMode mode )
{
    _impl->setInputMode( mode );
}

InputMode SerialDevice::inputMode() const
{
    return _impl->inputMode();
}
      
void SerialDevice::setControlMode( ControlMode mode )
{
    _impl->setControlMode( mode );
}

ControlMode SerialDevice::controlMode() const
{
    _impl->controlMode();
}
        
LocalMode SerialDevice::localMode() const
{
    return _impl->localMode();
}

void SerialDevice::setLocalMode( LocalMode mode )
{
    _impl->setLocalMode( mode );
}

OutputMode SerialDevice::outputMode() const
{
    _impl->outputMode();
}

void SerialDevice::setOutputMode( OutputMode mode )
{
    _impl->setOutputMode( mode );
}

int SerialDevice::vMin() const
{
    return _impl->vMin();
}
 
void SerialDevice::setVMin( const int chars )
{
    return _impl->setVmin( chars );
}

int SerialDevice::vTime() const
{
    _impl->vTime( );
}

void SerialDevice::setVTime( const int secs )
{
    _impl->setVTime( secs );
}
    
void SerialDevice::_close()
{
    _impl->close();
}

bool SerialDevice::_wait( WaitMode mode , unsigned int msec)
{ 
    return _impl->wait( mode, msec );
}

size_t SerialDevice::_read( char* buffer, size_t count )
{
    return _impl->read( buffer, count );
}

size_t SerialDevice::_write(const char* buffer, size_t count)
{
    return _impl->write( buffer, count );
}

}//namespace System
}//namespace Pt