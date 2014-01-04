#ifndef Pt_Hmi_Base_PropertyBase_h
#define Pt_Hmi_Base_PropertyBase_h

#include <Pt/Signal.h>
#include <string>

namespace Pt {
namespace Hmi {

#define DefinePropertyInitMacro(prop,value) prop(#prop,me(),value)
#define DefinePropertyDefaultMacro(prop) prop(#prop,me())

class Model;
    
class PT_HMI_API PropertyBase
{
public:
	PropertyBase(const char* name, Model* parent);
    
    inline const Model* parent() const
    {
        return _parent;
    }
    
    inline Model* parent()
    {
        return _parent;
    }
    
    inline const std::string& name() const
    {
        return _name;
    }

protected:
    void changed();
    
private:
	Model* _parent;
    std::string _name;
};

}}
#endif

