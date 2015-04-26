#ifndef Pt_Hmi_FocusEvent_h
#define Pt_Hmi_FocusEvent_h

#include <Pt/Types.h>
#include <Pt/Hmi/Api.h>
#include <Pt/Event.h>


namespace Pt{
namespace Hmi{

class PT_HMI_API FocusEvent : public Pt::BasicEvent<FocusEvent>
{
	public:	
		FocusEvent(bool f)		
		{
		}


		FocusEvent()
		{
		}


		virtual ~FocusEvent()
		{
		}


		void setFocus( bool f )
		{
			_isFocussed = f;
		}


		bool isFocussed( ) const
		{
			return _isFocussed;
		}	

	private:
		bool _isFocussed;
};

}}

#endif

