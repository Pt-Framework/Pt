/* Copyright (C) 2013 Laurentiu-Gheorghe Crisan
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
#include <Pt/Forms/Button.h>

namespace Pt{
namespace Forms{

Button::Button()
: _defController(_defModel, _defView)
{
	_defModel.ForeColor.set(Pt::Gfx::ARgbColor(0,0,0));
	_defModel.BorderWidth.set(1);
	_defModel.BorderStyle.set(Hmi::BorderStyleType::Widget);

	setButton(_defController);
}

void Button::setButton(Pt::Hmi::Button& controller)
{
	_currentController = &controller;
    button().PressedAction += Pt::slot(*this, &Button::handleOnClicked);
    button().CheckedAction += Pt::slot(*this, &Button::handleOnChecked);
}

void Button::setToggleButton(bool toggle)
{
	if(toggle)
		buttonModel().ButtonType = Pt::Hmi::ButtonType::Toggle;       
	else
		buttonModel().ButtonType = Pt::Hmi::ButtonType::Press;       
}

bool Button::isToggleButton() const
{
	return (buttonModel().ButtonType.get() == Pt::Hmi::ButtonType::Toggle);	
}
    
void Button::setCaption(const std::string& caption)
{
    buttonModel().Caption = caption;    
}

const std::string& Button::caption() const
{
	return buttonModel().Caption.get();
}

void Button::setSize(const Pt::Gfx::SizeF& size)
{
	buttonModel().Size = size;        
}

const Pt::Gfx::SizeF& Button::size() const
{
	return buttonModel().Size.get();        
}

void Button::setPosition(const Pt::Gfx::PointF& position)
{
    buttonModel().Position = position; 
}

const Pt::Gfx::PointF& Button::position() const
{
	return  buttonModel().Position.get();
}

void Button::onClicked()
{
    ClickedAction.send();
}
    
void Button::onChecked(bool state)
{
    CheckedAction.send(state);
}    

void Button::handleOnClicked(Hmi::Controller* sender)
{
    onClicked();
}

void Button::handleOnChecked(Hmi::Controller* sender, bool state)
{
    onChecked(state);
}
    
void Button::setActionKey(const std::string& keyString)
{
	buttonModel().ActionKey = keyString;
}

const std::string& Button::actionKey() const
{
	return buttonModel().ActionKey.get();
}

const Hmi::ButtonModel& Button::buttonModel() const
{
	return _currentController->buttonModel();
}

const Hmi::Button& Button::button() const
{
	return *_currentController;
}

Hmi::ButtonModel& Button::buttonModel()
{
	return _currentController->buttonModel();
}

Hmi::Button& Button::button()
{
	return *_currentController;
}

Button::~Button()
{
}
 
}}
