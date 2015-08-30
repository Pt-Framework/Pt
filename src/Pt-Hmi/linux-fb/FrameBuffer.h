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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA*/
#ifndef Pt_Hmi_FrameBuffer_h
#define Pt_Hmi_FrameBuffer_h

#include "InputDevice.h"
#include <Pt/System/MainLoop.h>
#include <Pt/Ui/Point.h>
#include <Pt/Ui/Size.h>
#include <Pt/Ui/Rect.h>
#include <Pt/Ui/Image.h>
#include <Pt/Hmi/WindowManager.h>
#include <Pt/Hmi/Cursor.h>
#include <linux/fb.h>

namespace Pt {
namespace Hmi {

class FrameBuffer
{
  public:
    enum BlitOp
		{
			CopyOp,
			AndOp,
			XorOp
		};

    FrameBuffer();

    ~FrameBuffer();
		

    size_t width() const
    {
        return _screenInfo.xres;
    }

    size_t height() const
    {
      return _screenInfo.yres;
    }	

    size_t depth() const
    {
       return _screenInfo.bits_per_pixel;
    }

    void bitBlit( const Pt::uint8_t* , size_t width, size_t height, const Ui::Point& pos, BlitOp op );

    void bitBlit( const Ui::Image& image );
    
    void grabImage( Ui::Image& image, const Ui::Point& pos,  const Ui::Size& size);
    
    void sync();

    size_t strideInBytes() const
    {
        return  _fixedInfo.line_length - ( width() *  depth() /8  );
    }


protected:


    char* buffer()
		{ 
			return &((char*)_buffer)[_backBufferOffset]; 
		}

    size_t size() const
    {
      return _bufferSize;
    }


	  const fb_var_screeninfo& screenInfo() const
		{
			return _screenInfo;
		}

		const fb_fix_screeninfo& fixedInfo() const
		{
			return _fixedInfo;
		}    

	protected:
		int								_fd;
		fb_var_screeninfo	_screenInfo;
		fb_fix_screeninfo	_fixedInfo;        
		void*							_buffer;
    size_t            _backBufferOffset;  
    size_t            _bufferSize;
    size_t		        _depth;
};

} // namespace

} // namespace

#endif

