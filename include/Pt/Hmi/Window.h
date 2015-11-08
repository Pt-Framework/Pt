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
#ifndef Pt_Hmi_Window_H
#define Pt_Hmi_Window_H

#include <Pt/Hmi/Widget.h>
#include <Pt/Hmi/WindowManager.h>
#include <Pt/Hmi/WindowStartPosition.h>
#include <Pt/Hmi/WindowState.h>
#include <Pt/Hmi/WindowBorder.h>
#include <Pt/Hmi/ActivateEvent.h>
#include <Pt/Hmi/CloseEvent.h>
#include <Pt/Gfx/Image.h>

namespace Pt {
namespace Hmi {

class ChildWindow;

class PT_HMI_API Window : public Widget
{
	public:    		
    virtual ~Window();    

    ValueProperty<Gfx::SizeF>												MinimumSize;
    ValueProperty<Gfx::SizeF>												MaximumSize;
    ValueProperty<Hmi::WindowStartPosition::Type>   StartPostion;
    ValueProperty<Hmi::WindowState::Type>           State;    
    ValueProperty<bool>                             ShowInTaskbar;
    ValueProperty<bool>                             ShowTitle;
    ValueProperty<bool>                             ShowMinimizeButton;
    ValueProperty<bool>                             ShowMaximizeButton;
    ValueProperty<bool>                             ShowSysMenu;
    ValueProperty<Hmi::WindowBorder::Type>          Border;
    ValueProperty<Gfx::Image>						            Icon;
    ValueProperty<bool>                             CanClose;
    ValueProperty<bool>                             FirstShow;
    ValueProperty<std::string>                      FocuseMoveKey;      
		ValueProperty<Hmi::WindowBorder::Type>				  WindowBorder;
				
		Signal<bool> FocusedAction;

		void setWindowParent(Window* parent);

		Window* windowParent() const;
		
		void addChildWindow(ChildWindow& w);

		void removeChildWindow(ChildWindow& w);

		const std::vector<ChildWindow*> childWindows() const;

		WindowManager& windowManager()
		{
			return _windowManager;
		}

		bool isClosed() const
		{
			return _isClosed;
		}

		Signal<> Closed;	

		void close();    

    void activate();

    bool isActive() const
    {
      return _isActive;
    }

		void setPointedWidget( Widget* widget);

	protected:
		Window(Window* parent = 0);

	protected:		
		virtual void onKeyInput( const KeyEvent& ev );			
		virtual void onPointerInput( const PointerEvent& ev );	
		virtual void setClosed( bool close );				
		virtual void onActivateEvent(const ActivateEvent& ev);
		virtual void onRender( PaintSurface& surface );

	protected:
		void onSizeEvent(const SizeEvent& ev);
		void onPositionEvent( const PositionEvent& ev);		
		void onCloseEvent(const CloseEvent& ev);

	private:
		virtual void onActivate() = 0;

	protected:
		Window*				_winParent;
		Widget*				_pointedWidget;
//TODO:		Widget*				_focusWidget;

		WindowManager _windowManager;		
		bool	_isClosed;		
    bool  _isActive;
};

}}

#endif
