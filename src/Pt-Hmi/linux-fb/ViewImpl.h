/*
Copyright (C) 2013 Marc Boris Duerner                                 
Copyright (C) 2013 Laurentiu-Gheorghe Crisan                          
                                                                          
This program is free software; you can redistribute it and/or modify  
it under the terms of the GNU Library General Public License as       
published by the Free Software Foundation; either version 2 of the    
License, or (at your option) any later version.                       
                                                                          
This program is distributed in the hope that it will be useful,       
but WITHOUT ANY WARRANTY; without even the implied warranty of        
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         
GNU General Public License for more details.                          
                                                                          
You should have received a copy of the GNU Library General Public     
License along with this program; if not, write to the                 
Free Software Foundation, Inc.,                                       
59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.*/
#ifndef Pt_Hmi_ViewImpl_h
#define Pt_Hmi_ViewImpl_h

#include <Pt/Hmi/Api.h>
#include <Pt/Gfx/Rgb888Image.h>
#include <Pt/Gfx/Rgb565Image.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <Pt/Hmi/PointingEvent.h>
#include <Pt/Hmi/KeyEvent.h>
#include <Pt/Connectable.h>
#include <linux/input.h>


namespace Pt {

namespace Hmi {

class WindowController;
class WindowModel;
class Controller;
class Model;

class ViewImpl  :public Pt::Connectable
{
	public:
		ViewImpl();
    
		~ViewImpl();

		void output(Pt::Hmi::Controller* controller, Pt::Hmi::Model* model);


	protected:
        template <typename Iterator>
        void drawImage(ssize_t toX, ssize_t toY, Iterator begin, Iterator end, size_t width, size_t height)
        {
            const char* imageData = 0;

            switch( depth() )
            {
                case 32:
                {
                    Gfx::Rgb888Image rgbImage( width, height );
                    assign( begin, end, rgbImage.begin() );
                    this->copyImageData( toX, toY, (char*)rgbImage.data(), rgbImage.width(), rgbImage.height() );                    
                }
				break;

                case 16:
                {
                    Gfx::Rgb565Image rgbImage( width, height );
                    assign( begin, end, rgbImage.begin() );
                    imageData = (char*)( rgbImage.data() );
                    this->copyImageData( toX, toY, (char*)rgbImage.data(), rgbImage.width(), rgbImage.height() );                    
                }
				break;

                default:
                    imageData = 0;
            }
        }

		void copyImageData(ssize_t toX, ssize_t toY, const char* data, size_t fromWidth, size_t fromHeight);

		Pt::ssize_t depth() const
		{ return _screenInfo.bits_per_pixel;}

		Pt::ssize_t width() const
		{ return _screenInfo.xres;}

		Pt::ssize_t height() const
		{ return _screenInfo.yres;}

		char* frameBuffer()
		{ return (char*)_buffer; }
			
	protected:
		void onInputEvent(const struct input_event& ev);

	private:    
		int _fd;
		fb_var_screeninfo _screenInfo;
		fb_fix_screeninfo _fixedInfo;
		void* _buffer;
		Pt::size_t _bufferSize;
		WindowController* _controller;
		WindowModel* _model;
		Pt::Hmi::PointingEvent 	_mouseEvent;
		Pt::Hmi::KeyEvent      	_keyEvent;
};

} // namespace

} // namespace

#endif
