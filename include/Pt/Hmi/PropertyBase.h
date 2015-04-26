#ifndef Pt_Hmi_Base_PropertyBase_h
#define Pt_Hmi_Base_PropertyBase_h

#include <Pt/Hmi/Api.h>
#include <Pt/Signal.h>
#include <string>

namespace Pt {
namespace Hmi {
    
class PropertyBase
{
	public:
		PropertyBase(const char* name);

		virtual ~PropertyBase()
		{
		}
        
		inline const std::string& name() const
		{
				return _name;
		}

	  virtual Pt::Any getValue() const = 0;

	  virtual void setValue(const Pt::Any& value, bool notify = true ) = 0;
		
	private:
		 std::string _name;
};

}}
#endif

