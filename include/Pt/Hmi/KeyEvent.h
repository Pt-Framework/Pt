#ifndef Pt_Hmi_KeyEvent_h
#define Pt_Hmi_KeyEvent_h

#include <Pt/Hmi/Api.h>
#include <Pt/Event.h>
#include <Pt/Allocator.h>
#include <Pt/String.h>
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

	/**@brief Return the translate key to unicode UTF8 encoding.
	* 
	* @return The translate key to unicode UTF8 encoding,*/

	std::string toUTF8String() const;

	/**@brief Return the translate key to UTF8 encoding short cut command.
	* 
	* @return The translate key to UTF8 encoding short cut command.*/
	std::string shortCutKey() const;

protected:
    virtual Pt::Event& onClone(Pt::Allocator& allocator) const;
    virtual void onDestroy(Pt::Allocator& allocator);
    virtual const std::type_info& onTypeInfo() const;

private:
    Pt::Char _unicode;
	bool _alt;
	bool _shift;
	bool _ctrl;
	KeyState _state;
};

}}

#endif

