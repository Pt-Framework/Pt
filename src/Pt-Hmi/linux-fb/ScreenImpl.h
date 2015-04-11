/* Copyright (C) 2015 Marc Boris Duerner 
 * Copyright (C) 2015 Laurentiu-Gheorghe Crisan
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA*/
#ifndef Pt_Hmi_ScreenImpl_H
#define Pt_Hmi_ScreenImpl_H

#include <Pt/Hmi/WindowManager.h>
#include <Pt/Hmi/Window.h>
#include <Pt/Hmi/Cursor.h>
#include <linux/fb.h>
#include <Pt/Gfx/Rgb888Color.h>
#include <Pt/Gfx/Rgb565Color.h>

namespace Pt{
namespace Hmi{

class ScreenImpl : public Window
{
	public:
		ScreenImpl();
		virtual ~ScreenImpl();

		double width() const
		{
			return  Size.get().width();
		}

		double height() const
		{
			return  Size.get().height();
		}

		Pt::ssize_t depth() const
		{ 
			return _screenInfo.bits_per_pixel;
		}

		char* frameBuffer()
		{ 
			return (char*)_buffer; 
		}

		WindowManager& windowManager()
		{
			return _windowManager;
		}

		
	protected:
		virtual void onInvalidate();
		void onPointerInput( const Pt::Hmi::PointerEvent& mouseEvent );

	private:
   
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

		enum BlitOp
		{
			CopyOp,
			AndOp,
			XorOp
		};

		void blitPlane(const std::vector<Pt::uint8_t>& plane, size_t width, size_t height, const Gfx::Point& pos, BlitOp op);
		void saveCursorImage(const Pt::Hmi::PointerEvent& mouseEvent);

	private:
		WindowManager						_windowManager;
		int											_fd;
		fb_var_screeninfo				_screenInfo;
		fb_fix_screeninfo				_fixedInfo;
		void*										_buffer;
		Pt::size_t							_bufferSize;
		PaintSurface            _paintSurface;	
		
		std::vector<Pt::uint8_t>	_cursorBuffer;
		Pt::Gfx::Point            _cursorPos;
		size_t									  _cursorWidth;	
		size_t									  _cursorHeight;	
};

}}

#endif