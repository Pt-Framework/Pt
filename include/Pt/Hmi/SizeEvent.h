#ifndef Pt_Hmi_SizeEvent_h
#define Pt_Hmi_SizeEvent_h

#include <Pt/Types.h>
#include <Pt/Hmi/Api.h>
#include <Pt/Event.h>
#include <Pt/Hmi/WindowProperties.h>
#include <Pt/Gfx/Size.h>


namespace Pt{
namespace Hmi{

class PT_HMI_API SizeEvent : public Pt::BasicEvent<SizeEvent>
{
	public:	
		SizeEvent(const Pt::Gfx::SizeF&  size, const WindowState::Type&  state)
		: _size( size )
		, _state( state )
		{
		}


		SizeEvent()
		{
		}


		virtual ~SizeEvent()
		{
		}


		void setSize( const Pt::Gfx::SizeF&  size )
		{
			_size = size;
		}


		const Pt::Gfx::SizeF&  size( ) const
		{
			return _size;
		}


		void setState( const WindowState::Type&  state )
		{
			_state = state;
		}


		const WindowState::Type& state( ) const
		{
			return _state;
		}

	private:
		Pt::Gfx::SizeF _size;
		WindowState::Type _state;
};

}}

#endif

