#ifndef Pt_Forms_Window_H
#define Pt_Forms_Window_H

#include <Pt/Forms/Api.h>
#include <Pt/Forms/Widget.h>
#include <Pt/Hmi/View.h>
#include <Pt/Hmi/WindowController.h>
#include <Pt/Hmi/WindowModel.h>
#include <Pt/Hmi/WindowRenderer.h>

namespace Pt {
namespace Forms {

class PT_FORMS_API Window : public Pt::Connectable
{
public:
	Window();
	virtual ~Window();
	
	virtual void show();
	void close();	

	Hmi::WindowController& windowController();
	Hmi::WindowModel& windowModel();

	void setWindowController(Hmi::WindowController& controller);

	const Hmi::WindowController& windowController() const;
	const Hmi::WindowModel& windowModel() const;

		
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
	Pt::Hmi::View    _View;
};
 
}}

#endif
