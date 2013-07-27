#include <Pt/Hmi/Event2D.h>
#include <new>

namespace Pt {
namespace Hmi {

Event2D::Event2D()
: _x(0)
, _y(0)
{
}

Event2D::Event2D(const Event2D& copy)
: _x(copy._x)
, _y(copy._y)
, _buttons(copy._buttons)
, _controlDial(copy._controlDial)
{
}

Event2D::~Event2D()
{
}

Pt::Event& Event2D::onClone(Pt::Allocator& allocator) const
{
    void* buffer = allocator.allocate(sizeof(Event2D));
	Event2D* ev = new (buffer) Event2D(*this);	
    return *ev;
}

void Event2D::onDestroy(Pt::Allocator& allocator)
{
	allocator.deallocate(this, sizeof(Event2D));
}

const std::type_info& Event2D::onTypeInfo() const
{
    static const std::type_info& ti = typeid(Event2D);
    return ti;
}

void Event2D::operator=(const Event2D& copy)
{
	 _x = copy._x;
	 _y = copy._y;
	 _buttons = copy._buttons;
	 _controlDial = copy._controlDial;
}

}}
