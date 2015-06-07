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
#include <Pt/Hmi/WindowProperties.h>
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

		void setCaption(const std::string& text);

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
	
	protected:
		virtual void onInvalidate();
		virtual void onRender();
		virtual void onPointerInput(const PointerEvent& ev);				
		virtual void onKeyInput(const KeyEvent& ev);

	private:    
		Window* _apiWindow;
		Pt::Hmi::Application& _app;
};

}} // namespace

#endif
