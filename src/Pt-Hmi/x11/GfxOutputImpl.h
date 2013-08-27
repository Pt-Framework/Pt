/***************************************************************************
 *   Copyright (C) 2013 Marc Boris Duerner                                 *
 *   Copyright (C) 2013 Laurentiu-Gheorghe Crisan                          *
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
#ifndef Pt_Hmi_GfxOutputImpl_h
#define Pt_Hmi_GfxOutputImpl_h

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/cursorfont.h>

// X11 defines these two globally, which conflicts with enum values in ptv/text/Char.h
#undef Above
#undef Below

#include <Pt/Api.h>
#include <Pt/Connectable.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/Hmi/WindowModel.h>
#include <Pt/Hmi/PointingEvent.h>
#include <Pt/Hmi/KeyEvent.h>

namespace Pt{
namespace Hmi{

class GfxOutputImpl :public Pt::Connectable
{
public:
    GfxOutputImpl();
    virtual ~GfxOutputImpl();

	void output(Pt::Hmi::Model* model);

	Pt::Gfx::Painter* nativePainter()
	{
		return 0;
	}

private:
	void onWindowEvent(XEvent& ev);
	void onClientMessage(XEvent& ev);
	void onMotionNotify(XEvent& ev);
	void onMouseButtonPress(XEvent& ev);
	void onMouseButtonRelease(XEvent& xev);
	void onPaint(XEvent& xev);
	void onConfigureNotify(XEvent& xev);
	void onKeyEvent(XEvent& ev);

private:
	void create();
	void destroy();
	void show();
	void hide();
	void output();
	void paint();
	void drawIndependentImage(size_t x, size_t y, const char* data, size_t width, size_t height);       
	void writeWindowSizeAndPos();
	void writeWindowProperties();

private:
    Atom AtomAppWake;
    Atom AtomWindowResize;
    Atom AtomWindowMove;
    Atom AtomWindowClosed;
    Atom AtomWMProtocols;

	bool					_ignoreSizeEvent;
	Pt::Hmi::WindowModel* 	_model;
	Pt::Hmi::PointingEvent 	_mouseEvent;
	Pt::Hmi::KeyEvent      	_keyEvent;
	Pt::Gfx::Rgb888Image	_rgb88Image;
    Window  				_window;
    GC 						_brushGc;
	Display* 				_display;
};

}}

#endif

