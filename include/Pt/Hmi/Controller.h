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
#include <Pt/Hmi/Output.h>
#include <Pt/Hmi/Model.h>
#include <Pt/Hmi/Api.h>
#include <Pt/Hmi/PointingEvent.h>
#include <Pt/Hmi/KeyEvent.h>

namespace Pt{
namespace Hmi{

class PT_HMI_API Controller  : public Pt::Connectable
{
protected:
	Controller(Model& model);

public:
	virtual ~Controller();
	
	inline Model& model() 
	{
		return _model;
	}

	inline const Model& model() const 
	{
		return _model;
	}

	void addOutput(Output* device);

	void removeOutput(Output* device);

	inline const std::vector<Output*>& outputDevices() const
	{
		return _outputDevices;
	}

	inline const std::vector<Controller*>& children() const
	{
		return _children;
	}

	void addChild(Controller* cotroller);

	inline void removeChild(Controller* controller);

	inline const Controller* widgetParent() const
	{
		return _parent;
	}

	inline Controller* widgetParent() 
	{
		return _parent;
	}

	inline void setWidgetParent(Controller* p)
	{
		_parent = p;
	}

	inline void notifyModelChanged(bool created)
	{
		onModelChanged(created, model());
	}

	inline void devicePointerInput(const PointingEvent& ev)
	{
		if( this != ev.controller())
			return;

		onPointerInput(ev);
	}

	inline void notifyPointerInput(const PointingEvent& ev)
	{
		onPointerInput(ev);
	}

	inline void deviceKeyInput(const KeyEvent& ev)
	{
		if( this != ev.controller())
			return;

		onKeyInput(ev);
	}

	
	inline void notifyKeyInput(const KeyEvent& ev)
	{
		onKeyInput(ev);
	}

	inline bool moveFocusNext()
	{
		return onMoveFocusNext();
	}
	
	inline bool moveFocusPrev()
	{
		return onMoveFocusPrev();
	}
		
	void invalidate();

protected:		
	void output();		

	virtual bool onMoveFocusNext()
	{
		return false;
	}
	
	virtual bool onMoveFocusPrev()
	{
		return false;
	}

	virtual void onModelChanged(bool created, const Model& model)
	{ }
	
	virtual void onPointerInput(const PointingEvent& ev)
	{ }

	virtual void onKeyInput(const KeyEvent& ev)
	{ }

	 
private:
	void modelChanged(const Model& model)
	{
		onModelChanged(false, model);
	}

private:
	std::vector<Output*>	_outputDevices;
	std::vector<Controller*>    _children;
	Controller*					_parent;
	Model&						_model;
};

}}
#endif
