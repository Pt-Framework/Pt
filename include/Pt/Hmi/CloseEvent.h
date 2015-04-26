#ifndef Pt_Hmi_CloseEvent_h
#define Pt_Hmi_CloseEvent_h

#include <Pt/Types.h>
#include <Pt/Hmi/Api.h>
#include <Pt/Event.h>

namespace Pt{
namespace Hmi{

class PT_HMI_API CloseEvent : public Pt::BasicEvent<CloseEvent>
{
	public:	
		CloseEvent()
		{
		}

		virtual ~CloseEvent()
		{
		}

};

}}

#endif

