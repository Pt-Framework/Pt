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
#ifndef Pt_Hmi_GfxOutputDeviceImpl_h
#define Pt_Hmi_GfxOutputDeviceImpl_h

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
#include <vector>

namespace Pt{
namespace Hmi{

class GfxOutputDeviceImpl :public Pt::Connectable
{
public:
    GfxOutputDeviceImpl();
    virtual ~GfxOutputDeviceImpl();

	void output(Pt::Hmi::Model* model);

	Pt::Gfx::Painter* nativePainter()
	{
		return 0;
	}

	Window window()
	{
		return _window;
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
	void drawIndependentImage(const Pt::Gfx::ARgbImage& image);
	void bringWindowToTop();
	void create();
	void destroy();
	void show();
	void hide();
	void writeWindowSizeAndPos();
	void writeWindowProperties();
	void redraw();
	void readClientSizeAndPos(Pt::Gfx::SizeF& size, Pt::Gfx::PointF& pos);
	void pixelToScreen(char* data, const Pt::Gfx::ARgbColor& pixel);
	void updateDrawBuffer();
	void maximizeWindow();
	void minimizeWindow();
	void restoreWindow();
    bool isWindowMinimized();
    bool isWindowMaximazed();
    
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
    Window  				_window;
    GC 						_brushGc;
	Display* 				_display;
	unsigned int			_windowBorderWidth;
	bool 					_visible;
	std::vector<char> 		_pixelBuffer;
};

}}

#endif

