/***************************************************************************
 *   Copyright (C) 2005-2007 by Dr. Marc Boris Drner                       *
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

#include "SerialChannel.h"
#include <Pt/System/Url.h>
#include <Pt/System/IOError.h>
#include <iostream>
#include <sstream>


namespace Pt {

namespace Log {

SerialChannel::SerialChannel()
: Pt::System::Thread( _threadLoop )
, Channel()
{
    _n = 0;
    Pt::connect( _threadLoop.event, *this, &SerialChannel::processEvent);
    System::Thread::start();
}


SerialChannel::~SerialChannel()
{
    _threadLoop.exit();
    System::Thread::wait();
    this->close();
}


void SerialChannel::processEvent(const Pt::Event& ev)
{
    const WriteEvent* wev = dynamic_cast<const WriteEvent*>(&ev);
    if(wev)
        this->_write( wev->message(), false );

     --_n;
}


void SerialChannel::_open(const std::string& urlstr)
{
    Pt::System::MutexLock lock( _mutex );

    //TODO Use System::Url for "comm" scheme as soon as protocol handler for "comm" is implemented
    //System::Url url(urlstr);
    std::stringstream sStream(urlstr);
    std::string protocol;
    std::getline(sStream, protocol, ':');
    if( protocol != "comm" )
    {
        throw Pt::System::OpenFailed( "Unexpected protocol type: " + protocol, PT_SOURCEINFO );
    }
    if( sStream.get() != '/' ) 
    {
        throw Pt::System::OpenFailed( "Malformed URL! Expected '/' after protocol part.", PT_SOURCEINFO );
    }
    if( sStream.get() != '/' ) 
    {
        throw Pt::System::OpenFailed( "Malformed URL! Expected '/' after protocol part.", PT_SOURCEINFO );
    }
    std::string path;
    std::getline( sStream, path );
    _device.open( path, std::ios::out );
    _device.setBaudRate(Pt::System::SerialDevice::BaudRate4800);
	_device.setCharSize(8);
	_device.setStopBits(Pt::System::SerialDevice::OneStopBit);
	_device.setParity(Pt::System::SerialDevice::ParityNone);
}


void SerialChannel::_close()
{
    Pt::System::MutexLock lock( _mutex );

    _device.close();
}


void SerialChannel::_write(const std::string& message, bool isAsync)
{
    if( isAsync )
    {
        if(_n > 10)
        {
            return;
        }

        ++_n;

        WriteEvent wev(message);
        _threadLoop.commitEvent(wev);

        return;
    }

    Pt::System::MutexLock lock( _mutex );

    if(_device.valid() == false)
        return;

    _device.write( message.data(), message.size() );
    _device.write("\r\n", 2);
    _device.sync();
}

} // namespace Log

} // namespace Pt
