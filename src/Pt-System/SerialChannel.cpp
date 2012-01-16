/*
 * Copyright (C) 2005-2010 by Dr. Marc Boris Duerner
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
#include <Pt/System/IOError.h>
#include <iostream>
#include <sstream>


namespace Pt {

namespace System {

SerialChannel::SerialChannel()
: LogChannel()
{
}


SerialChannel::~SerialChannel()
{
    try
    {
        this->close();
    }
    catch(...) {}
}


void SerialChannel::onOpen(const std::string& urlstr)
{
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


void SerialChannel::onClose()
{
    _device.close();
}


void SerialChannel::onWrite(const std::string& message)
{
    //if(_device.enabled() == false)
    //    return;

    _device.write( message.data(), message.size() );
    _device.write("\r\n", 2);
    _device.sync();
}

} // namespace System

} // namespace Pt
