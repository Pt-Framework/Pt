#ifndef Pt_Hmi_Desktop_Dialog_H
#define Pt_Hmi_Desktop_Dialog_H

#include <Pt/Hmi/Desktop/Window.h>
#include <Pt/Hmi/DialogModel.h>
#include <Pt/Hmi/MouseDevice.h>
#include <Pt/Hmi/KeyboardDevice.h>
#include <Pt/Hmi/GfxOutput.h>

namespace Pt{
namespace Hmi{

class DialogController;
class DialogModel;
class DialogRenderer;

namespace Desktop{

class PT_HMI_DESKTOP_API Dialog : public Widget
{
public:
	Dialog();
	virtual ~Dialog();

	void show(Window* parent);
	void show(Dialog* parent);

	DialogResultType::Type result() const;

protected:
	virtual void show(WindowController* parent); 

private:
	Pt::Hmi::DialogController*	_defController;
	Pt::Hmi::DialogModel*		_defModel;
	Pt::Hmi::DialogRenderer*	_defRenderer;
	Pt::Hmi::MouseDevice        _mouseDevice;
	Pt::Hmi::KeyboardDevice     _keyboardDevice;
	Pt::Hmi::GfxOutput          _gfxOutputDevice;
};
 
}}}
#endif