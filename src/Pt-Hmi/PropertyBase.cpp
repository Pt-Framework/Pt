#include <Pt/Hmi/Model.h>
#include <Pt/Hmi/PropertyBase.h>

namespace Pt {
namespace Hmi {
        
PropertyBase::PropertyBase(const char* name)
: _parent(0)
, _name(name)
{
    
}
    
    
void PropertyBase::changed()
{
	if(_parent != 0)
		_parent->Changed.send(this);
    
}

}}