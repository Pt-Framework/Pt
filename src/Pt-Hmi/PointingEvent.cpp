#include <Pt/Hmi/PointingEvent.h>
#include <Pt/Allocator.h>
#include <new>

namespace Pt {
namespace Hmi {

PointingEvent::PointingEvent(Controller* controller)
: Event(controller)
, _x(0)
, _y(0)
{
}

PointingEvent::~PointingEvent()
{
}

Pt::Event& PointingEvent::onClone(Pt::Allocator& allocator) const
{
    void* buffer = allocator.allocate(sizeof(PointingEvent));
	PointingEvent* ev = new (buffer) PointingEvent(*this);	
    return *ev;
}

void PointingEvent::onDestroy(Pt::Allocator& allocator)
{
	allocator.deallocate(this, sizeof(PointingEvent));
}

const std::type_info& PointingEvent::onTypeInfo() const
{
    static const std::type_info& ti = typeid(PointingEvent);
    return ti;
}

}}
