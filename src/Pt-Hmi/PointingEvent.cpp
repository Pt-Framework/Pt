#include <Pt/Hmi/PointingEvent.h>
#include <new>

namespace Pt {
namespace Hmi {

PointingEvent::PointingEvent()
: _x(0)
, _y(0)
{
}

PointingEvent::PointingEvent(const PointingEvent& copy)
: _x(copy._x)
, _y(copy._y)
, _buttons(copy._buttons)
, _controlDial(copy._controlDial)
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

void PointingEvent::operator=(const PointingEvent& copy)
{
	 _x = copy._x;
	 _y = copy._y;
	 _buttons = copy._buttons;
	 _controlDial = copy._controlDial;
}

}}
