#ifndef Pt_Hmi_ResizeEvent_h
#define Pt_Hmi_ResizeEvent_h

#include <Pt/Hmi/Api.h>
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
};

}}

#endif

