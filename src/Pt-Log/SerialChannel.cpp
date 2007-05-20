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
#include <iostream>


namespace Pt {

namespace Log {

SerialChannel::SerialChannel()
: Channel()
{
}


SerialChannel::~SerialChannel()
{
}


void SerialChannel::_open(const std::string& urlstr)
{
    System::Url url(urlstr);
    _device.open( url.path(), std::ios::out );
}


void SerialChannel::_close()
{
    _device.close();
}


void SerialChannel::_write(const std::string& message)
{
    _device.write( message.data(), message.size() );
}

} // namespace Log

} // namespace Pt
