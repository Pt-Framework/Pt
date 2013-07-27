#ifndef Pt_Hmi_Desktop_Window_H
#define Pt_Hmi_Desktop_Window_H

#include <Pt/Hmi/Desktop/Api.h>
#include <Pt/Hmi/MouseDevice.h>
#include <Pt/Hmi/GfxOutput.h>

namespace Pt{
namespace Hmi{

class WindowController;
class WindowModel;
class WindowRenderer;
class WindowRenderer;

namespace Desktop{

class PT_HMI_DESKTOP_API Window 
{
public:
	Window();
	virtual ~Window();


	void show();
	void hide();

	inline Pt::Hmi::WindowController& controller()
	{
		return  *_controller;
	}
	
	inline const Pt::Hmi::WindowController& controller() const
	{
		return  *_controller;
	}
	
	
	inline void setController(Pt::Hmi::WindowController& controller)
	{		
		_controller = &controller;
	}

private:
	Pt::Hmi::WindowController*	_defController;
	Pt::Hmi::WindowModel*		_defModel;
	Pt::Hmi::WindowRenderer*	_defRenderer;
	Pt::Hmi::MouseDevice        _mouseDevice;
	Pt::Hmi::GfxOutput          _gfxOutputDevice;
	Pt::Hmi::WindowController*  _controller;
};
 
}}}
#endif