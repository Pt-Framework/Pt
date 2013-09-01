#ifndef Pt_Hmi_GfxOutputDeviceImpl_H
#define Pt_Hmi_GfxOutputDeviceImpl_H

#include <Pt/Hmi/Model.h>
#include <Pt/Hmi/OutputDevice.h>
#include <Pt/Hmi/Api.h>
#include <Pt/Gfx/Gfx.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/Hmi/WindowModel.h>
#include <Pt/Hmi/PointingEvent.h>
#include <Pt/Hmi/KeyEvent.h>
#include <Pt/System/Timer.h>
#include <map>

#ifdef __OBJC__
    #import <AppKit/NSWindow.h>
    #import <AppKit/NSGraphicsContext.h>
#else
	struct NSView;
    struct NSWindow;
    struct NSResponder;
    struct NSGraphicsContext;
#endif

namespace Pt{
	
namespace Hmi{

class GfxOutputImpl : public Pt::Connectable
{
public:
	GfxOutputImpl();
	virtual ~GfxOutputImpl();

	void output(Pt::Hmi::Model* model);
	Pt::Gfx::Painter* nativePainter();
    
    WindowModel* model()
    {
        return _model;
    }

public:
    void onPositionAndSize();
    void onPosition();
    bool onCanClose();
    void onMouseMove(double x,double y);
    void onLMouseDown(double x, double y);
    void onLMouseUp(double x, double y);
    void onKeyDown(int key);
    void onKeyUp(int key);
    void onSpezialKeyEvent(unsigned int mask);
    void onLostFocus();
    
private:
    void writeWindowSizeAndPos();
	void checkModal();
	//void writeWindowProperties();

    
	void create();
	void destroy();
    Pt::Gfx::PointF convertMousePosToGlobal(double x, double y);

private:
	NSWindow* _window;
	NSView* _view;

	Pt::Hmi::WindowModel* _model;
	Pt::Gfx::Painter* _nativePainter;
	bool _ignoreSizeEvent;
    Pt::Hmi::PointingEvent 	_mouseEvent;
	Pt::Hmi::KeyEvent      	_keyEvent;
    Pt::System::Timer       _timer;
    bool _visible ;
    int _level;
};

}}
#endif