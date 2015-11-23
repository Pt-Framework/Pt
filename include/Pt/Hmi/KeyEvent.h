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
#ifndef Pt_Hmi_KeyEvent_h
#define Pt_Hmi_KeyEvent_h

#include <Pt/Types.h>
#include <Pt/Hmi/Api.h>
#include <Pt/Event.h>
#include <Pt/String.h>
#include <string>
#include <Pt/TextStream.h>
#include <Pt/Utf8Codec.h>
#include <Pt/String.h>
#include <sstream>

namespace Pt{
namespace Hmi{

class PT_HMI_API KeyEvent : public Pt::BasicEvent<KeyEvent>
{
public:

	enum KeyState
	{
		KeyNone,
		KeyDown,
		KeyUp
	};

public:	
	KeyEvent()
  : _alt(false)
	, _shift(false)
	, _ctrl(false)
	, _state(KeyNone)
	{
	}

	virtual ~KeyEvent()
	{
	}

	/**@brief Return the translate key to unicode.
	* 
	* @return The translate key to unicode*/
	inline const Pt::Char& unicode() const
	{
		return _unicode;
	}

	inline void setUnicode(Pt::uint32_t ucode)
	{
		_unicode = Pt::Char(ucode);
	}
	
	inline KeyState state() const
	{
		return _state;
	}

	inline void setState(KeyState s)
	{
		_state = s;
	}

	inline void setAlt(bool b)
	{
		_alt = b;
	}

	inline bool alt() const
	{
		return _alt;
	}

	inline void setShift(bool b)
	{
		_shift = b;
	}

	inline bool shift() const
	{
		return _shift;
	}

	inline void setCtrl(bool b)
	{
		_ctrl = b;
	}

	inline bool ctrl() const
	{
		return _ctrl;
	}


	std::string toUTF8String() const
	{
		Pt::Char unicodeChar(_unicode); 
		std::stringstream ss;		
		Pt::TextStream stream(ss, new Pt::Utf8Codec());
		stream<<unicodeChar;
		stream.flush();
		return ss.str();
	}

	std::string shortCutKey() const
	{
		std::string shortKey = "";
		
		if( _ctrl)
			shortKey += "C//";

		if( _alt)
			shortKey += "A//";

		if(_shift)
			shortKey += "S//";  

		shortKey += toUTF8String();
		return shortKey; 			
	}

private:
  Pt::Char _unicode;
	bool _alt;
	bool _shift;
	bool _ctrl;
	KeyState _state;
};

}}

#endif

