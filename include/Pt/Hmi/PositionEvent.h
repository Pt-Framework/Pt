#ifndef Pt_Hmi_PositionEvent_h
#define Pt_Hmi_PositionEvent_h

#include <Pt/Types.h>
#include <Pt/Hmi/Api.h>
#include <Pt/Event.h>
#include <Pt/Gfx/Point.h>


namespace Pt{
namespace Hmi{

class PT_HMI_API PositionEvent : public Pt::BasicEvent<PositionEvent>
{
	public:	
		PositionEvent(const Pt::Gfx::PointF& pos)
		: _position( pos )		
		{
		}


		PositionEvent()
		{
		}


		virtual ~PositionEvent()
		{
		}


		void setPosition( const Pt::Gfx::PointF&  pos )
		{
			_position = pos;
		}


		const Pt::Gfx::PointF&  position( ) const
		{
			return _position;
		}	

	private:
		Pt::Gfx::PointF _position;
};

}}

#endif

