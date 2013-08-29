#ifndef Pt_Hmi_GfxOutputDeviceImpl_H
#define Pt_Hmi_GfxOutputDeviceImpl_H

#include <Pt/Hmi/Model.h>
#include <Pt/Hmi/OutputDevice.h>
#include <Pt/Hmi/Api.h>
#include <Pt/Gfx/Gfx.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/Hmi/WindowModel.h>
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
    void onSize(double width, double height );
    void onPosition(double x, double y);
    bool onCanClose();
private:
    void writeWindowSizeAndPos();
	/*void readWindowSizeAndPos();
	void writeWindowProperties();
	void drawIndependentImage(size_t x, size_t y, const char* data, size_t width, size_t height);
	
	 */
	void create();
	void destroy();

private:
	NSWindow* _window;
	NSView* _view;

	Pt::Hmi::WindowModel* _model;
	Pt::Gfx::Painter* _nativePainter;
	bool _ignoreSizeEvent;
};

}}
#endif