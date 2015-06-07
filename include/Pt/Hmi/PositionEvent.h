#ifndef Pt_Hmi_PositionEvent_h
#define Pt_Hmi_PositionEvent_h

#include <Pt/Types.h>
#include <Pt/Hmi/Api.h>
#include <Pt/Event.h>
#include <Pt/Ui/Point.h>


namespace Pt{
namespace Hmi{

class PT_HMI_API PositionEvent : public Pt::BasicEvent<PositionEvent>
{
	public:	
		PositionEvent(const Ui::PointF& pos)
		: _position( pos )		
		{
		}


		PositionEvent()
		{
		}


		virtual ~PositionEvent()
		{
		}


		void setPosition( const Ui::PointF&  pos )
		{
			_position = pos;
		}


		const Ui::PointF&  position( ) const
		{
			return _position;
		}	

	private:
		Ui::PointF _position;
};

}}

#endif

