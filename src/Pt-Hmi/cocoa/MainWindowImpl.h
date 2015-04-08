/* Copyright (C) 2013 Laurentiu-Gheorghe Crisan
 * Copyright (C) 2013 Marc Boris Dürner
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
#ifndef Pt_Hmi_ViewImpl_H
#define Pt_Hmi_ViewImpl_H

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
#include <Pt/Hmi/Window.h>
#include <Pt/System/Timer.h>
#include <map>

#ifdef __OBJC__
    #import <AppKit/NSWindow.h>
    #import <AppKit/NSGraphicsContext.h>
    #import <AppKit/NSView.h>
    #import <AppKit/NSScreen.h> 
#else
	struct NSRect;
	struct NSView;
  struct NSWindow;
	struct NSResponder;
	struct NSGraphicsContext;
#endif

namespace Pt{	
namespace Hmi{

class WindowImpl : public Pt::Connectable
{
	public:
		WindowImpl(PaintSurface* surface);

		virtual ~WindowImpl();

		void create();
	
		void destroy();

		void show();

		void hide();

		void render();

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

		Pt::Signal<const Pt::Event&>& windowEvent()
		{
			return _windowEvent;
		}
    
		NSView* view();
    
		NSWindow* window()
		{
				return _window;
		}

		PaintSurface* paintSurface()
		{
			return _surface;
		}

public:
	void onSize();
	void onPosition();
	void onClosing();
	void onMouseMove(double x,double y);
	void onLMouseDown(double x, double y);
	void onLMouseUp(double x, double y);
	void onKeyDown(int key);
	void onKeyUp(int key);
	void onSpezialKeyEvent(unsigned int mask);
	void onLostFocus();
    
private:
	void bringToFront();
	Pt::Gfx::PointF convertMousePosition(double x, double y);

private:
	NSWindow*				_window;
	NSView*					_view;
  Pt::Hmi::PaintSurface*				_surface;
	Pt::Signal<const Pt::Event&>	_windowEvent;
	KeyEvent											_keyEvent;
	PointingEvent									_pointerEvent;
	ResizeEvent										_resizeEvent;
	PositionEvent									_positionEvent;
	ActivateEvent									_activateEvent;
	Pt::System::Timer       _timer;
  bool					_showtitle;
  int						_level;
	std::string		_title;
	int _windowStyle;
	bool	_topMost;
};

}}
#endif