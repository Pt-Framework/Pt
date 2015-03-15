#ifndef Pt_Forms_Window_H
#define Pt_Forms_Window_H

#include <Pt/Forms/Api.h>
#include <Pt/Forms/Widget.h>
#include <Pt/Hmi/Window.h>
#include <Pt/Hmi/WindowModel.h>
#include <Pt/Hmi/WindowView.h>

namespace Pt {
namespace Forms {

class PT_FORMS_API Window : public Pt::Connectable
{
public:
	Window();
	virtual ~Window();
	
	virtual void show();
	void close();	

	Hmi::Window& window();
	Hmi::WindowModel& windowModel();

	void setWindow(Hmi::Window& controller);

	const Hmi::Window& window() const;
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
	Pt::Hmi::WindowModel	_defModel;
	Pt::Hmi::WindowView		_defView;
	Pt::Hmi::Window			  _defController;
	Pt::Hmi::Window*	_currController;		
};
 
}}

#endif
