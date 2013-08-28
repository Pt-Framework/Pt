#ifndef Pt_Hmi_GfxOutputDeviceImpl_H
#define Pt_Hmi_GfxOutputDeviceImpl_H

#include <Pt/Gfx/Gfx.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/Hmi/Model.h>
#include <Pt/Hmi/OutputDevice.h>
#include <Pt/Hmi/Api.h>
#include <Pt/Hmi/WindowModel.h>
#include <Pt/Hmi/KeyEvent.h>
#include <Pt/Hmi/PointingEvent.h>
#include <Windows.h>
#include <map>

namespace Pt{
namespace Hmi{

class GfxOutputImpl : public Pt::Connectable
{
public:
	GfxOutputImpl();
	virtual ~GfxOutputImpl();

	void output(Pt::Hmi::Model* model);
	Pt::Gfx::Painter* nativePainter();

private:
	void onWindowEvent(HWND wnd, unsigned int msg, WPARAM wparam, LPARAM lparam, bool& handled);
	virtual void onPaint();
	virtual void onSize(WPARAM wparam, LPARAM lparam);
	virtual void onMouse(unsigned int msg,  WPARAM wparam, LPARAM lparam);
	virtual void onKey(unsigned int ms, WPARAM wparam, LPARAM lparam);
	virtual void onMove();
	virtual bool onClosing();
	virtual void onClosed();
	void readWindowSize();
	void readWindowPos();
	void writeWindowSizeAndPos();
	void writeWindowProperties();
	void drawIndependentImage(size_t x, size_t y, const char* data, size_t width, size_t height);
	void create();
	void destroy();
	void output();

private:
	HWND					_hwnd;
	Pt::Hmi::WindowModel*	_model;
	Pt::Gfx::Painter*		_nativePainter;
	bool					_ignoreEvent;
	KeyEvent				_keyEvent;
	PointingEvent			_pointerEvent;
	Pt::Gfx::Rgb888Image	_rgb88Image;
};

}}
#endif