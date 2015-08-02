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

#include <Pt/Hmi/Api.h>
#include <Pt/Ui/Image.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <Pt/Connectable.h>
#include <Pt/Signal.h>
#include <Pt/Hmi/Api.h>
#include <Pt/Hmi/KeyEvent.h>
#include <Pt/Hmi/WindowState.h>
#include <Pt/Hmi/ChildWindow.h>
#include <linux/input.h>
#include <Pt/Ui/Size.h>
#include <Pt/Ui/Point.h>

namespace Pt {
namespace Hmi {

class Window;
class Application;

class MainWindowImpl  : public ChildWindow
{
	public:
		MainWindowImpl(Window* Window);
    
		~MainWindowImpl();

		void create();
	
		void destroy();

		void show();

		void hide();

		void setWindowPos(const Ui::PointF& p);

		void setWindowSize(const Ui::SizeF& size);

		void showTitle(bool p);

		void setWindowCaption(const std::string& text);

		void showMinimizedButton(bool p);
  
		void showMaximizeButton(bool p);
  
		void showSysMenu(bool p);

		void setForceTopMost(bool force);
  
		void setWindowState(WindowState::Type p);
  
		void setBorder(WindowBorder::Type p);
  
		void showInTaskbar(bool p);
  
		void setIcon(const Ui::Image& p);

		void setEnable(bool e);	

		void setMinSize(const Ui::SizeF& s);
	
		void setMaxSize(const Ui::SizeF& s);

		void setTopMost(bool top)
		{
		}
	
    virtual PaintSurface& windowSurface()
    {
      return ChildWindow::windowSurface(); 
    }

    void render();
    
	protected:
		virtual void onInvalidate();
	  virtual void onRender(PaintSurface& paintSurface);
		virtual void onPointerInput(const PointerEvent& ev);				
		virtual void onKeyInput(const KeyEvent& ev);
		virtual void onSizeEvent(const SizeEvent& ev);
		virtual void onPositionEvent( const PositionEvent& ev);
		virtual void onFocusEvent( const FocusEvent& ev);
		virtual void onCloseEvent(const CloseEvent& ev);

    
    void onFocusChanged(bool focused);

	private:    
		Window* _apiWindow;
		Pt::Hmi::Application& _app;
    bool _forceTopMost;
};

}} // namespace

#endif
