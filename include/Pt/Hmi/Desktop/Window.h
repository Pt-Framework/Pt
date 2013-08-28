#ifndef Pt_Hmi_Desktop_Window_H
#define Pt_Hmi_Desktop_Window_H

#include <Pt/Hmi/Desktop/Api.h>
#include <Pt/Hmi/Desktop/Widget.h>
#include <Pt/Hmi/Desktop/Panel.h>
#include <Pt/Hmi/MouseDevice.h>
#include <Pt/Hmi/KeyboardDevice.h>
#include <Pt/Hmi/GfxOutput.h>

namespace Pt{
namespace Hmi{

class WindowController;
class WindowModel;
class WindowRenderer;

namespace Desktop{

class PT_HMI_DESKTOP_API Window : public Widget
{
public:
	Window();



	virtual ~Window();
	
	virtual void show();
	void close();
	
	Pt::Signal<> Closed;
	Pt::Signal<> CanClose;

private:
	Pt::Hmi::WindowController*	_defController;
	Pt::Hmi::WindowModel*		_defModel;
	Pt::Hmi::WindowRenderer*	_defRenderer;
	Pt::Hmi::MouseDevice        _mouseDevice;
	Pt::Hmi::KeyboardDevice     _keyboardDevice;
	Pt::Hmi::GfxOutput          _gfxOutputDevice;
};
 
}}}
#endif