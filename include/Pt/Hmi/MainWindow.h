/* Copyright (C) 2015 Marc Boris Duerner 
 * Copyright (C) 2015 Laurentiu-Gheorghe Crisan
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#ifndef Pt_Hmi_MainWindow_H
#define Pt_Hmi_MainWindow_H

#include <Pt/Hmi/Window.h>
#include <Pt/Hmi/WindowManager.h>
#include <Pt/Hmi/ChildWindow.h>
#include <Pt/Hmi/WindowProperties.h>

namespace Pt{
namespace Hmi{

class MainWindowImpl;

class PT_HMI_API MainWindow  : public Window
{
	public:    
	    MainWindow(MainWindow* parent = 0);    
    
		~MainWindow();    

		void setTopMost( bool topMost );

	protected:
	    virtual void onInvalidate();	     		
      virtual void onSizeChanged(const Property<Pt::Gfx::SizeF>& prop);

	private: 					

		void onPositionChanged(const Property<Pt::Gfx::PointF>& prop);    

		void onClosedChanged(const Property<bool> & closed);
	  
		void onVisibleChanged(const Property<bool> & visible);

		void onCaptionChanged(const Property<std::string> & p);
	  
		void onShowTitleChanged(const Property<bool> & p);
	  
		void onShowMinimizedButtonChanged(const Property<bool> & p);
	  
		void onShowMaximizeButtonChanged(const Property<bool> & p);
	  
		void onShowSysMenuChanged(const Property<bool> & p);

		void onTopMostChanged(const Property<bool> & p);
	  
		void onWindowStateChanged(const Property<WindowState::Type> & p);
	  
		void onBorderChanged(const Property<WindowBorder::Type> & p);
	  
		void onShowInTaskbarChanged(const Property<bool> & p);
	  
		void onIconChanged(const Property<Pt::Gfx::ARgbImage> & p);    		

		void onEnabledChanged(const Property<bool> & p);

		void onMinSizeChnaged(const Property<Pt::Gfx::SizeF>& prop);
	
		void onMaxSizeChnaged(const Property<Pt::Gfx::SizeF>& prop);

	private:
		MainWindowImpl*	 _impl;    

};

}}

#endif
