#include "CursorImpl.h"
#include <Pt/Hmi/Api.h>
#include <Windows.h>
#include <Pt/Hmi/Application.h>
#include "ApplicationImpl.h"
#include <Pt/Hmi/WindowController.h>

namespace Pt{
namespace Hmi{

CursorImpl::CursorImpl()
{	
}

CursorImpl::~CursorImpl()
{
}

void CursorImpl::setCursor(Cursors::Type c, WindowController* parent)
{
	_type = c;
	HINSTANCE hin = NULL; //System Instance

	_cursor = LoadCursor(hin, IDC_ARROW);

	switch(_type)
	{
		default:			
		break;
		case Cursors::Arrow:		
			_cursor = LoadCursor(hin, IDC_ARROW);
		break;
				
		case Cursors::Cross:
			_cursor = LoadCursor(hin, IDC_CROSS);
		break;

		case Cursors::Hand:
			_cursor = LoadCursor(hin, IDC_HAND);
		break;
		
		case Cursors::IBeam:
			_cursor = LoadCursor(hin, IDC_IBEAM);
		break;

		case Cursors::SizeNS:
			_cursor = LoadCursor(hin, IDC_SIZENS);
		break;		
		
		case Cursors::SizeWE:
			_cursor = LoadCursor(hin, IDC_SIZEWE);
		break;
	}

	SetCursor(_cursor);
}

Cursors::Type CursorImpl::getCursor() const
{
	return _type;
}


}}
