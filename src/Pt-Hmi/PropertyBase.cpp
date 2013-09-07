#include <Pt/Hmi/Model.h>
#include <Pt/Hmi/PropertyBase.h>

namespace Pt {
namespace Hmi {
        
PropertyBase::PropertyBase(const char* name, Model* parent)
: _parent(parent)
, _name(name)
{
    _parent->registerProperty(this);
}
    
    
void PropertyBase::changed()
{
    _parent->Changed.send(this);
    
}

}}