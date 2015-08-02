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
#include <Pt/Hmi/FocusEvent.h>
#include <Pt/Hmi/CloseEvent.h>
#include <Pt/Ui/Image.h>

namespace Pt {
namespace Hmi {

class ChildWindow;

class PT_HMI_API Window  : public Widget
{
	public:    		
    virtual ~Window();    

    ValueProperty<Ui::SizeF>                  MinimumSize;
    ValueProperty<Ui::SizeF>                  MaximumSize;
    ValueProperty<Hmi::WindowStartPosition::Type>       StartPostion;
    ValueProperty<Hmi::WindowState::Type>               State;    
    ValueProperty<bool>                            ShowInTaskbar;
    ValueProperty<bool>                            ShowTitle;
    ValueProperty<bool>                            ShowMinimizeButton;
    ValueProperty<bool>                            ShowMaximizeButton;
    ValueProperty<bool>                            ShowSysMenu;
    ValueProperty<Hmi::WindowBorder::Type>              Border;
    ValueProperty<Ui::Image>						           Icon;
    ValueProperty<bool>                            CanClose;
    ValueProperty<bool>                            FirstShow;
    ValueProperty<std::string>                     FocuseMoveKey;      
		ValueProperty<Hmi::WindowBorder::Type>				 WindowBorder;
				
		void setWindowParent(Window* parent);

		Window* windowParent() const;
		
		void addChildWindow(ChildWindow& w);

		void removeChildWindow(ChildWindow& w);

		const std::vector<ChildWindow*>& childWindows() const;

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

    virtual PaintSurface& windowSurface() = 0;

		Pt::Signal<const Pt::Event&>& eventReceived()
		{
			return _eventReceived;
		}      

	protected:
		Window(Window* parent = 0);    

	protected:		
		virtual void onKeyInput(const KeyEvent& ev);			
    virtual void onInvalidate();  			
		virtual void setClosed( bool close );				
    virtual void setSize(const Ui::SizeF& size);
		virtual void onSizeEvent(const SizeEvent& ev);
		virtual void onPositionEvent( const PositionEvent& ev);
		virtual void onFocusEvent( const FocusEvent& ev);
		virtual void onCloseEvent(const CloseEvent& ev);

	protected:
		Window*				_winParent;
		WindowManager _windowManager;		
		bool	_isClosed;
    Pt::Signal<const Pt::Event&> _eventReceived;
};

}}

#endif
