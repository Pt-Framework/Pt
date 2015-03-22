#ifndef Pt_Hmi_PositionEvent_h
#define Pt_Hmi_PositionEvent_h

#include <Pt/Hmi/Api.h>
#include <Pt/Event.h>
#include <Pt/Gfx/Point.h>

namespace Pt{
namespace Hmi{

class PT_HMI_API PositionEvent : public Pt::BasicEvent<PositionEvent>
{
public:	
	PositionEvent()
	{
	}

	virtual ~PositionEvent()
	{
	}

	void setPosition(const Pt::Gfx::PointF& p)
	{
		_position = p;
	}

	const Pt::Gfx::PointF& position() const 
	{
		return _position;
	}


private:
	Pt::Gfx::PointF _position;   
};

}}

#endif

