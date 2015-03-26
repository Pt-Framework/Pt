#ifndef Pt_Hmi_ActivateEvent_h
#define Pt_Hmi_ActivateEvent_h

#include <Pt/Hmi/Api.h>
#include <Pt/Event.h>

namespace Pt{
namespace Hmi{

class PT_HMI_API ActivateEvent : public Pt::BasicEvent<ActivateEvent>
{
public:	
	ActivateEvent()
	: _active(false)
	{
	}

	virtual ~ActivateEvent()
	{
	}

  bool active() const
  {
    return _active;
  }

	void setActive(bool a)
	{
		_active = a;
	}
	
	
private:
	bool _active;
	
};

}}

#endif

