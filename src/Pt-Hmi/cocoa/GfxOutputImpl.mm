#include "GfxOutputImpl.h"
#include <Pt/Hmi/GfxModel.h>
#include <Pt/Hmi/Application.h>
#include "ApplicationImpl.h"
#include <Pt/Gfx/Rgb888Color.h>
#include <Pt/Gfx/Rgb888Image.h>
#include <Pt/Hmi/WindowModel.h>
#include <Pt/Hmi/WindowController.h>
#include "WidgetView.h"
#include "Window.h"

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
    _window = nil;
    _view = [[WidgetView alloc] init: this ];

	Gfx::PointF at(20, 20);
	Gfx::SizeF size(400, 200);

	_window = [[Window alloc] initWithContentRect:NSMakeRect(at.x(), at.y(), size.width(), size.height()) styleMask:NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask | NSResizableWindowMask backing:NSBackingStoreBuffered defer:NO];
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
	_view = nil;
}

void GfxOutputImpl::output(Pt::Hmi::Model* model)
{
	_model = dynamic_cast<WindowModel*>(model);

    if( _model == 0)
		throw std::logic_error("ERROR: WindowModel model expected!");

    NSRect viewRect = [_view frame];
    
    //Initial size and position
    if(_model->Size.get().width() != viewRect.size.width && _model->Size.get().height() != viewRect.size.height)
    {
        writeWindowSizeAndPos();
        return;
    }

    //Create/Destroy handling
	if(_model->Closed.get())
	{
		if(_view != nil)
		{
			destroy();
		}

		return;
	}
	else
	{
		if(_view == nil)
		{
			if(_model->Visible.get())
				create();
			else
				return;
		}
	}
    
    //Size and position handling
    _ignoreSizeEvent = true;
    writeWindowSizeAndPos();
    _ignoreSizeEvent = false;
    
    //Redraw
    [_view setNeedsDisplay:YES];
}
    
void GfxOutputImpl::writeWindowSizeAndPos()
{
    int screenHeight = [[NSScreen mainScreen] frame].size.height;
    NSRect windowRect =  [_window frame];
   
    //Position
    windowRect.origin.x = _model->WinPos.get().x();
    windowRect.origin.y = screenHeight - (_model->WinPos.get().y() + _model->WinSize.get().height());
    
    //Size
    windowRect.size.width = _model->WinSize.get().width();
    windowRect.size.height = _model->WinSize.get().height();
    
    [_window setFrame:windowRect display:YES animate:NO];
    
}
    
void GfxOutputImpl::onPositionAndSize()
{
    if(_ignoreSizeEvent)
        return;
    
    if( _model == 0)
        return;
    
    int screenHeight = [[NSScreen mainScreen] frame].size.height;
    
    //Window
    NSRect windowRect = [_window frame];
    _model->WinPos = Pt::Gfx::PointF(windowRect.origin.x,screenHeight - (windowRect.origin.y + windowRect.size.height));
    _model->WinSize = Pt::Gfx::SizeF(windowRect.size.width,windowRect.size.height);
    
    //View
    NSRect viewRect = [_view frame];
    _model->Position = Pt::Gfx::PointF(viewRect.origin.x, _model->WinSize.get().height()- viewRect.origin.y);
    _model->Size = Pt::Gfx::SizeF(viewRect.size.width,viewRect.size.height);
}
    
bool GfxOutputImpl::onCanClose()
{
    if(_model->CanClose.get())
    {
        _model->Closed = true;
        return true;
    }
    
    return false;
}


Pt::Gfx::Painter* GfxOutputImpl::nativePainter()
{
	return _nativePainter;
}
    
}}
