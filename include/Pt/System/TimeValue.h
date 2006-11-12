#ifndef PT_SYSTEM_TIMEVALUE_H
#define PT_SYSTEM_TIMEVALUE_H

#include <Pt/Api.h>
#include <Pt/Types.h>


namespace Pt 
{

namespace System 
{

//! @brief TimeValue
/**
*   The TimeValue class is a container for a time value devided in seconds 
*   and mili seconds part. To obtain the concrete time value you need to added
*   the mili seconds part to the seconds part.
*/
class PT_API TimeValue
{
public:        
    //! @brief Default constructor
    /**
        @param seconds The seconds part to set.    
        @param microSec The microseconds part to set.
    */
	TimeValue( size_t seconds = 0, size_t microSeconds = 0 );
	
	
	//! @brief Copy constructor.
    /**
        @param ref The time value to copy.
    */
	TimeValue( const TimeValue& ref );
	
	//! @brief Destructor.
	~TimeValue();

    //! @brief Subtract a TimeValue.
    /**
        @param from Value to subtract.
    */    
	TimeValue operator-( TimeValue& from );
	
	//! @brief Reduce the time value.
    /**
        @param from Value to reduce.
    */    	
	TimeValue operator-=( TimeValue& from );
	
	//! @brief Add the time value.
    /**
        @param with Value to add.
    */    	
	TimeValue operator+( TimeValue& with );
	
	//! @brief Increase the time value.
    /**
        @param with Value to increase.
    */    	
	TimeValue operator+=( TimeValue& with );
	
	//! @brief Compare of egality.
    /**
        @param with Value to compare.
    */    		
	bool operator==( TimeValue& with );
	
	//! @brief Compare of lesser.
    /**
        @param with Value to compare.
    */    		
	bool operator<( TimeValue& with );
	
	//! @brief Compare of greater.
    /**
        @param with Value to compare.
    */    		
	bool operator>( TimeValue& with );

	//! @brief Sets the micro seconds part of the TimeValue.
    /**
        @param microSec The micro seconds part to set.
    */    		
    inline void setMicroSeconds( size_t microSec )
    {
        _microSeconds = microSec;
    }
    
    //! @brief Return the microseconds part of the TimeValue.
    /**
        @return Return the micro seconds part.
    */    
    inline size_t microSeconds() const 
    {
        return _microSeconds;
    }  
    
    //! @brief Sets the seconds part of the TimeValue.
    /**
        @param seconds The seconds part to set.
    */        
    inline void setSeconds( size_t seconds )
    {
        _seconds = seconds;
    }
    
    //! @brief Return the second part of the TimeValue.
    /**
        @return Return the micro seconds part.
    */   
    inline size_t seconds() const
    {
        return _seconds;
    }      

protected:
	size_t _seconds;
	size_t _microSeconds;
};


} //namespace Pt

} //namespace System


#endif // PT_TimeValue_H

