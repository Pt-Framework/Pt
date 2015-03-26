/* Copyright (C) 2013 Laurentiu-Gheorghe Crisan
 * 
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

#ifndef Pt_Hmi_WindowImpl_H
#define Pt_Hmi_WindowImpl_H

#include <Pt/Connectable.h>
#include <Pt/Signal.h>
#include <Pt/Hmi/Api.h>
#include <Pt/Hmi/KeyEvent.h>
#include <Pt/Hmi/PointingEvent.h>
#include <Pt/Hmi/PositionEvent.h>
#include <Pt/Hmi/ResizeEvent.h>
#include <Pt/Hmi/CloseEvent.h>
#include <Pt/Hmi/PaintSurface.h>
#include <Pt/Hmi/Window.h>
#include <Windows.h>
#include <map>

namespace Pt{

namespace Hmi{

class Application;

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

  void setTopMost();
  
  void setWindowState(WindowState::Type p);
  
  void setBorder(WindowBorder::Type p);
  
  void showInTaskbar(bool p);
  
  void setIcon(const Pt::Gfx::ARgbImage& p);

protected:
	void onWindowEvent(HWND wnd, unsigned int msg, WPARAM wparam, LPARAM lparam, bool& handled);

protected:
	void onPaint();
	void onSize(WPARAM wparam, LPARAM lparam);
	void onMouse(unsigned int msg,  WPARAM wparam, LPARAM lparam);
	void onKey(unsigned int ms, WPARAM wparam, LPARAM lparam);
	void onMove();
	void onClosing();	

protected:	
	void drawIndependentImage(size_t x, size_t y, const char* data, size_t width, size_t height);

private:	
  HWND													_hwnd;
	Pt::Hmi::Application&         _app; 
  Pt::Hmi::PaintSurface*				_surface;
	Pt::Signal<const Pt::Event&>	_windowEvent;
	KeyEvent											_keyEvent;
	PointingEvent									_pointerEvent;
	ResizeEvent										_resizeEvent;
	PositionEvent									_positionEvent;
};

}}

#endif