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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA*/
#ifndef Pt_Hmi_Window_H
#define Pt_Hmi_Window_H

#include <Pt/Hmi/Widget.h>
#include <Pt/Hmi/WindowManager.h>
#include <Pt/Hmi/WindowProperties.h>

namespace Pt {
namespace Hmi {

class ChildWindow;

class PT_HMI_API Window  : public Widget
{
	public:    		
    virtual ~Window();    

    Property<Pt::Gfx::SizeF>                  MinimumSize;
    Property<Pt::Gfx::SizeF>                  MaximumSize;
    Property<WindowStartPosition::Type>       StartPostion;
    Property<WindowState::Type>               State;    
    Property<bool>                            ShowInTaskbar;
    Property<bool>                            ShowTitle;
    Property<bool>                            ShowMinimizeButton;
    Property<bool>                            ShowMaximizeButton;
    Property<bool>                            ShowSysMenu;
    Property<std::string>                     Caption;
    Property<WindowBorder::Type>              Border;
    Property<Pt::Gfx::ARgbImage>              Icon;
    Property<bool>                            Closed;
    Property<bool>                            CanClose;
    Property<bool>                            FirstShow;
    Property<std::string>                     FocuseMoveKey;      
		Property<WindowBorder::Type>							WindowBorder;
				
		void setWindowParent(Window* parent);

		Window* windowParent() const;
		
		void addChildWindow(ChildWindow& w);

		void removeChildWindow(ChildWindow& w);

		const std::vector<ChildWindow*>& childWindows() const;

		WindowManager& windowManager()
		{
			return _windowManager;
		}

	protected:
		Window(Window* parent = 0);    

	protected:		
		virtual void onKeyInput(const KeyEvent& ev);	
    virtual void onInvalidate();	  	

	protected:
		Window*				_winParent;
		WindowManager _windowManager;
};

}}

#endif
