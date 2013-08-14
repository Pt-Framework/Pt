#include "CursorImpl.h"
#include <Pt/Hmi/Api.h>
#include <Pt/Hmi/Application.h>
#include "ApplicationImpl.h"

namespace Pt{
namespace Hmi{

CursorImpl::CursorImpl()
{
	setCursor(Cursors::Default);
}

CursorImpl::~CursorImpl()
{
}

void CursorImpl::setCursor(Cursors::Type c)
{
	_type = c;
//TODO: set the system cursor
}

Cursors::Type CursorImpl::getCursor() const
{
	return _type;
}


}}
