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

void SerialDevice::_close()
{
    _impl->close();
}

size_t SerialDevice::_read( char* buffer, size_t count, bool& eof )
{
    return _impl->read( buffer, count, eof );
}

size_t SerialDevice::_write(const char* buffer, size_t count)
{
    return _impl->write( buffer, count );
}

}//namespace System
}//namespace Pt
