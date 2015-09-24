/* Copyright (C) 2013 Marc Boris Duerner 
   Copyright (C) 2013 Laurentiu-Gheorghe Crisan
  
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
#ifndef Pt_Hmi_Button_H
#define Pt_Hmi_Button_H

#include <Pt/Hmi/Label.h>
#include <Pt/System/Timer.h>
#include <Pt/Gfx/Image.h>

namespace Pt{
namespace Hmi{

class PointerEvent;

namespace ButtonType
{
	enum Type
	{
		Press,
		Toggle
	};
}


class PT_HMI_API Button  : public Label
{
public:
	Button();
	virtual ~Button();

	ValueProperty<DeviceButton::State> ButtonState;		
	ValueProperty<bool>								 Hover;		
	ValueProperty<ButtonType::Type>    ButtonType;	
	ValueProperty<int>								 DoublePressTimeInMs; 
  ValueProperty<Gfx::Image>					 Image;	
  ValueProperty<Align::Type>         ImageAlign;

	Signal<bool>											 Checked;	  
	Signal<>													 Clicked;	
	Signal<>													 DoubleClicked;	

protected:
	virtual void onPressedAction();
	virtual void onDoublePressedAction();
	virtual void onMnemonic();
	virtual void onShortcutKey(KeyEvent::KeyState state);
	virtual void onActionKey(KeyEvent::KeyState state);
	virtual void onPointerInput(const PointerEvent& ev);
	virtual void onKeyInput(const KeyEvent& ev);
	virtual void onButtonStateChanged( const DeviceButton::State& prop);
	virtual void onDoublePressedTimeout();
	virtual void onRender(PaintSurface& surface);

private:	
	bool _pressed;
	bool _timeout;
	int _pressCounter;
	Pt::System::Timer _doublePressTimer;
	DeviceButton::State _lastPointerState;
};

}}
#endif