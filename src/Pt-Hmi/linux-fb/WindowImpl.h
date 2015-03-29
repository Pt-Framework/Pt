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
#ifndef Pt_Hmi_WindowImpl_h
#define Pt_Hmi_WindowImpl_h

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
#include <Pt/Connectable.h>
#include <Pt/Signal.h>
#include <Pt/Hmi/Api.h>
#include <Pt/Hmi/KeyEvent.h>
#include <Pt/Hmi/PointingEvent.h>
#include <Pt/Hmi/PositionEvent.h>
#include <Pt/Hmi/ResizeEvent.h>
#include <Pt/Hmi/CloseEvent.h>
#include <Pt/Hmi/ActivateEvent.h>
#include <Pt/Hmi/PaintSurface.h>
#include <Pt/Hmi/Window.h>
#include <linux/input.h>


namespace Pt {
namespace Hmi {

class WindowImpl  :public Pt::Connectable
{
	public:
		WindowImpl(PaintSurface* surface);
    
		~WindowImpl();

		void create();
	
		void destroy();

		void show();

		void hide();

		void render();

		void setPosition(const Gfx::PointF& p);

		void setSize(const Gfx::SizeF& size);

		void showTitle(bool p);

		void setCaption(const std::string& text);

		void showMinimizedButton(bool p);
  
		void showMaximizeButton(bool p);
  
		void showSysMenu(bool p);

		void setForceTopMost(bool force);
  
		void setWindowState(WindowState::Type p);
  
		void setBorder(WindowBorder::Type p);
  
		void showInTaskbar(bool p);
  
		void setIcon(const Pt::Gfx::ARgbImage& p);

		void setEnable(bool e);	


		Pt::Signal<const Pt::Event&>& windowEvent()
		{
			return _windowEvent;
		}
			
		const Pt::Gfx::SizeF& windowSize() const
		{
			return _size;
		}
		
		void onActivate();

		void onDeactivate();
	

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
			
	private:    
		PaintSurface*           _surface;
		PaintSurface						_windowSurface;
		Pt::Gfx::SizeF					_windowSize;

};

}} // namespace

#endif
