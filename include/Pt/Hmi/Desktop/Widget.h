#ifndef Pt_Hmi_Desktop_Widget_H
#define Pt_Hmi_Desktop_Widget_H

#include <Pt/Hmi/Desktop/Api.h>
#include <vector>

namespace Pt{
namespace Hmi{

class GfxController;

namespace Desktop{

class PT_HMI_DESKTOP_API Widget 
{
public:
	Widget(Widget* parent = 0);
	virtual ~Widget();

	virtual void show();
	virtual void hide();

	const std::vector<Widget*>& children() const
	{
		return _children;
	}

	void addChild(Widget* w);
	void removeChild(Widget* w);

	inline Pt::Hmi::GfxController& controller()
	{
		return  *_controller;
	}
	
	inline const Pt::Hmi::GfxController& controller() const
	{
		return  *_controller;
	}
		
	inline void setController(Pt::Hmi::GfxController& controller)
	{		
		_controller = &controller;
	}


	inline const Widget* parent() const 
	{
		return _parent;
	}

	inline Widget* parent()
	{
		return _parent;
	}

protected:
	inline void setParent(Widget* p)
	{
		_parent = p;
	}

private:
	GfxController* _controller;
	std::vector<Widget*> _children;
	Widget* _parent;
};
 
}}}
#endif