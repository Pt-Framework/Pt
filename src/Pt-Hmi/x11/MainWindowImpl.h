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

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/cursorfont.h>

// X11 defines these two globally, which conflicts with enum values in ptv/text/Char.h
#undef Above
#undef Below

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
#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/Window.h>

namespace Pt{
namespace Hmi{

class WindowImpl :public Pt::Connectable
{
	public:
    WindowImpl(PaintSurface* surface);
	  virtual ~WindowImpl();

		void create();
	
		void destroy();

		void show();

		void hide();

		void render();

		Pt::Signal<const Pt::Event&>& windowEvent()
		{
			return _windowEvent;
		}

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
   
	private:
		void onWindowEvent(XEvent& ev);	
		void onClientMessage(XEvent& xev);
		void onMotionNotify(XEvent& xev); 
		void onMouseButtonPress(XEvent& xev);
		void onMouseButtonRelease(XEvent& xev);
		void onKeyEvent(XEvent& xev);
		void onConfigureNotify( XEvent& xev);

	private:
		void bringWindowToTop();
		bool isWindowMinimized();
		bool isWindowMaximazed();
		void restoreWindow();
		void minimizeWindow();
		void maximizeWindow();

	private:
		Atom AtomAppWake;
		Atom AtomWindowResize;
		Atom AtomWindowMove;
		Atom AtomWindowClosed;
		Atom AtomWMProtocols;
		Pt::Hmi::Application&         _app; 
		Pt::Hmi::PaintSurface*				_surface;
		Pt::Signal<const Pt::Event&>	_windowEvent;
		KeyEvent											_keyEvent;
		PointingEvent									_pointerEvent;
		ResizeEvent										_resizeEvent;
		PositionEvent									_positionEvent;
		ActivateEvent									_activateEvent;
		bool													_forceTopMost;		
		::Window  										_window;
		::GC 													_brushGc;
		::Display* 										_display;
		std::vector<char> 						_pixelBuffer;
		bool													_forceTopMost;
		int _x;
		int _y;
		int _width;
		int _height;
		bool _showTitle;
		std::string _title;
};

}}

#endif

