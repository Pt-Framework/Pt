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

#ifndef Pt_Hmi_MainWindowImpl_H
#define Pt_Hmi_MainWindowImpl_H

#include <Pt/Connectable.h>
#include <Pt/Signal.h>
#include <Pt/Hmi/Api.h>
#include <Pt/Hmi/KeyEvent.h>
#include <Pt/Hmi/PointerEvent.h>
#include <Pt/Hmi/PaintSurface.h>
#include <Pt/Hmi/WindowProperties.h>
#include <Windows.h>
#include <map>

namespace Pt{

namespace Hmi{

class Application;
class Window;

class MainWindowImpl : public Pt::Connectable
{
public:
	MainWindowImpl(Window* window);

	virtual ~MainWindowImpl();
	
	void create();
	
  void destroy();

  void show();
	
  void hide();

	void render();

  void setPosition(const Ui::PointF& p);

  void setSize(const Ui::SizeF& size);

  void showTitle(bool p);

  void setCaption(const std::string& text);

  void showMinimizedButton(bool p);
  
  void showMaximizeButton(bool p);
  
  void showSysMenu(bool p);

  void setTopMost(bool force);
  
  void setWindowState(WindowState::Type p);
  
  void setBorder(WindowBorder::Type p);
  
  void showInTaskbar(bool p);
  
  void setIcon(const Ui::Image& p);

	void setEnable(bool e);	

	void setMinSize(const Ui::SizeF& s);
	
	void setMaxSize(const Ui::SizeF& s);

	void focus();

protected:
	void onWindowEvent(HWND wnd, unsigned int msg, WPARAM wparam, LPARAM lparam, bool& handled);
	void onPaint();
	void onSize(WPARAM wparam, LPARAM lparam);
	void onMouse(unsigned int msg,  WPARAM wparam, LPARAM lparam);
	void onKey(unsigned int ms, WPARAM wparam, LPARAM lparam);
	void onMove(LPARAM lparam);	
	void onClose();
	void onFocus(bool f);

private:	
  HWND													_hwnd;
	Pt::Hmi::Application&         _app; 
	KeyEvent											_keyEvent;
	PointerEvent									_pointerEvent;
	Window*												_window;
	bool													_forceTopMost;
	Ui::Size                 _minSize;
	Ui::Size                 _maxSize;
};

}}

#endif