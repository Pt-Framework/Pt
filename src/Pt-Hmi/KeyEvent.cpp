#include <Pt/Hmi/KeyEvent.h>
#include <Pt/TextStream.h>
#include <Pt/Utf8Codec.h>
#include <Pt/String.h>
#include <sstream>
#include <Pt/Hmi/Controller.h>
#include <new>

namespace Pt {
namespace Hmi {

KeyEvent::KeyEvent(Controller* controller)
: Event(controller)
, _unicode(-1)
, _state(KeyNone)
, _alt(false)
, _shift(false)
, _ctrl(false)
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

std::string KeyEvent::toUTF8String() const
{
	Pt::Char unicodeChar(_unicode); 
	std::stringstream ss;		
	Pt::TextStream stream(ss, new Pt::Utf8Codec());
	stream<<unicodeChar;
	stream.terminate();
	return ss.str();
}

std::string KeyEvent::shortCutKey() const
{
	std::string shortKey = "";
		
	if( _ctrl)
		shortKey += "C//";

	if( _alt)
		shortKey += "A//";

	if(_shift)
		shortKey += "S//";  

	shortKey += toUTF8String();
	return shortKey; 			
}

}}
