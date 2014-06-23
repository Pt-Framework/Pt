#ifndef Pt_Hmi_Desktop_Dialog_H
#define Pt_Hmi_Desktop_Dialog_H

#include <Pt/Hmi/Desktop/Window.h>
#include <Pt/Hmi/DialogModel.h>
#include <Pt/Hmi/MouseDevice.h>
#include <Pt/Hmi/KeyboardDevice.h>
#include <Pt/Hmi/GfxOutputDevice.h>
#include <Pt/Hmi/DialogController.h>
#include <Pt/Hmi/DialogModel.h>
#include <Pt/Hmi/DialogRenderer.h>
#include <Pt/Hmi/WidgetController.h>

namespace Pt{
namespace Hmi{
namespace Desktop{

class PT_HMI_DESKTOP_API Dialog : public Window
{
public:
	Dialog();
	virtual ~Dialog();

	void show(Window* parent);
	void show(Dialog* parent);

	virtual DialogResultType::Type result() const;
	virtual void setResult(DialogResultType::Type r);

	DialogController& dialogController();
	void setDialogController(DialogController& controller);

	DialogModel& dialogModel();

	const DialogController& dialogController() const;
	const DialogModel& dialogModel()const ;

	virtual void setSize(const Pt::Gfx::SizeF& size);
	virtual const Pt::Gfx::SizeF& size() const;

    virtual void setPosition(const Pt::Gfx::PointF& position);
	virtual const Pt::Gfx::PointF& position() const;

protected:
	virtual void show(WindowController& parent); 

private:
	Pt::Hmi::DialogModel		_defModel;
	Pt::Hmi::DialogRenderer 	_defRenderer;

	Pt::Hmi::DialogController	_defController;
	Pt::Hmi::DialogController*	_currController;

	Pt::Hmi::MouseDevice        _mouseDevice;
	Pt::Hmi::KeyboardDevice     _keyboardDevice;
	Pt::Hmi::GfxOutputDevice    _gfxOutputDevice;
};
 
}}}
#endif