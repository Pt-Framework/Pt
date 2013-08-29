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

	Gfx::Point at(20, 20);
	Gfx::Size size(400, 200);

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
	WindowModel* wmodel = dynamic_cast<WindowModel*>(model);

    if( wmodel == 0)
		throw std::logic_error("ERROR: WindowModel model expected!");
    
    _model = wmodel;
    NSRect viewRect = [_view frame];
    
    if(_model->Size.get().width() != viewRect.size.width && _model->Size.get().height() != viewRect.size.height)
    {
        writeWindowSizeAndPos();
        return;
    }

	if(wmodel->Closed.get())
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
			if(wmodel->Visible.get())
				create();
			else
				return;
		}
	}
    
    _ignoreSizeEvent = true;
    
    
    writeWindowSizeAndPos();
    _ignoreSizeEvent = false;
    [_view setNeedsDisplay:YES];
}
    
void GfxOutputImpl::writeWindowSizeAndPos()
{
    //Position
    NSPoint pos;
    int screenHeight = [[NSScreen mainScreen] frame].size.height;
    
    pos.x = _model->WinPos.get().x();
    pos.y = screenHeight - _model->WinPos.get().y();
    
    [_window setFrameOrigin: pos];
    
    //Size
    NSRect fr =  [_window frame];
    fr.origin.y -= fr.size.height;
    fr.origin.y += _model->WinSize.get().height();
    fr.size.width = _model->WinSize.get().width();
    fr.size.height = _model->WinSize.get().height();
    [_window setFrame:fr display:NO animate:NO];
    
}
    
void GfxOutputImpl::onSize(double width, double height )
{
    if(_ignoreSizeEvent)
        return;
    
    if( _model == 0)
        return;
    
    int screenHeight = [[NSScreen mainScreen] frame].size.height;
   
    NSRect windowRect = [_window frame];
    _model->WinPos = Pt::Gfx::PointF(windowRect.origin.x,screenHeight- windowRect.origin.y);
    _model->WinSize = Pt::Gfx::SizeF(windowRect.size.width,windowRect.size.height);
    _model->Size = Pt::Gfx::SizeF(width,height);   
}
    

void GfxOutputImpl::onPosition(double x, double y)
{
    if(_ignoreSizeEvent)
        return;
    
    if( _model == 0)
        return;
    
    int screenHeight = [[NSScreen mainScreen] frame].size.height;
    _model->Position = Pt::Gfx::PointF(x,screenHeight- y);
    _model->WinPos = Pt::Gfx::PointF(x,screenHeight- y);
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
