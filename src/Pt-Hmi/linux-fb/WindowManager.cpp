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

#include "WindowManager.h"

namespace Pt {
namespace Hmi {

WindowManager::WindowManager()
{
	_systemEvent += Pt::slot( *this, & WindowManager::onInputEvent );

   // Open the frame buffer device
    _fd = open ("/dev/fb0", O_RDWR);

    if(_fd < 0)
        throw std::runtime_error("Could not open framebuffer device" + PT_SOURCEINFO);

/*
    if( 0 > ioctl(_fd, FBIOGET_VSCREENINFO, &_screenInfo) )
        throw std::runtime_error("FBIOGET_VSCREENINFO failed" + PT_SOURCEINFO);

    _screenInfo.bits_per_pixel = 16;
    _screenInfo.xres           = 640;
    _screenInfo.yres           = 480;

    if( 0 > ioctl(_fd, FBIOPUT_VSCREENINFO, &_screenInfo) )
        throw std::runtime_error("FBIOPUT_VSCREENINFO failed" + PT_SOURCEINFO);
*/

    if( 0 > ioctl(_fd, FBIOGET_VSCREENINFO, &_screenInfo) )
        throw std::runtime_error("FBIOGET_VSCREENINFO failed" + PT_SOURCEINFO);

    // Get the fixed state
    if( ioctl(_fd, FBIOGET_FSCREENINFO, &_fixedInfo) < 0 )
        throw std::runtime_error("FBIOGET_FSCREENINFO failed" + PT_SOURCEINFO);

    //_fixedInfo.type;   // 0 -> Packed pixels
                         // 1 -> Non interleaved planes
                         // 2 -> Interleaved planes
                         // 3 -> Text/attributes
                         // 4 -> EGA/VGA planes

    //_fixedInfo.visual; // 0 -> Mono (1=black, 0=white)
                         // 1 -> Mono (1=white, 0=black)
                         // 2 -> True color
                         // 3 -> Pseudo color (like atari)
                         // 4 -> Direct color
                         // 5 -> Pseudo color readonly

    // Memory map the display
    unsigned _pitch = _screenInfo.xres * _screenInfo.bits_per_pixel / 8;
    _bufferSize     = _pitch * _screenInfo.yres;
    _buffer         =  mmap(NULL, _bufferSize, PROT_READ | PROT_WRITE, MAP_SHARED, _fd, 0);		  
}


void WindowManager::copyImageData(ssize_t toX, ssize_t toY, const char* data, size_t fromWidth, size_t fromHeight)
{
	toX  = (width()  - fromWidth) ;

	size_t pixelSize = depth() / 8;
	unsigned bufferOffset = toX + ( toY * width() );
	char* bufferData = (char*)( _buffer) + ( bufferOffset * pixelSize);

	for(size_t n = 0; n < fromHeight; ++n)
	{
		memcpy(bufferData, data, fromWidth * pixelSize);
		bufferData += width() * pixelSize;
		data += fromWidth * pixelSize;
	}
}


WindowImpl* WindowManager::active()
{	
		return  _windows [ _windows.size() - 1 ];
}

void WindowManager::add( WindowImpl* w )
{
	_windows.push_back(w);
	w->onActivate();
	invalidate();
}

void WindowManager::remove( WindowImpl* window )
{
	std::vector<WindowImpl*>::iterator it = std::find(_windows.begin(), _windows.end(), w);

	if( it == _windows.end() )
		return;
		
	_windows.erase( it );
	window->onDeactivate();
			
	invalidate();
}


void WindowManager::activate(WindowImpl* w)
{
	std::vector<WindowImpl*>::iterator it = std::find(_windows.begin(), _windows.end(), w);

	if( it == _windows.end() )
		return;
		
	_windows.erase( it );	
	_windows.push_back(w);

	if( _windows.size() > 1 )
		_windows[_windows.size() - 2]->onDeactivate();

	_windows[_windows.size() - 1]->onDeactivate();
	
	invalidate();
}

void WindowManager::invalidate()
{

}

void WindowManager::updateActive(size_t x, size_t y)
{
	WindowImpl* active = 0;

	for( int i = _windows.size() - 1;  i > -1; --i)
	{
		if( contains(_windows[i], Pt::Gfx::Point(x, y) )			
		{
			active = _windows[i];
			break;
		}	
	}
	 
}

void WindowManager::onInputEvent(const struct input_event& ev)
{
	if( _windows.size() == 0 )
		return;

    switch (ev.type)
    {
			case EV_KEY:
			{
				if(ev.value == 1)
					_keyEvent.setState(KeyEvent::KeyDown);
				else if(ev.value == 0)
					_keyEvent.setState(KeyEvent::KeyUp);
				else
					return;
			
	      switch(ev.code)
				{
					case KEY_RIGHTALT:
					case KEY_LEFTALT:
						_keyEvent.setAlt(_keyEvent.state() == KeyEvent::KeyDown);
											break;	
		
					case KEY_LEFTCTRL:
					case KEY_RIGHTCTRL:
						_keyEvent.setCtrl(_keyEvent.state() == KeyEvent::KeyDown);
					break;

					case KEY_LEFTSHIFT:
					case KEY_RIGHTSHIFT:
						_keyEvent.setShift(_keyEvent.state() == KeyEvent::KeyDown);  
					break;  

					default:
					{
						struct kbentry ke;
						ke.kb_table = 0;
						ke.kb_index = ev.code;

						::ioctl(STDIN_FILENO, KDGKBENT, (unsigned long)&ke);
					
						unsigned unicode = 0;
						unsigned typ = KTYP(ke.kb_value);
						unsigned value = KVAL(ke.kb_value);
                    
						if(typ == KT_LETTER|| typ == KT_LATIN)
						{
								unicode = value;
						}
						else if(typ == KT_PAD && value < 10)
						{
								unicode = 0x30 + value;
						}
						else
								return;

							_keyEvent.setUnicode(unicode);
						}
					break;
				}
					
				active().windowEvent().send(_keyEvent);			
			}
			break;

      case EV_REL:
			{
				if(ev.code == REL_X)
					_mouseEvent.addX( static_cast<double>(ev.value) );
				else if(ev.code == REL_Y)
					_mouseEvent.addX( static_cast<double>(ev.value) );
                
				updateActive( _mouseEvent.x(),  _mouseEvent.y() );
				
				invalidate();

				active().windowEvent().send( _mouseEvent );			  
			}
			break;

			case EV_ABS:
			{
				switch(ev.code)
				{
					case ABS_MT_SLOT:                
					case ABS_MT_TRACKING_ID:
                
					case ABS_X:
					case ABS_MT_POSITION_X:
						_mouseEvent.setX( static_cast<double>(ev.value) );
					break;
                    
					case ABS_Y:
					case ABS_MT_POSITION_Y:
						_mouseEvent.setY( static_cast<double>(ev.value) );
					break;
                    
					case ABS_PRESSURE:
					case ABS_MT_PRESSURE:                  
					default:
					break;
				}
		    
				updateActive( _mouseEvent.x(),  _mouseEvent.y() );

				invalidate();

				active().windowEvent().send( _mouseEvent );		
			}
			break;  
		}		
}


WindowManager::~WindowManager()
{
   if(_buffer)
        munmap(_buffer, _bufferSize);

    if(_fd > 0)
        close(_fd);
}


}} // namespace

