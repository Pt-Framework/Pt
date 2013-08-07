/* Copyright (C) 2013 Marc Boris Duerner 
 * Copyright (C) 2013 Laurentiu-Gheorghe Crisan
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

#include <Pt/Hmi/ButtonController.h>
#include <Pt/Hmi/ButtonModel.h>
#include <Pt/Hmi/PointingDevice.h>

namespace Pt{
namespace Hmi{

class PointingDevice;
class GfxOutput;

ButtonController::ButtonController()
{
}

ButtonController::~ButtonController()
{
}

void ButtonController::onKeyInput(const KeyEvent& ev)
{	
	ButtonModel* model = dynamic_cast<ButtonModel*>(gfxModel());
	
	if( model == 0)
		return;

	
	if(!model->Enable.get())
	{
		LabelController::onKeyInput(ev);
		return;
	}

	if(!model->Visible.get())
	{
		LabelController::onKeyInput(ev);
		return;
	}

	switch(model->ButtonType.get())
	{
		case ButtonType::Press:
		{
			if(ev.virtualCode() == ' ' && model->Focused.get())	
			{
				model->ButtonState = (ev.state() == KeyEvent::KeyDown) ? DeviceButton::Pressed : DeviceButton::Released;
			}
			else if(ev.shortCutKey() == model->ActionKey.get())
			{
				model->ButtonState = (ev.state() == KeyEvent::KeyDown) ? DeviceButton::Pressed : DeviceButton::Released;				
				model->Focused = false;
				model->Focused = true;
			}
			else
			{
				model->ButtonState = DeviceButton::Released;
			}
		}
		break;

		case ButtonType::Toggle:
		{
			if(ev.virtualCode() == ' ' && model->Focused.get())		
			{
				if((ev.state() == KeyEvent::KeyDown))
				{
					if(model->ButtonState.get() == DeviceButton::Pressed)
						model->ButtonState = DeviceButton::Released;
					else
						model->ButtonState = DeviceButton::Pressed;
				}
			}
			else if(ev.shortCutKey() == model->ActionKey.get())
			{			
				if((ev.state() == KeyEvent::KeyDown))
				{
					model->ButtonState = (model->ButtonState.get() == DeviceButton::Pressed) ? DeviceButton::Released : DeviceButton::Pressed;											
					model->Focused = false;
					model->Focused = true;
				}
			}
		}			
		break;
	}
	
	LabelController::onKeyInput(ev);
}

void ButtonController::onPointerInput(const PointingEvent& ev)
{	
	ButtonModel* model = dynamic_cast<ButtonModel*>(gfxModel());
	
	if( model == 0)
		return;

	Pt::Gfx::PointF point = toClient(Pt::Gfx::PointF(ev.x(), ev.y()));
	
	if(!model->Enable.get())
	{
		LabelController::onPointerInput(ev);
		return;
	}

	if(!model->Visible.get())
	{
		LabelController::onPointerInput(ev);
		return;
	}

	if(!model->contains(point))
	{
		model->Armed = false;
		LabelController::onPointerInput(ev);
		return;
	}

	model->Armed = true;

	if( ev.buttons().size() == 0)
	{
		LabelController::onPointerInput(ev);
		return;
	}

	switch(model->ButtonType.get())
	{
		case ButtonType::Press:
		{
			model->ButtonState = ev.buttons()[0].state();
			if(ev.buttons()[0].state() == DeviceButton::Pressed)
			{
				model->Focused = false;
				model->Focused = true;
			}
		}
		break;

		case ButtonType::Toggle:
		{
			if(ev.buttons()[0].state() == DeviceButton::Pressed )
			{
				model->Focused = false;
				model->Focused = true;

				if(model->ButtonState.get() == DeviceButton::Pressed)
					model->ButtonState = DeviceButton::Released;
				else
					model->ButtonState = DeviceButton::Pressed;
			}
		}				
		break;
	}

	LabelController::onPointerInput(ev);
}

}}
