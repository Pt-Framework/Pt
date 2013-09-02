#include "CursorImpl.h"
#include <Pt/Hmi/Api.h>
#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/GfxOutput.h>
#include <Pt/Hmi/WindowController.h>
#include "GfxOutputImpl.h"
#include "ApplicationImpl.h"

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
	Display* display = Application::instance().impl()->display();
	unsigned int shape = 0;


	switch(_type)
	{
		default:
			shape = XC_top_left_arrow;
		break;	
	
		case Cursors::Arrow:		
			shape = XC_top_left_arrow;
		break;
			
		case Cursors::Cross:
			shape = XC_crosshair;
		break;

		case Cursors::Hand:
			shape = XC_hand2;	
		break;
	
		case Cursors::IBeam:
			shape = XC_xterm;
		break;
	
		case Cursors::SizeNS:
			shape = XC_sb_v_double_arrow;
		break;
		
		case Cursors::SizeWE:
			shape = XC_sb_h_double_arrow;
		break;

	}

	_cursorId = XCreateFontCursor(display, shape);

	for(size_t i = 0; i < parent->outputDevices().size(); ++i)
	{
		GfxOutput* outDev= dynamic_cast<GfxOutput*>(parent->outputDevices()[i]);

		if( outDev == 0)
			continue;

		GfxOutputImpl* impl = outDev->impl();
		::Window w = impl->window();
	
		if( w != 0)
		{
			XUndefineCursor(display,w);
			XDefineCursor(display, w,	_cursorId);	
		}
	}


}

Cursors::Type CursorImpl::getCursor() const
{
	return _type;
}


}}
