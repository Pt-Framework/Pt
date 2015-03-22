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
		stream.terminate();
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

