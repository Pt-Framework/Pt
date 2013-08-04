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

	inline int scanCode() const
	{
		return _scanCode;
	}

	inline void setScancode(int c)
	{
		_scanCode = c;
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

protected:
    virtual Pt::Event& onClone(Pt::Allocator& allocator) const;
    virtual void onDestroy(Pt::Allocator& allocator);
    virtual const std::type_info& onTypeInfo() const;

private:
	int _virtualCode;
	int _repeatCount;
	int _scanCode;
	bool _extCode;
	bool _alt;
	KeyState _state;
};

}}

#endif

