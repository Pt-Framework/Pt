#ifndef Pt_Forms_Widget_H
#define Pt_Forms_Widget_H

#include <Pt/Forms/Api.h>
#include <vector>
#include <Pt/Connectable.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Hmi/Widget.h>

namespace Pt {
namespace Forms {

class PT_FORMS_API Widget : public Pt::Connectable
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
	virtual Hmi::Widget& widgetController() = 0;

protected:    
	inline void setParent(Widget* p)
	{
		_parent = p;
	}
	


private:
	std::vector<Widget*> _children;
	Widget* _parent;
};
 
}}
#endif
