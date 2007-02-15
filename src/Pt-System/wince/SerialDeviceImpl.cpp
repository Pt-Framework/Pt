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
#include "SerialDeviceImpl.h"

namespace Pt{
namespace System{

SerialDeviceImpl::SerialDeviceImpl( const std::string& file, std::ios_base::openmode mode ) throw(IO::IOError)
{    
}

SerialDeviceImpl::~SerialDeviceImpl()
{ 
}
        
void SerialDeviceImpl::close()
{
}

bool SerialDeviceImpl::wait( IO::IODevice::WaitMode mode, unsigned int msec )
{
    return true;
}

size_t SerialDeviceImpl::read( char* buffer, size_t count, bool& eof )
{
    return 0;
}

size_t SerialDeviceImpl::write( const char* buffer, size_t count )
{ 
    return 0;
}

void SerialDeviceImpl::sync() const
{

}

}//namespace System
}//namespace Pt