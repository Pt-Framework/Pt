/*
 * Copyright (C) 2006 Marc Boris Duerner
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

#include "ApplicationImpl.h"
#include <Pt/Hmi/Application.h>
#include "ScreenImpl.h"
#include <fcntl.h>
#include <sys/ioctl.h> 
#include <sys/mman.h>

namespace Pt {
namespace Hmi {

/*_fixedInfo.type;   // 0 -> Packed pixels
                         // 1 -> Non interleaved planes
                         // 2 -> Interleaved planes
                         // 3 -> Text/attributes
                         // 4 -> EGA/VGA planes

    /_fixedInfo.visual; // 0 -> Mono (1=black, 0=white)
                         // 1 -> Mono (1=white, 0=black)
                         // 2 -> True color
                         // 3 -> Pseudo color (like atari)
                         // 4 -> Direct color
                         // 5 -> Pseudo color readonly
*/

ApplicationImpl::ApplicationImpl()
: _inputDevice("/dev/input/event0")
, _inputDevice2("/dev/input/event1")
{  		 
	_inputDevice.setActive(*this);
	_inputDevice.begin();

	_inputDevice2.setActive(*this);
	_inputDevice2.begin();  

	_fd = open ("/dev/fb0", O_RDWR);

	if(_fd < 0)
		throw std::runtime_error("Could not open framebuffer device" + PT_SOURCEINFO);

	if( 0 > ioctl(_fd, FBIOGET_VSCREENINFO, &_screenInfo) )
		throw std::runtime_error("FBIOGET_VSCREENINFO failed" + PT_SOURCEINFO);

	// Get the fixed state
	if( ioctl(_fd, FBIOGET_FSCREENINFO, &_fixedInfo) < 0 )
		throw std::runtime_error("FBIOGET_FSCREENINFO failed" + PT_SOURCEINFO);

    
	// Memory map the display
	std::clog<<"LineWidth = " << _fixedInfo.line_length << " xres = " << _screenInfo.xres << std::endl;
	std::clog<<"yres = " << _screenInfo.yres << std::endl;

	const unsigned widthInBytes = _screenInfo.xres * _screenInfo.bits_per_pixel / 8;
	size_t bufferSize     = _fixedInfo.line_length * _screenInfo.yres;
	_buffer         =  mmap(NULL, bufferSize, PROT_READ | PROT_WRITE, MAP_SHARED, _fd, 0);	
}


ApplicationImpl::~ApplicationImpl()
{
	if(_buffer)
		munmap(_buffer, (_fixedInfo.line_length * _screenInfo.yres));

	if(_fd > 0)
		::close(_fd);
} 

void ApplicationImpl::nextEvent()
{
	MainLoop::waitNext();
}

void ApplicationImpl::setCursor( const Hmi::Cursor* cursor )
{	
	if( cursor == 0 )
		Application::instance().mainScreen().impl()->Cursor = Hmi::Cursor::defaultCursor();
	else
		Application::instance().mainScreen().impl()->Cursor = *cursor;
}

}} // namespace

