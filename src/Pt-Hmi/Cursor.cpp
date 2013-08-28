#include <Pt/Hmi/Cursor.h>
#include "CursorImpl.h"
#include <Pt/Hmi/WindowController.h>

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

void Cursor::setCursor(Cursors::Type c, WindowController* parent)
{
	_impl->setCursor(c, parent);
}

Cursors::Type Cursor::getCursor() const
{
	return _impl->getCursor();
}

}}
