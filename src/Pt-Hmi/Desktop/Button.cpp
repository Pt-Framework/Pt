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
#include <Pt/Hmi/Desktop/Button.h>
#include <Pt/Hmi/ButtonController.h>
#include <Pt/Hmi/ButtonModel.h>
#include <Pt/Hmi/ButtonRenderer.h>
#include <Pt/Hmi/Controller.h>

namespace Pt{
namespace Hmi{
namespace Desktop{

Button::Button()
: _defController( new ButtonController())
, _defModel( new ButtonModel())
, _defRenderer( new ButtonRenderer())
{
	_defController->setModel(_defModel);
	_defController->setRenderer(_defRenderer);	
	_defModel->ForeColor.set(Pt::Gfx::ARgbColor(0,0,0));
	_defModel->BorderWidth.set(1);
	_defModel->BorderStyle.set(BorderStyleType::Widget);

    _defController->PressedAction += Pt::slot(*this, &Button::handleOnClicked);
    _defController->CheckedAction += Pt::slot(*this, &Button::handleOnChecked);
	setController(*_defController);
}
    
void Button::setToggleButton(bool toggle)
{
	if(toggle)
		buttonModel()->ButtonType = Pt::Hmi::ButtonType::Toggle;       
	else
		buttonModel()->ButtonType = Pt::Hmi::ButtonType::Press;       
}
    
void Button::setCaption(const char* caption)
{
    buttonModel()->Caption = caption;    
}

void Button::setSize(const Pt::Gfx::SizeF& size)
{
	buttonModel()->Size = size;        
}

void Button::setPosition(const Pt::Gfx::PointF& position)
{
    buttonModel()->Position = position; 
}

void Button::onClicked()
{
    ClickedAction.send();
}
    
void Button::onChecked(bool state)
{
    CheckedAction.send(state);
}    

void Button::handleOnClicked(Controller* sender)
{
    onClicked();
}

void Button::handleOnChecked(Controller* sender, bool state)
{
    onChecked(state);
}
    
void Button::setActionKey(const char* keyString)
{
	buttonModel()->ActionKey = keyString;
}

ButtonModel* Button::buttonModel()
{
	return (ButtonModel*) buttonController()->model();
}

ButtonController* Button::buttonController()
{
	return (ButtonController*)&controller();
}

Button::~Button()
{
	delete _defController;
	delete _defModel;
	delete _defRenderer;
}
 
}}}
