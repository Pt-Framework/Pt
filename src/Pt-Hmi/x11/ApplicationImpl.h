 /* Copyright (C) 2015 Marc Boris Duerner 
  
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
  MA  02110-1301  USA
*/

#ifndef Pt_Hmi_x11_ApplicationImpl_h
#define Pt_Hmi_x11_ApplicationImpl_h

#include "X11Fd.h"

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/cursorfont.h>

// X11 defines these two globally, which conflicts with enum values in Pt/Char.h
#undef Above
#undef Below

#include <Pt/Hmi/Api.h>
#include <Pt/System/MainLoop.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/Event.h>

#include <map>
#include <unistd.h>

namespace Pt {

namespace Hmi {

class ApplicationImpl : public Pt::System::MainLoop
{
	public:
    	ApplicationImpl();
    
		virtual ~ApplicationImpl();

    	inline Display* display()
    	{ return _display; }

		void nextEvent();

		Pt::Signal<XEvent&> WindowEvent;  

 		double toUnit(int value);
		Pt::Gfx::PointF toUnit(const Pt::Gfx::Point& value);
		Pt::Gfx::SizeF toUnit(const Pt::Gfx::Size& value);

		int fromUnit(double value);
		Pt::Gfx::Point fromUnit(const Pt::Gfx::PointF& value);
		Pt::Gfx::Size fromUnit(const Pt::Gfx::SizeF& value);
		Pt::Gfx::Rect fromUnit(const Pt::Gfx::RectF& value);

		double unitSizeInch() const;
		double unitSizeMm() const;

		void setResolution(double dpi);

		double resolutionDPI() const;
		
	
		inline Pt::Signal<const Pt::Event&>& systemEvent()
		{
			return _systemEvent;
		}

	private:
    	Display* _display;
    	X11Fd	 _xfd;
		Pt::Signal<const Pt::Event&> _systemEvent;				
		double   _dpi;
};

} // namespace

} // namespace

#endif // include guard
