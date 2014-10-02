#include <Pt/Hmi/Model.h>
#include <Pt/Hmi/PropertyBase.h>

namespace Pt {
namespace Hmi {
        
PropertyBase::PropertyBase(const char* name)
: _model(0)
, _name(name)
{
    
}
    
void PropertyBase::updateModel()
{
	if(_model != 0)
		_model->changed().send(*model());
}

}}