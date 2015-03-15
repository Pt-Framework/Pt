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
#include <Pt/Hmi/Widget.h>
#include <Pt/Hmi/Window.h>

namespace Pt{
namespace Hmi{

Controller::Controller(Model& model)
: _model(model)
, _parent(0)
{ 
	_model.changed() += Pt::slot(*this, &Controller::modelChanged);	
}

Controller::~Controller()
{ 
}

void Controller::addOutput(Output* device)
{
	_outputDevices.push_back(device);
}

void Controller::removeOutput(Output* device)
{
	for(size_t i = 0; i < _outputDevices.size(); ++i)
	{
		if(_outputDevices[i] != device)
			continue;
		
		_outputDevices.erase(_outputDevices.begin() + i);
		return;
	}
}

void Controller::addChild(Controller* base)
{
	_children.push_back(base);
	base->setWidgetParent(this);
}

void Controller::removeChild(Controller* base)
{
	for(size_t i = 0; i < _children.size(); ++i)
	{
		if(_children[i] != base)
			continue;
		
		_children.erase(_children.begin() + i);
		base->setWidgetParent(0);
		return;
	}			
}	

void Controller::output()
{	
	for( size_t i = 0; i < _outputDevices.size(); ++i)
		_outputDevices[i]->output(&model());
}
	
}}
