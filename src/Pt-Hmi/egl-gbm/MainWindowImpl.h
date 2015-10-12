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
#ifndef Pt_Hmi_WindowImpl_h
#define Pt_Hmi_WindowImpl_h

#include <Pt/Connectable.h>
#include <Pt/Signal.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Hmi/Api.h>
#include <Pt/Hmi/Api.h>
#include <Pt/Hmi/KeyEvent.h>
#include <Pt/Hmi/WindowState.h>
#include <Pt/Hmi/ChildWindow.h>

namespace Pt {
namespace Hmi {

class MainWindow;
class Application;

class MainWindowImpl  : public ChildWindow
{
	public:
		MainWindowImpl(MainWindow* Window);
    
		virtual ~MainWindowImpl();

		void create();
	
		void destroy();

		void show();

		void hide();

		void activate();

		void setWindowPos(const Gfx::PointF& p);

		void setWindowSize(const Gfx::SizeF& size);

		void showTitle(bool p);

		void setWindowCaption(const std::string& text);

		void showMinimizedButton(bool p);
  
		void showMaximizeButton(bool p);
  
		void showSysMenu(bool p);

		void setTopMost(bool force);
  
		void setWindowState(WindowState::Type p);
  
		void setBorder(WindowBorder::Type p);
  
		void showInTaskbar(bool p);
  
		void setIcon(const Gfx::Image& p);

		void setEnable(bool e);	

		void setMinSize(const Gfx::SizeF& s);
	
		void setMaxSize(const Gfx::SizeF& s);

		void bringToFront();

		void focus();

	protected:
		virtual void onInvalidate();
	  virtual void onRender(PaintSurface& paintSurface);

	private:
    Pt::Hmi::Application& _app;
    MainWindow*           _apiWindow;
};

}} // namespace

#endif
