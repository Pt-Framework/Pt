#ifndef Pt_Hmi_KeyEvent_h
#define Pt_Hmi_KeyEvent_h

#include <Pt/Hmi/Api.h>
#include <Pt/Event.h>
#include <Pt/Allocator.h>
#include <cstddef>
#include <string>

namespace Pt{
namespace Hmi{

class PT_HMI_API KeyEvent : public Pt::Event
{
public:

enum KeyState
{
	KeyNone,
	KeyDown,
	KeyUp
};

public:	
	KeyEvent();
	KeyEvent(const KeyEvent& copy);
	virtual ~KeyEvent();

	bool operator==(const KeyEvent& cmp)
	{
		return false;
	}
	
	bool operator!=(const KeyEvent& cmp)
	{
		return true;
	}
	
	void operator=(const KeyEvent& copy);

	inline int virtualCode() const
	{
		return _virtualCode;
	}

	inline void setVirtualCode(int c)
	{
		_virtualCode = c;
	}

	inline int repeatCount() const
	{
		return _repeatCount;
	}
	
	inline void setRepeatCount(int c)
	{
		_repeatCount = c;
	}

	inline bool extCode() const
	{
		return _extCode;
	}

	inline void setExtCode(bool c)
	{
		_extCode = c;
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

	inline std::string shortCutKey() const
	{
		std::string shortCutKey = "";
		
		if( _ctrl)
			shortCutKey += "CTRL//";

		if( _alt)
			shortCutKey += "ALT//";

		if(_shift)
			shortCutKey += "SHIFT//";  


		char value =  (char) _virtualCode;
		shortCutKey += value;

		return shortCutKey; 			
	}

protected:
    virtual Pt::Event& onClone(Pt::Allocator& allocator) const;
    virtual void onDestroy(Pt::Allocator& allocator);
    virtual const std::type_info& onTypeInfo() const;

private:
	int _virtualCode;
	int _repeatCount;	
	bool _extCode;
	bool _alt;
	bool _shift;
	bool _ctrl;
	KeyState _state;
};

}}

#endif

