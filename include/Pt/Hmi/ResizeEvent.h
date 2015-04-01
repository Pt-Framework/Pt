#ifndef Pt_Hmi_ResizeEvent_h
#define Pt_Hmi_ResizeEvent_h

#include <Pt/Hmi/Api.h>
#include <Pt/Hmi/WindowProperties.h>
#include <Pt/Event.h>
#include <Pt/Gfx/Size.h>

namespace Pt{
namespace Hmi{

class PT_HMI_API ResizeEvent : public Pt::BasicEvent<ResizeEvent>
{
public:	
	ResizeEvent()
	{
	}

	virtual ~ResizeEvent()
	{
	}

  WindowState::Type state() const
  {
    return _state;
  }

	void setState(WindowState::Type s)
	{
		_state = s;
	}
	
	void setSize(const Pt::Gfx::SizeF& s)
	{
		_size = s;
	}

	const Pt::Gfx::SizeF& size() const
	{
		return _size;
	}

private:
	Pt::Gfx::SizeF _size;
  WindowState::Type _state;	
};

}}

#endif

