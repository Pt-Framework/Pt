/***************************************************************************
 *   Copyright (C) 2013 Marc Boris Duerner                                 *
 *   Copyright (C) 2013 Laurentiu-Ghoerghe Crisan                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef Pt_Hmi_x11_ApplicationImpl_h
#define Pt_Hmi_x11_ApplicationImpl_h

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
#include <Pt/System/Selectable.h>
#include <Pt/System/MainLoop.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Hmi/Controller.h>
#include <Pt/Singleton.h>
#include <Pt/Event.h>
#include "posix/Selector.h"
#include <map>
#include <unistd.h>
#include "X11Fd.h"

namespace Pt{
namespace Hmi{

class ApplicationImpl : public Pt::System::MainLoop
{
public:
    ApplicationImpl();
    virtual ~ApplicationImpl();

    inline Display* display()
    { return _display; }

		Pt::Signal<XEvent&> WindowEvent;  

 		double toUnit(int value);
		Pt::Gfx::PointF toUnit(const Pt::Gfx::Point& value);
		Pt::Gfx::SizeF toUnit(const Pt::Gfx::Size& value);

		int fromUnit(double value);
		Pt::Gfx::Point fromUnit(const Pt::Gfx::PointF& value);
		Pt::Gfx::Size fromUnit(const Pt::Gfx::SizeF& value);

		double unitSizeInch() const;
		double unitSizeMm() const;

		void setResolution(double dpi);

		double resolutionDPI() const;
		
		void showConsole(bool show);
	
		inline Pt::Signal<Controller*, const PointingEvent&>& pointerEvent()
		{
			return _pointerEvent;
		}

		inline Pt::Signal<Controller*, const KeyEvent&>& keyDeviceEvent()
		{
			return _keyDeviceEvent;
		}

		void nextEvent();

private:
    Display* _display;
    X11Fd	_xfd;

	Pt::Signal<Controller*, const PointingEvent&> _pointerEvent;			
	Pt::Signal<Controller*, const KeyEvent&> _keyDeviceEvent;
	double _dpi;
};

}}

#endif


