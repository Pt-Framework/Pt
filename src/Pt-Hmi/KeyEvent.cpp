#include <Pt/Hmi/KeyEvent.h>
#include <new>

namespace Pt {
namespace Hmi {

KeyEvent::KeyEvent()
: _virtualCode(-1)
, _repeatCount(0)
, _scanCode(-1)
, _extCode(false)
, _state(KeyNone)
, _alt(false)
{
}

KeyEvent::KeyEvent(const KeyEvent& copy)
: _virtualCode(copy._virtualCode)
, _repeatCount(copy._repeatCount)
, _scanCode(copy._scanCode)
, _extCode(copy._extCode)
, _state(copy._state)
, _alt(copy._alt)
{
}

KeyEvent::~KeyEvent()
{
}

Pt::Event& KeyEvent::onClone(Pt::Allocator& allocator) const
{
    void* buffer = allocator.allocate(sizeof(KeyEvent));
	KeyEvent* ev = new (buffer) KeyEvent(*this);	
    return *ev;
}

void KeyEvent::onDestroy(Pt::Allocator& allocator)
{
	allocator.deallocate(this, sizeof(KeyEvent));
}

const std::type_info& KeyEvent::onTypeInfo() const
{
    static const std::type_info& ti = typeid(KeyEvent);
    return ti;
}

void KeyEvent::operator=(const KeyEvent& copy)
{
	_virtualCode = copy._virtualCode;
	_repeatCount = copy._repeatCount;
	_scanCode = copy._scanCode;
	_extCode = copy._extCode;
	_state = copy._state;
	_alt = copy._alt;
}

}}
