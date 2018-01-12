/* Copyright (C) 2013 Marc Boris Duerner
   
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
   MA 02110-1301 USA
*/

#ifndef Pt_Hmi_MainWindowImpl_h
#define Pt_Hmi_MainWindowImpl_h

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/cursorfont.h>

// X11 defines these two globally, which conflicts with enum values in ptv/text/Char.h
#undef Above
#undef Below

#include <Pt/Hmi/Api.h>
#include <Pt/Hmi/Window.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>

#include <Pt/Connectable.h>
#include <Pt/Signal.h>

#include <Pt/Hmi/KeyEvent.h>
#include <Pt/Hmi/ResizeEvent.h>
#include <Pt/Hmi/CloseEvent.h>
#include <Pt/Hmi/ActivateEvent.h>
#include <Pt/Hmi/PaintSurface.h>
#include <Pt/Hmi/Application.h>

namespace Pt {

namespace Hmi {

class MainWindowImpl :public Pt::Connectable
{
	public:
    	MainWindowImpl(Window::Type type);
	  	
		virtual ~MainWindowImpl();

		void setType(Window::Type type);

		void show(bool visible);

		void activate();

		void enable(bool e);

        void move(const Gfx::PointF& pos);

        void resize(const Gfx::SizeF& size);

        void close();

        void paint(const Gfx::RectF& rect);
    
        void setState(Window::State s);
       
        void setIcon(const Gfx::Image& p);
    
        void setMinimumSize(const Gfx::SizeF& s);
    
        void setMaximumSize(const Gfx::SizeF& s);

        void setTitle( const std::string& t );

        void setTopMost(bool e);

        void grabPointer();

		Window& window()
		{ 
			return _window; 
		}

		Pt::Signal<const Pt::Event&>& windowEvent()
		{
			return _windowEvent;
		}

   
	private:
		void onWindowEvent(XEvent& ev);	
		void onClientMessage(XEvent& xev);
		void onMotionNotify(XEvent& xev); 
		void onMouseButtonPress(XEvent& xev);
		void onMouseButtonRelease(XEvent& xev);
		void onKeyEvent(XEvent& xev);
		void onConfigureNotify( XEvent& xev);

	private:
		void create(Window::Type type);
	
		void destroy();
		
		bool isMinimized();
		
		bool isMaximazed();

	private:
		Atom AtomAppWake;
		Atom AtomWindowResize;
		Atom AtomWindowMove;
		Atom AtomWindowClosed;
		Atom AtomWMProtocols;
		Pt::Hmi::Application&        _app; 
		Pt::Hmi::PaintSurface*	     _surface;
		Pt::Signal<const Pt::Event&> _windowEvent;
		KeyEvent					 _keyEvent;
		PointingEvent				 _pointerEvent;
		ResizeEvent					 _resizeEvent;
		PositionEvent				 _positionEvent;
		ActivateEvent				 _activateEvent;
		bool						 _forceTopMost;		
		::Window  		    		 _window;
		::GC 						 _brushGc;
		::Display* 					 _display;
		std::vector<char> 			 _pixelBuffer;
		bool						 _forceTopMost;
		int _x;
		int _y;
		int _width;
		int _height;
		bool _showTitle;
};

} // namespace

} // namespace

#endif // include guard
