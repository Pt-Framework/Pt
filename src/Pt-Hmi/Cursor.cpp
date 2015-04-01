#include <Pt/Hmi/Cursor.h>
#include "CursorImpl.h"

namespace Pt{
namespace Hmi{

Cursor::Cursor()
: _impl(new CursorImpl())
{
}

Cursor::~Cursor()
{
	delete _impl;
}

void Cursor::setCursor(Cursors::Type c)
{
	_impl->setCursor(c);
}

Cursors::Type Cursor::getCursor() const
{
	return (Cursors::Type) _impl->getCursor();
}

}}
