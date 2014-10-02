#ifndef Pt_Hmi_Base_PropertyBase_h
#define Pt_Hmi_Base_PropertyBase_h

#include <Pt/Signal.h>
#include <string>

#define PT_HMI_INIT_PROPERTY_VALUE(prop, value) prop(#prop, value)
#define PT_HMI_INIT_PROPERTY(prop) prop(#prop)

namespace Pt {
namespace Hmi {

class Model;
    
class PT_HMI_API PropertyBase
{
public:
	PropertyBase(const char* name);
    
    inline const Model* model() const
    {
        return _model;
    }
    
    inline Model* model()
    {
        return _model;
    }

	inline void setModel(Model* model)
    {
        _model = model;
    }
    
    inline const std::string& name() const
    {
        return _name;
    }

protected:
    void updateModel();
    
private:
	Model* _model;
    std::string _name;
};

}}
#endif

