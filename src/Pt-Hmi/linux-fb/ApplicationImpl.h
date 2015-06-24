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
#ifndef Pt_Hmi_ApplicationImpl_h
#define Pt_Hmi_ApplicationImpl_h

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

class ApplicationImpl : public Pt::System::MainLoop
{
    public:
    ApplicationImpl();

    ~ApplicationImpl();


		Ui::PointF toUnit(const Ui::Point& value)
		{
			return Ui::PointF(value.x(), value.y());
		}

		Ui::SizeF toUnit(const Ui::Size& value)
		{
			return Ui::SizeF(value.width(), value.height());
		}

		double toUnit(int value)
		{
			return value;
		}

		Ui::Point fromUnit(const Ui::PointF& value)
		{
			return Ui::Point(value.x(), value.y());
		}

		Ui::Size fromUnit(const Ui::SizeF& value)
		{
			return Ui::Size(value.width(), value.height());
		}

		Ui::Rect fromUnit(const Ui::RectF& value)
		{
			return Ui::Rect(Ui::Point( value.x(), value.y()) , Ui::Size(value.width(), value.height()));
		}

		int fromUnit(double value)
		{
			return (int) value;
		}

		double unitSizeInch() const
		{
			return 0;
		}
			
		double unitSizeMm() const
		{
			return 0;
		}

		void setResolution(double dpi)
		{
		}

		double resolutionDPI() const
		{
			return 0;
		}

		Pt::Signal<const struct input_event&>& inputEvent()
		{
			return _inputEvent;
		}

		void nextEvent();

		void setCursor( const Hmi::Cursor* cursor );

		/*
		Pt::ssize_t depth() const
		{ 
			return _screenInfo.bits_per_pixel;
		}

		Pt::ssize_t stride() const
		{ 
			return _fixedInfo.line_length -  _screenInfo.xres;
		}

		*/

		char* frameBuffer()
		{ 
			return (char*)_buffer; 
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
		Pt::Signal<const struct input_event&> _inputEvent;
		InputDevice _inputDevice;
		InputDevice _inputDevice2;		
		int											_fd;
		fb_var_screeninfo				_screenInfo;
		fb_fix_screeninfo				_fixedInfo;
		void*										_buffer;


};

} // namespace

} // namespace

#endif

