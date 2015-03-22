#ifndef Pt_Hmi_Base_PropertyBase_h
#define Pt_Hmi_Base_PropertyBase_h

#include <Pt/Hmi/Api.h>
#include <Pt/Signal.h>
#include <string>

#define PT_HMI_INIT_PROPERTY_VALUE(prop, value) prop(#prop, value)
#define PT_HMI_INIT_PROPERTY(prop) prop(#prop)

namespace Pt {
namespace Hmi {
    
class PT_HMI_API PropertyBase
{
public:
	PropertyBase(const char* name);
        
  inline const std::string& name() const
  {
      return _name;
  }

private:
   std::string _name;
};

}}
#endif

