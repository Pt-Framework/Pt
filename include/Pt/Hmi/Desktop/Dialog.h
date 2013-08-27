#ifndef Pt_Hmi_Desktop_Dialog_H
#define Pt_Hmi_Desktop_Dialog_H

#include <Pt/Hmi/Desktop/Api.h>
#include <Pt/Hmi/Desktop/Widget.h>
#include <Pt/Hmi/Desktop/Panel.h>
#include <Pt/Hmi/MouseDevice.h>
#include <Pt/Hmi/KeyboardDevice.h>
#include <Pt/Hmi/GfxOutput.h>
#include <Pt/Hmi/DesktopWindow.h>

namespace Pt{
namespace Hmi{

class DialogController;
class DialogModel;
class DialogRenderer;

namespace Desktop{

class PT_HMI_DESKTOP_API Dialog : public Window
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