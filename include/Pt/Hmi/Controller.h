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
#ifndef Pt_Hmi_Controller_H
#define Pt_Hmi_Controller_H

#include <Pt/Connectable.h>
#include <Pt/Hmi/InputDevice.h>
#include <Pt/Hmi/OutputDevice.h>
#include <Pt/Hmi/Model.h>
#include <Pt/Hmi/Renderer.h>
#include <Pt/Hmi/Api.h>
#include <Pt/Hmi/PointingEvent.h>

namespace Pt{
namespace Hmi{

class PT_HMI_API Controller  : public Pt::Connectable
{
protected:
	Controller(Controller* parent = 0);

public:
	virtual ~Controller();
	
	void setModel(Model* model);

	inline Model* model() 
	{
		return _model;
	}

	inline const Model* model() const 
	{
		return _model;
	}
	
	inline void setRenderer(Renderer* r)
	{
		_renderer = r;
	}

	inline Renderer* renderer()
	{
		return _renderer;
	}

	inline const Renderer* renderer() const 
	{
		return _renderer;
	}

	void addInputDevice(InputDevice* device);

	void removeInputDevice(InputDevice* device);

	void addOutputDevice(OutputDevice* device);

	void removeOutputDevice(OutputDevice* device);

	inline const std::vector<InputDevice*>& inputDevices() const
	{
		return _inputDevices;
	}

	inline const std::vector<OutputDevice*>& outputDevices() const
	{
		return _outputDevices;
	}

	inline const std::vector<Controller*>& children() const
	{
		return _children;
	}

	void addChild(Controller* cotroller);

	inline void removeChild(Controller* controller);

	inline const Controller* parent() const
	{
		return _parent;
	}

	inline Controller* parent() 
	{
		return _parent;
	}

	inline void setParent(Controller* p)
	{
		_parent = p;
	}


	inline void notifyModelChanged(bool created)
	{
		onModelChanged(created);
	}

	inline void notifyInput2D(const PointingEvent& ev)
	{
		onInput2D(ev);
	}
	
	void output();		

protected:	

	virtual void onModelChanged(bool created = false)
	{
	}
	
	virtual void onInput2D(const PointingEvent& ev)
	{

	}
	 
private:
	void modelChanged()
	{
		onModelChanged(false);
	}
private:
	std::vector<InputDevice*>	_inputDevices;
	std::vector<OutputDevice*>	_outputDevices;
	Model*						_model;
	Renderer*					_renderer;
	std::vector<Controller*>    _children;
	Controller*					_parent;
};

}}
#endif
