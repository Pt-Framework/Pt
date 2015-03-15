#include "CursorImpl.h"
#include <Pt/Hmi/Api.h>
#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/View.h>
#include <Pt/Hmi/Window.h>
#include "ViewImpl.h"
#include "ApplicationImpl.h"

namespace Pt{
namespace Hmi{

CursorImpl::CursorImpl()
{
}

CursorImpl::~CursorImpl()
{
}

void CursorImpl::setCursor(Cursors::Type c)
{    
	_type = c;

}

Cursors::Type CursorImpl::getCursor() const
{
	return _type;
}


}}
