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
#ifndef Pt_Hmi_PointerEvent_h
#define Pt_Hmi_PointerEvent_h

#include <Pt/Hmi/Api.h>
#include <Pt/Hmi/DeviceButton.h>
#include <Pt/Hmi/DeviceControlDial.h>
#include <Pt/Hmi/Event.h>
#include <vector>

namespace Pt{
namespace Hmi{


class PointerState 
{
	public:
			enum Type 
			{            	
				PressToRelease,
				ReleaseToPress
			};

	public:
			PointerState(PointerState::Type v) 
			: _value(v) 
			{
			}
			
			operator PointerState::Type() const
			{ 
				return _value; 
			}

			PointerState& operator=(PointerState::Type v) 
			{
					_value = v;
					return *this;
			}

	private:
			PointerState::Type _value;
};


class PT_HMI_API PointerEvent : public Pt::BasicEvent<PointerEvent>
{
public:	
	explicit PointerEvent()
	: _buttons(3)
	, _state( PointerState::PressToRelease)
	{
	}

	virtual ~PointerEvent()
	{
	}

	inline void setX(double x)
	{
		_x = x;
	}
	
	inline void setY(double y)
	{
		_y = y;
	}

	inline void addX(double x)
	{
		_x += x;
	}
	
	inline void addY(double y)
	{
		_y += y;
	}

	inline double x() const
	{
		return _x;
	}

	inline double y() const
	{
		return _y;
	}

	inline const std::vector<DeviceButton>& buttons() const
	{
		return _buttons;
	}

	inline const std::vector<DeviceControlDial>& controlDial() const 
	{
		return _controlDial;
	}

	inline std::vector<DeviceButton>& buttons()
	{
		return _buttons;
	}

	inline std::vector<DeviceControlDial>& controlDial()
	{
		return _controlDial;
	}	

	PointerState pointerState() const
	{
		return _state;
	}

	void setPointerState( PointerState s )
	{
		_state = s;
	}

private:
	double _x;
	double _y;
	std::vector<DeviceButton>	     _buttons;
	std::vector<DeviceControlDial> _controlDial;	
	PointerState                   _state;
};

}}

#endif

