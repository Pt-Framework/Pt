#include <Pt/Hmi/Desktop/Widget.h>
#include <Pt/Hmi/GfxController.h>
#include <Pt/Hmi/GfxModel.h>

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
	GfxModel* m = dynamic_cast<GfxModel*>(controller().model());	
	m->Visible = true;
	controller().invalidate();
}

void Widget::hide()
{
	GfxModel* m = dynamic_cast<GfxModel*>(controller().model());
	m->Visible = false;
	controller().invalidate();	
}

void Widget::addChild(Widget* w)
{
	std::vector<Widget*>::iterator it = std::find(_children.begin(), _children.end(), w);
	
	if( it != _children.end())
	{
		std::logic_error("ERROR: Child allready exists.");
		return;
	}

	this->controller().addChild(&w->controller());
	
	_children.push_back(w); 
	w->setParent(this);
}

void Widget::removeChild(Widget* w)
{
}


}}}