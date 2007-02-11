#include "Pt/System/TimeValue.h"
#include "Pt/Exception.h"


namespace Pt {

namespace System {

TimeValue::TimeValue( size_t seconds , size_t microSeconds )
: _seconds( seconds )
, _microSeconds( microSeconds )
{
}

TimeValue::TimeValue( const TimeValue& value )
{
   _seconds         = value._seconds;
   _microSeconds    = value._microSeconds;
}

TimeValue::~TimeValue()
{
}

TimeValue TimeValue::operator-( const TimeValue& from )
{
    if( _seconds < from._seconds)
        throw std::logic_error("Time to subtract is lesser!" + PT_SOURCEINFO);
    
    if( ( _seconds == from._seconds ) && ( _microSeconds < from._microSeconds ) )
        throw std::logic_error("Time to subtract is lesser!" + PT_SOURCEINFO);
    
    TimeValue diff;
     
    diff._seconds = _seconds - from._seconds;
     
    if( _microSeconds < from._microSeconds )
    {
        diff._seconds--;                
        diff._microSeconds =  1000000 + ( _microSeconds - from._microSeconds );        
    }
    else
    {
        diff._microSeconds =  _microSeconds - from._microSeconds ;        
    }
        
    return diff;
}

TimeValue TimeValue::operator+( const TimeValue& with )
{
    TimeValue sum;    
    if( _microSeconds + with._microSeconds > 1000000)
    {
        sum._seconds      = _seconds + with._seconds + ( _microSeconds + with._microSeconds ) % 1000000;    
           sum._microSeconds = ( _microSeconds + with._microSeconds ) / 1000000;
    }
    else
    {
        sum._seconds      = _seconds + with._seconds;    
        sum._microSeconds = _microSeconds + with._microSeconds;
    }       
    
    return sum;
}

bool TimeValue::operator==( TimeValue& with )
{
    return ( ( with._seconds == _seconds ) &&  (with._microSeconds == _microSeconds) );
}

bool TimeValue::operator<( TimeValue& with)
{    
    return (  ( _seconds < with._seconds) || ( (_seconds == with._seconds) && _microSeconds < with._microSeconds));
}

bool TimeValue::operator>( TimeValue& with )
{
    return ( ( _seconds > with._seconds ) || ( ( _seconds == with._seconds ) && ( _microSeconds > with._microSeconds ) ) );
}

TimeValue TimeValue::operator+=( const TimeValue& with )
{
    *this = *this + with;
    return *this;   
}

TimeValue TimeValue::operator-=( const TimeValue& with )
{
    *this = *this - with;
    return *this;
}

}

}
