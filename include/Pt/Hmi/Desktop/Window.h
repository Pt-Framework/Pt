#ifndef Pt_Hmi_Desktop_Window_H
#define Pt_Hmi_Desktop_Window_H

#include <Pt/Hmi/Desktop/Api.h>
#include <Pt/Hmi/Desktop/Widget.h>
#include <Pt/Hmi/GfxOutputDevice.h>
#include <Pt/Hmi/WindowController.h>
#include <Pt/Hmi/WindowModel.h>
#include <Pt/Hmi/WindowRenderer.h>

namespace Pt{
namespace Hmi{
namespace Desktop{

class PT_HMI_DESKTOP_API Window : public Pt::Connectable
{
public:
	Window();
	virtual ~Window();
	
	virtual void show();
	void close();	

	WindowController& windowController();
	WindowModel& windowModel();

	void setWindowController(WindowController& controller);

	const WindowController& windowController() const;
	const WindowModel& windowModel() const;

		
	virtual void setSize(const Pt::Gfx::SizeF& size);
	virtual const Pt::Gfx::SizeF& size() const;

    virtual void setPosition(const Pt::Gfx::PointF& position);
	virtual const Pt::Gfx::PointF& position() const;

	void addChild(Widget* w);

public:	
	Pt::Signal<> Closed;
	Pt::Signal<> CanClose;
	
private:	
	Pt::Hmi::WindowModel		_defModel;
	Pt::Hmi::WindowRenderer		_defRenderer;
	Pt::Hmi::WindowController	_defController;
	Pt::Hmi::WindowController*	_currController;	
	Pt::Hmi::GfxOutputDevice    _gfxOutputDevice;
};
 
}}}

#endif