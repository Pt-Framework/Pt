#include "CursorImpl.h"
#include <Pt/Hmi/Api.h>
#include <Windows.h>
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
	HINSTANCE hin = NULL; //System Instance

	switch(_type)
	{
		default:
			_cursor = LoadCursor(hin, IDC_ARROW);
		break;
		
		case Cursors::AppStarting:
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
		
		case Cursors::No:
			_cursor = LoadCursor(hin, IDC_NO);
		break;

		case Cursors::SizeAll:
			_cursor = LoadCursor(hin, IDC_SIZEALL);
		break;

		case Cursors::SizeNESW:
			_cursor = LoadCursor(hin, IDC_SIZENESW);
		break;
		
		case Cursors::SizeNS:
			_cursor = LoadCursor(hin, IDC_SIZENS);
		break;
		
		case Cursors::SizeNWSE:
			_cursor = LoadCursor(hin, IDC_SIZENWSE);
		break;
		
		case Cursors::SizeWE:
			_cursor = LoadCursor(hin, IDC_SIZEWE);
		break;

		case Cursors::UpArrow:
			_cursor = LoadCursor(hin, IDC_UPARROW);
		break;

		case Cursors::Wait:
			_cursor = LoadCursor(hin, IDC_WAIT);
		break;
	}

	SetCursor(_cursor);
}

Cursors::Type CursorImpl::getCursor() const
{
	return _type;
}


}}
