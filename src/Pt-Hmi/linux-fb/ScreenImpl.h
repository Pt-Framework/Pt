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
#ifndef Pt_Hmi_ScreenImpl_H
#define Pt_Hmi_ScreenImpl_H

#include <Pt/Hmi/WindowManager.h>
#include <Pt/Hmi/Window.h>
#include <Pt/Hmi/PaintSurface.h>
#include <Pt/Gfx/Color.h>
#include <Pt/Gfx/Image.h>
#include "FrameBuffer.h"

namespace Pt{
namespace Hmi{

class ApplicationImpl;
class FrameBuffer;
class Cursor;

class ScreenImpl : public Window
{
	public:
		ScreenImpl(ApplicationImpl& app);

		virtual ~ScreenImpl();

		double width() const
		{
			return  Size.get().width();
		}

		double height() const
		{
			return  Size.get().height();
		}
		 		 
		virtual void activate()
		{
		}

		const Gfx::Image& image() const
		{
			return _image;
		} 

		Gfx::Image& image() 
		{
			return _image;
		} 

		double toUnit(int value)
    {
      return value;
    }

		Gfx::PointF toUnit(const Gfx::Point& value)
    {
      return Gfx::PointF( value.x(), value.y() );
    }

		Gfx::SizeF toUnit(const Gfx::Size& value)
    {
      return Gfx::SizeF( value.width(), value.height() );
    }

		int fromUnit(double value)
    {
      return (int) value;
    }

		Gfx::Point fromUnit(const Gfx::PointF& value)
    {
      return Gfx::Point( (int)value.x(), (int)value.y() );
    }

		Gfx::Size fromUnit(const Gfx::SizeF& value)
    {
       return Gfx::Size( (int)value.width(), (int)value.height() );
    }

		Gfx::Rect fromUnit(const Gfx::RectF& value)
    {
      return Gfx::Rect(Gfx::Point( (int) value.x(), (int)value.y()) ,Gfx::Size( (int)value.width(), (int)value.height() ) );
    }

    double unitSizeInch() const
    {
	    return 1.0/96;
    }

    double unitSizeMm() const
    {
	    return 25.4 * unitSizeInch();
    }


		void setResolution(double dpi)
    {
      _dpi = dpi;
    }

		double resolutionDPI() const
    {
      return _dpi;
    }
		
		void setCursor(const Hmi::Cursor* cursor );

	protected:
		virtual void onInvalidate();
		virtual void onPointerEvent( const Pt::Hmi::PointerEvent& mouseEvent );

	private:
    enum BlitOp
		{
			CopyOp,
			AndOp,
			XorOp
		};

		void grabImage( const Pt::uint8_t* buffer, const Gfx::Point& pos,Gfx::Image& image);
 
    void bitBlit( const Pt::uint8_t* , size_t width, size_t height, const Gfx::Point& pos, Pt::uint8_t* buffer, BlitOp op );

    void bitBlit( const Gfx::Image& image, Pt::uint8_t* buffer );
	void drawCursor( Pt::uint8_t* buffer );
	void updateScreen();
	
	private:
    FrameBuffer& _frameBuffer;  
		Gfx::Image	  _cursorBackground;
		Gfx::Point		_cursorPos;    
		Gfx::Image   _image;
    double      _dpi;
	bool _drawCursor;
};

}

}

#endif
