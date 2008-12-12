/*
 * Copyright (C) 2005-2007 by Dr. Marc Boris Drner
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

#include "SerialChannel.h"
#include <Pt/System/Url.h>
#include <Pt/System/IOError.h>
#include <iostream>
#include <sstream>


namespace Pt {

namespace Log {

SerialChannel::SerialChannel()
: Channel()
, _threadLoop()
, _thread(_threadLoop)
, _n(0)
{
    Pt::connect( _threadLoop.event, *this, &SerialChannel::processEvent);
    _thread.start();
}


SerialChannel::~SerialChannel()
{
    _threadLoop.exit();
    _thread.join();
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
        throw Pt::System::AccessFailed( "Unexpected protocol type: " + protocol, PT_SOURCEINFO );
    }
    if( sStream.get() != '/' )
    {
        throw Pt::System::AccessFailed( "Malformed URL! Expected '/' after protocol part.", PT_SOURCEINFO );
    }
    if( sStream.get() != '/' )
    {
        throw Pt::System::AccessFailed( "Malformed URL! Expected '/' after protocol part.", PT_SOURCEINFO );
    }
    std::string path;
    std::getline( sStream, path );
    _device.open( path, Pt::System::SerialDevice::Write );
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

    if(_device.enabled() == false)
        return;

    _device.write( message.data(), message.size() );
    _device.write("\r\n", 2);
    _device.sync();
}

} // namespace Log

} // namespace Pt
