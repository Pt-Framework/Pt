#ifndef Pt_Hmi_GfxOutputDeviceImpl_H
#define Pt_Hmi_GfxOutputDeviceImpl_H

#include <Pt/Hmi/Model.h>
#include <Pt/Hmi/OutputDevice.h>
#include <Pt/Hmi/Api.h>
#include <Pt/Gfx/Gfx.h>
#include <Pt/Gfx/Painter.h>
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
	void OnPaint(HWND hwnd);

	void drawIndependentImage(size_t x, size_t y, const char* data, size_t width, size_t height);
	HWND _hwnd;
	Pt::Hmi::Model* _model;
	Pt::Gfx::Painter* _nativePainter;
};

}}
#endif