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

#include <Pt/Hmi/Controller.h>
#include <Pt/Hmi/Input2DDevice.h>

namespace Pt{
namespace Hmi{

Controller::Controller(Controller* parent)
: _model(0)
, _renderer(0)
, _parent(parent)
{ 

}

Controller::~Controller()
{ 
}

void Controller::setModel(Model* model)
{
	_model = model;
	_model->Changed += Pt::slot(*this, &Controller::modelChanged);
	notifyModelChanged(true);
}

void Controller::addInputDevice(InputDevice* device)
{
	Input2DDevice* pointerDev = dynamic_cast<Input2DDevice*>(device);
	
	if( pointerDev != 0)
		pointerDev->Event += Pt::slot(*this, &Controller::notifyInput2D);

	_inputDevices.push_back(device);
}

void Controller::removeInputDevice(InputDevice* device)
{
	for(size_t i = 0; i < _inputDevices.size(); ++i)
	{
		if(_inputDevices[i] == device)
		{
			_inputDevices.erase(_inputDevices.begin() + i);
			return;
		}
	}
}

void Controller::addOutputDevice(OutputDevice* device)
{
	_outputDevices.push_back(device);
}

void Controller::removeOutputDevice(OutputDevice* device)
{
	for(size_t i = 0; i < _outputDevices.size(); ++i)
	{
		if(_outputDevices[i] == device)
		{
			_outputDevices.erase(_outputDevices.begin() + i);
			return;
		}
	}
}

void Controller::addChild(Controller* base)
{
	_children.push_back(base);
	base->setParent(this);
}

void Controller::removeChild(Controller* base)
{
	for(size_t i = 0; i < _children.size(); ++i)
	{
		if(_children[i] == base)
		{
			_children.erase(_children.begin() + i);
			return;
		}
	}			
}	


void Controller::output()
{	
	for( size_t i = 0; i < _children.size(); ++i)
		_children[i]->output();

	for( size_t i = 0; i < _outputDevices.size(); ++i)
	{
		_outputDevices[i]->output(model());
	}	 
}
	
}}