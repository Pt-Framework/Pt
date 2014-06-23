#include <Pt/Hmi/Desktop/Widget.h>
#include <Pt/Hmi/GfxController.h>
#include <Pt/Hmi/GfxModel.h>
#include <algorithm>

namespace Pt{
namespace Hmi{
namespace Desktop{

Widget::Widget(Widget* parent)
: _parent(parent)
{
}

Widget::~Widget()
{

}

void Widget::show()
{	
	widgetController().widgetModel().Visible = true;
	widgetController().invalidate();
}

void Widget::hide()
{
	widgetController().widgetModel().Visible = false;
	widgetController().invalidate();
}

void Widget::addChild(Widget* w)
{
	std::vector<Widget*>::iterator it = std::find(_children.begin(), _children.end(), w);
	
	if( it != _children.end())
	{
		throw std::logic_error("ERROR: Child allready exists.");
		return;
	}

	widgetController().addChild(&w->widgetController());
	
	_children.push_back(w); 
	w->setParent(this);
}

void Widget::removeChild(Widget* w)
{
}


}}}