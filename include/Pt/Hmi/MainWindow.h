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
#ifndef Pt_Hmi_MainWindow_H
#define Pt_Hmi_MainWindow_H

#include <Pt/Hmi/Window.h>
#include <Pt/Hmi/WindowManager.h>
#include <Pt/Hmi/ChildWindow.h>

namespace Pt{
namespace Hmi{

class MainWindowImpl;

class PT_HMI_API MainWindow  : public Window
{
	public:    
	  MainWindow(MainWindow* parent = 0);    
    
		virtual ~MainWindow();    

		void setTopMost( bool topMost );						

    virtual PaintSurface& windowSurface();

	protected:	
		virtual void setSize(const Ui::SizeF& size);

		virtual void setPosition(const Ui::PointF& pos);

		virtual void setCaption( const std::string& c);	       		

		virtual void setClosed(bool c);		

		virtual void setFocus(bool f);

  protected:
    virtual void onInvalidate();	

	private: 					
		void onPositionChanged(const Ui::PointF& prop);    

		void onClosedChanged(const bool& closed);
	  
		void onVisibleChanged(const bool& visible);

		void onCaptionChanged(const std::string& p);
	  
		void onShowTitleChanged(const bool& p);
	  
		void onShowMinimizedButtonChanged(const bool& p);
	  
		void onShowMaximizeButtonChanged(const bool& p);
	  
		void onShowSysMenuChanged(const bool& p);

		void onTopMostChanged(const bool& p);
	  
		void onWindowStateChanged(const Hmi::WindowState::Type& p);
	  
		void onBorderChanged(const WindowBorder::Type& p);
	  
		void onShowInTaskbarChanged(const bool& p);
	  
		void onIconChanged(const Ui::Image & p);    		

		void onEnabledChanged(const bool & p);

		void onMinSizeChnaged(const Ui::SizeF& prop);
	
		void onMaxSizeChnaged(const Ui::SizeF& prop);
		
	private:
		MainWindowImpl*	 _impl;    
};

}}

#endif
