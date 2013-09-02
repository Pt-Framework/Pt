#ifndef Pt_Hmi_Desktop_Widget_H
#define Pt_Hmi_Desktop_Widget_H

#include <Pt/Hmi/Desktop/Api.h>
#include <vector>
#include <Pt/Connectable.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>

namespace Pt{
namespace Hmi{

class GfxController;

namespace Desktop{

class PT_HMI_DESKTOP_API Widget : public Pt::Connectable
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


	inline const Widget* parent() const 
	{
		return _parent;
	}

	inline Widget* parent()
	{
		return _parent;
	}

	virtual void setCaption(const std::string& caption)
	{
	}

    virtual const std::string& caption() const
	{
		static std::string empty;
		return empty;
	}

	virtual void setSize(const Pt::Gfx::SizeF& size) = 0;
	virtual const Pt::Gfx::SizeF& size() const = 0;

    virtual void setPosition(const Pt::Gfx::PointF& position) = 0;
	virtual const Pt::Gfx::PointF& position() const = 0;

protected:
    
	virtual void setController(Pt::Hmi::GfxController& controller)
	{
		_controller = &controller;
	}
    
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
