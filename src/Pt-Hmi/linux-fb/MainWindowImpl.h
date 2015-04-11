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
#include <Pt/Gfx/Rgb888Image.h>
#include <Pt/Gfx/Rgb565Image.h>
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
#include <Pt/Gfx/ARgbImage.h>
#include <linux/input.h>

namespace Pt {
namespace Hmi {

class Window;
class Application;

class MainWindowImpl  : public Pt::Connectable
{
	public:
		MainWindowImpl(Window* Window);
    
		~MainWindowImpl();

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

		void setMinSize(const Pt::Gfx::SizeF& s);
	
		void setMaxSize(const Pt::Gfx::SizeF& s);

		void setTopMost(bool top)
		{
		}
	
	protected:
    void onSizeChanged(const Property<Pt::Gfx::SizeF>& prop);
    void onPositionChanged(const Property<Pt::Gfx::PointF>& prop);    

	private:    

		Window* _apiWindow;
		Pt::Hmi::ChildWindow* _windowImpl;
		Pt::Hmi::Application& _app;

};

}} // namespace

#endif
