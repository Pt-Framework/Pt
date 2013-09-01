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
, _visible(false)
{
    _mouseEvent.buttons().resize(3);
    
    _timer.setActive(Application::instance().loop());
    _timer.timeout() += Pt::slot(*this, &GfxOutputImpl::onPosition);
	create();
}

NSView* GfxOutputImpl::view()
{
        return _view;
}
    
void GfxOutputImpl::create()
{
    _window = nil;
    _view = [[WidgetView alloc] init: this ];

	Gfx::PointF at(20, 20);
	Gfx::SizeF size(400, 200);

	_window = [[Window alloc] initWithContentRect:NSMakeRect(at.x(), at.y(), size.width(), size.height()) styleMask:NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask | NSResizableWindowMask backing:NSBackingStoreBuffered defer:NO];
    
    [_window setController: this ];
    
	[_window setReleasedWhenClosed: NO];
	[_window setAcceptsMouseMovedEvents:YES];
    [_window setInitialFirstResponder: _view];
	[_window setContentView: _view];
	[_window setDelegate: _view];
    
    
   [_window makeKeyAndOrderFront:_window];
[_view setHidden:NO];
      _level = [_window level];
  _timer.start(100);
}

GfxOutputImpl::~GfxOutputImpl()
{
	destroy();
}

void GfxOutputImpl::destroy()
{
    if(_window == nil)
        return;
    
    _timer.stop();
    [_window close];
    [_window release];
    [_view release];
	_view = nil;
    _window = nil;
    //Notife closed
    WindowController* controller = (WindowController*)_model->controller();
    controller->Closed.send();
}

void GfxOutputImpl::output(Pt::Hmi::Model* model)
{
	_model = dynamic_cast<WindowModel*>(model);

    if( _model == 0)
		throw std::logic_error("ERROR: WindowModel model expected!");

    NSRect viewRect = [_view frame];
    
    //Initial size and position
    if(_model->Size.get().width() != viewRect.size.width || _model->Size.get().height() != viewRect.size.height)
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
    checkModal();
    writeWindowSizeAndPos();
    _ignoreSizeEvent = false;
    
    //Redraw
    [_view setNeedsDisplay:YES];
    
}
    
void GfxOutputImpl::onLMouseUp(double x, double y)
{
    Pt::Gfx::PointF pos = convertMousePosToGlobal(x,y);
    
    _mouseEvent.buttons()[0].setState(DeviceButton::Released);
    _mouseEvent.setX(pos.x());
	_mouseEvent.setY(pos.y());
    Application::instance().pointerEvent().send(_model->controller(), _mouseEvent);
}
    
void GfxOutputImpl::onLMouseDown(double x, double y)
{
    Pt::Gfx::PointF pos = convertMousePosToGlobal(x,y);
    
    _mouseEvent.buttons()[0].setState(DeviceButton::Pressed);
    _mouseEvent.setX(pos.x());
	_mouseEvent.setY(pos.y());
    Application::instance().pointerEvent().send(_model->controller(), _mouseEvent);
}
    
void GfxOutputImpl::onMouseMove(double x, double y)
{
    Pt::Gfx::PointF pos = convertMousePosToGlobal(x,y);
    _mouseEvent.setX(pos.x());
    _mouseEvent.setY(pos.y());
    Application::instance().pointerEvent().send(_model->controller(), _mouseEvent);
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
    
Pt::Gfx::PointF GfxOutputImpl::convertMousePosToGlobal(double x, double y)
{
    int screenHeight = [[NSScreen mainScreen] frame].size.height;
    NSRect windowRect = [_view frame];
    double gx = x;
    double gy = (windowRect.size.height - y);
    return Pt::Gfx::PointF(gx,gy);
}
    
void GfxOutputImpl::onKeyDown(int key)
{
    _keyEvent.setUnicode(key);
    
    _keyEvent.setState(KeyEvent::KeyDown);
    
	Application::instance().keyDeviceEvent().send(_model->controller(), _keyEvent);
}

void GfxOutputImpl::onKeyUp(int key)
{
    _keyEvent.setUnicode(key);
    
    _keyEvent.setState(KeyEvent::KeyUp);
    
	Application::instance().keyDeviceEvent().send(_model->controller(), _keyEvent);
}
    
void GfxOutputImpl::onSpezialKeyEvent(unsigned int mask)
{
    _keyEvent.setUnicode(0);
    _keyEvent.setAlt((mask & NSAlternateKeyMask) == NSAlternateKeyMask);
    _keyEvent.setShift(((mask & NSShiftKeyMask) == NSShiftKeyMask) | ((mask & NSAlphaShiftKeyMask) == NSAlphaShiftKeyMask));
    _keyEvent.setCtrl(((mask & NSControlKeyMask) == NSControlKeyMask) | ((mask & NSCommandKeyMask) == NSCommandKeyMask));
    Application::instance().keyDeviceEvent().send(_model->controller(), _keyEvent);
}
    
void GfxOutputImpl::checkModal()
{
    if(_model->TopMost.get())
    {
        [_window setLevel: NSFloatingWindowLevel];
    }
    else
    {
        [_window setLevel: NSNormalWindowLevel];
    }
}
    
void GfxOutputImpl::onPosition()
{
    if(_ignoreSizeEvent)
        return;
    
    if( _model == 0)
        return;
    
    int screenHeight = [[NSScreen mainScreen] frame].size.height;
    
    //Window
    NSRect windowRect = [_window frame];
    Pt::Gfx::PointF pos(windowRect.origin.x,screenHeight - (windowRect.origin.y + windowRect.size.height));
    
    if( pos.x()  == _model->WinPos.get().x() && pos.y()  == _model->WinPos.get().y())
        return;
    
    _model->WinPos =pos;
    
    //View
    NSRect viewRect = [_view frame];
    
    _model->Position = Pt::Gfx::PointF(viewRect.origin.x, windowRect.size.height - viewRect.size.height);
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
    _model->Position = Pt::Gfx::PointF(viewRect.origin.x, windowRect.size.height - viewRect.size.height);
    _model->Size = Pt::Gfx::SizeF(viewRect.size.width,viewRect.size.height);
}
    
bool GfxOutputImpl::onCanClose()
{
    bool canClose = false;
    
    WindowController* controller = (WindowController*)_model->controller();
    controller->Closing.send(canClose);
    
    if(canClose)
        controller->Closed.send();
    
    return canClose;
}


Pt::Gfx::Painter* GfxOutputImpl::nativePainter()
{
	return _nativePainter;
}
    
}}
