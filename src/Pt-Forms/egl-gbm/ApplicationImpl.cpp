 /* Copyright (C) 2015 Marc Boris Duerner 
    Copyright (C) 2015 Laurentiu-Gheorghe Crisan
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  
  As a special exception, you may use this file as part of a free
  software library without restriction. Specifically, if other files
  instantiate templates or use macros or inline functions from this
  file, or you compile this file and link it with other files to
  produce an executable, this file does not by itself cause the
  resulting executable to be covered by the GNU General Public
  License. This exception does not however invalidate any other
  reasons why the executable file might be covered by the GNU Library
  General Public License.
  
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
	MA 02110-1301	USA
*/

#include "ApplicationImpl.h"
#include <Pt/System/FileInfo.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <fcntl.h>
#include <sys/ioctl.h> 
#include <sys/mman.h>
#include <sys/kd.h>

namespace Pt {

namespace Forms {

ApplicationImpl::ApplicationImpl()
{  		 
	showConsole( false );
	_inputDevices.reserve(10);

	for(size_t i = 0; i < 10; ++i)
	{
		System::Path deviceName("/dev/input/event");

		std::ostringstream oss;
		oss << i;
		deviceName += oss.str().c_str();
			
		if( Pt::System::FileInfo::exists(deviceName) )
		{
			InputDevice* device = new InputDevice( deviceName.toLocal().c_str() );
			device->setScreenLimit( _frameBuffer.size() );
			device->setActive(*this);
			device->begin();
			device->eventReady() += Pt::slot(_eventReady);	
			
			_inputDevices.push_back(device);
			std::clog << "using: " << deviceName.toLocal() << std::endl;
		}
	}
}


ApplicationImpl::~ApplicationImpl()
{
	std::vector<InputDevice*>::iterator it;
	for(it = _inputDevices.begin(); it != _inputDevices.end(); ++it)
	{
		delete *it;
	}

	showConsole(true);
} 


void ApplicationImpl::showConsole(bool s)
{
	std::string terminal;
	std::ifstream ifs("/sys/class/tty/tty0/active");
	ifs >> terminal;
	terminal = "/dev/" + terminal;

	int fd = open(terminal.c_str(), O_RDWR);
	
	if( ! s )
		ioctl( fd, KDSETMODE, KD_GRAPHICS );
	else
		ioctl( fd, KDSETMODE, KD_TEXT );

	close( fd );
}


void ApplicationImpl::nextEvent()
{
	MainLoop::waitNext();
}

}

}
