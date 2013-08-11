#include "GfxOutputImpl.h"
#include <Pt/Hmi/GfxModel.h>
#include <Pt/Hmi/Application.h>
#include "ApplicationImpl.h"
#include <Pt/Gfx/Rgb888Color.h>
#include <Pt/Gfx/Rgb888Image.h>
#include <Pt/Hmi/WindowModel.h>
#include <Pt/Hmi/WindowController.h>

namespace Pt{
namespace Hmi{

GfxOutputImpl::GfxOutputImpl()
: _model(0)
, _ignoreSizeEvent(false)
{
	create();
}


void GfxOutputImpl::create()
{
	std::clog <<"create"<<std::endl;
    _window = nil;
    _view = [[WidgetView alloc] init ];

	Gfx::Point at(20, 20);
	Gfx::Size size(400, 200);

	_window = [[NSWindow alloc] initWithContentRect:NSMakeRect(at.x(), at.y(), size.width(), size.height())
													styleMask:NSTitledWindowMask |
															  NSClosableWindowMask |
															  NSMiniaturizableWindowMask |
															  NSResizableWindowMask
													 backing:NSBackingStoreBuffered
													 defer:NO];
	[_window setReleasedWhenClosed: NO];
	[_window setAcceptsMouseMovedEvents:YES];
	[_window setContentView: _view];
	[_window setDelegate: _view];
	
    [_window makeKeyAndOrderFront:nil];
    [_view setHidden:NO];
}

GfxOutputImpl::~GfxOutputImpl()
{
	destroy();
}


void GfxOutputImpl::destroy()
{
    [_window release];
    [_view release];
}

void GfxOutputImpl::output(Pt::Hmi::Model* model)
{
	WindowModel* wmodel = dynamic_cast<WindowModel*>(model);

	_model = wmodel;

	if( wmodel == 0)
		throw std::logic_error("ERROR: WindowModel model expected!");
/*
	if(wmodel->Closed.get())
	{
		if(_hwnd != 0)
			destroy();
	}
	else
	{
		if(_hwnd == 0)
			create();
	}

	_ignoreSizeEvent = true;	

	writeWindowSizeAndPos();
	writeWindowProperties();
	
	_ignoreSizeEvent = false;

	InvalidateRect(_hwnd, 0, FALSE);*/
}


Pt::Gfx::Painter* GfxOutputImpl::nativePainter()
{
	return _nativePainter;
}
}}
