#include <Pt/System/Input2DEvent.h>
#include <new>

namespace Pt {
namespace System {

using namespace std;

const type_info& Input2DEvent::TYPE_INFO = typeid(Input2DEvent);

Input2DEvent::Input2DEvent(int button, Action action, int modifier, double x, double y)
: _button(button)
, _action(action)
, _modifier(modifier)
, _x(x)
, _y(y)
{
}

Input2DEvent::Input2DEvent(const Input2DEvent& copy)
: _button(copy._button)
, _action(copy._action)
, _modifier(copy._modifier)
, _x(copy._x)
, _y(copy._y)
{
}
Input2DEvent::~Input2DEvent()
{

}


Pt::Event& Input2DEvent::onClone(Pt::Allocator& allocator) const
{
    void* buffer = allocator.allocate(sizeof(Input2DEvent));
	Input2DEvent* ev = new (buffer) Input2DEvent(*this);	
    return *ev;
}

void Input2DEvent::onDestroy(Pt::Allocator& allocator)
{
	allocator.deallocate(this, sizeof(Input2DEvent));
}

const std::type_info& Input2DEvent::onTypeInfo() const
{
    static const std::type_info& ti = typeid(Input2DEvent);
    return ti;
}

}}
