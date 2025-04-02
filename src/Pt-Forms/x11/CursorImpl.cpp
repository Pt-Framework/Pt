#include "CursorImpl.h"
// #include <Pt/Forms/Api.h>
// #include <Pt/Forms/Application.h>
// #include "ApplicationImpl.h"

namespace Pt{
namespace Forms{

// CursorImpl::CursorImpl()
// {
// }

// CursorImpl::~CursorImpl()
// {
// }

// void CursorImpl::setCursor(Cursors::Type c)
// {    
// 	_type = c;
// 	unsigned int shape = 0;

// 	switch(_type)
// 	{
// 		default:
// 			shape = XC_top_left_arrow;
// 		break;	
	
// 		case Cursors::Arrow:
// 			shape = XC_top_left_arrow;
// 		break;
			
// 		case Cursors::Cross:
// 			shape = XC_crosshair;
// 		break;

// 		case Cursors::Hand:
// 			shape = XC_hand2;
// 		break;
	
// 		case Cursors::IBeam:
// 			shape = XC_xterm;
// 		break;
	
// 		case Cursors::SizeNS:
// 			shape = XC_sb_v_double_arrow;
// 		break;
		
// 		case Cursors::SizeWE:
// 			shape = XC_sb_h_double_arrow;
// 		break;
// 	}

//     Display* display = Application::instance().impl()->display();
//     _cursorId = XCreateFontCursor(display, shape);
//     ::Window w = XDefaultRootWindow(display);    
//     XDefineCursor(display, w, _cursorId);
// }

// Cursors::Type CursorImpl::getCursor() const
// {
// 	return _type;
// }


}}
