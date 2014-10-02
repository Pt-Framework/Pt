#include <Pt/Hmi/WidgetController.h>
#include <Pt/Hmi/WindowController.h>
#include <Pt/Hmi/WidgetModel.h>
#include <Pt/Hmi/WidgetRenderer.h>
#include <Pt/Hmi/Painter.h>

namespace Pt{
namespace Hmi{

WidgetController::WidgetController(WidgetModel& model, WidgetRenderer& renderer)
: Controller(model)
, _renderer(renderer)
, _mnemonicWidget(0)
{
	widgetModel().Focused.Changed += Pt::slot(*this, &WidgetController::onFocusChanged);
}

WidgetController::~WidgetController()
{

}

void WidgetController::onFocusChanged(const Property<bool>& prop)
{
	if(widgetModel().Focused.get())
	{//True
		Pt::Hmi::Controller* ctrl = this;
		Pt::Hmi::WidgetController* par = (Pt::Hmi::WidgetController*) ctrl->widgetParent();
	
		if( par != 0)
		{
			WidgetModel& parMod = par->widgetModel();
			
			//All parents set to true.
			parMod.Focused.set(true);
			parMod.Focused.Changed.send(parMod.Focused);

			//All sibling set to false. Only me let it true
			for( size_t i = 0; i < par->children().size(); i++)
			{
				WidgetController* childCtrl = (WidgetController*) par->children()[i];
				WidgetModel& childModel = childCtrl->widgetModel();
				
				if(&childModel != &widgetModel())
					childModel.Focused = false;
			}
		}
	}
	else
	{//False  
		Pt::Hmi::WidgetController* ctrl = (Pt::Hmi::WidgetController*) this;
		for( size_t i = 0; i < ctrl->children().size(); ++i)
		{//All childs set to false

			WidgetController* childCtrl = (Pt::Hmi::WidgetController*) ctrl->children()[i];
			WidgetModel& m = childCtrl->widgetModel();

			m.Focused.set(false);						
			m.Focused.Changed.send(m.Focused);
		}
	}
}


Pt::Gfx::PointF WidgetController::toClient(const Pt::Gfx::PointF& globalPoint)
{
	WidgetModel& m = widgetModel();	

	if( parent() == 0)
		return Pt::Gfx::PointF(globalPoint.x(), globalPoint.y());

	Pt::Gfx::PointF parPoint = parent()->toClient(globalPoint);
	return Pt::Gfx::PointF(parPoint.x() - m.Position.get().x(), parPoint.y() - m.Position.get().y());
}

Pt::Gfx::PointF WidgetController::fromClient(const Pt::Gfx::PointF& localPoint, bool toRoot)
{
	const WidgetController* par = parent();	

	double x = localPoint.x();
	double y = localPoint.y();

	while(par != 0)
	{
		const WidgetModel& m = widgetModel();
		par = parent()->parent();
		
		if(!(toRoot && par == 0))
		{
			x += m.Position.get().x();
			y += m.Position.get().y();
		}
	}
	
	return Pt::Gfx::PointF(x,y);
}

const WidgetModel& WidgetController::widgetModel() const
{
	return static_cast<const WidgetModel&>(model());
}

void WidgetController::invalidate()
{
	WidgetController* par = parent();
	
	if( par != 0)
		par->invalidate();
}


WidgetModel& WidgetController::widgetModel()
{
	return static_cast<WidgetModel&>(model());
}

void WidgetController::render()
{
	if(!widgetModel().Visible.get())
		return;	
		
	//Draw me
	_renderer.render(&widgetModel());

	//Let the user to render 
	Render.send(*this, *widgetModel().paintSurface());

	//Render my childs
	for( size_t i = 0; i < children().size(); ++i)
	{
		WidgetController* child = childAt(i);
		child->render();
	}
}

void WidgetController::output()
{
	if(!widgetModel().Visible.get())
		return;
	
	Pt::Hmi::Painter& localPainter = widgetModel().paintSurface()->painter();

	for( size_t i = 0; i < children().size(); ++i)
	{
		WidgetController* child = childAt(i);			
		child->output();

		WidgetModel& childModel = child->widgetModel();
		localPainter.drawSurface(childModel.Position.get(),*childModel.paintSurface());
	}

	Controller::output();
}

bool WidgetController::onMoveFocusPrev()
{
	if(children().size() == 0)
		return false;

	int index = getFocusedChild();

	if( index != -1)
	{
		WidgetController* child = childAt(index);
		WidgetModel& model = child->widgetModel();	
		
		if(!model.AcceptFocus.get())
		{
			if(child->moveFocusPrev())
				return true;
		}

		model.Focused = false;
		return focusPrevChild(index);
	}
	
	return focusPrevChild(children().size());
}

bool WidgetController::focusPrevChild(int index)
{
	index--;
	
	for( ; index >= 0; --index)
	{
		WidgetController* child = childAt(index);
		WidgetModel& model = child->widgetModel();		

		if(model.AcceptFocus.get())
		{
			model.Focused = true;
			return true;
		}

		if(child->moveFocusPrev())
		{
			model.Focused = true;
			return true;
		}
	}

	return false;
}



bool WidgetController::focusNextChild(int index)
{
	index++;
	
	for( ; index < (int)children().size(); ++index)
	{
		WidgetController* child = childAt(index);
		WidgetModel& model = child->widgetModel();
		
		if(model.AcceptFocus.get())
		{
			model.Focused = true;
			return true;
		}

		if(child->moveFocusNext())
		{
			model.Focused = true;
			return true;
		}
	}

	return false;
}


int WidgetController::getFocusedChild() const
{
	int i = 0;
	
	for( ; i < (int)children().size(); ++i)
	{
		const WidgetController* child = childAt(i);
		const WidgetModel& model = child->widgetModel();

		if(model.Focused.get())
			return i;		
	}		

	return -1;
}

bool WidgetController::onMoveFocusNext()
{
	if(children().size() == 0)
		return false;

	const int index = getFocusedChild();

	if( index == -1)
		return focusNextChild(index);
	
	WidgetController* child = childAt(index);
	WidgetModel& model = child->widgetModel();	
		
	if(!model.AcceptFocus.get())
	{
		if(child->moveFocusNext())
			return true;
	}

	model.Focused = false;

	return focusNextChild(index);
}

void WidgetController::onKeyInput(const KeyEvent& ev)
{ 
	WidgetModel& m = widgetModel();
	
	m.KeyStatus = ev;
	
	if(m.UseMnemonic.get() && _mnemonicWidget != 0 && m.Enabled.get() && ev.state() == Pt::Hmi::KeyEvent::KeyUp)
	{		
		std::string mnKey = "";

		if(m.KeyStatus.get().alt())
			mnKey = "A//";
			
		mnKey += m.KeyStatus.get().toUTF8String();

		 if(m.getMnemonicKey() == mnKey)
			_mnemonicWidget->onMnemonic();			
	}

	for( size_t i = 0; i < children().size(); ++i)
		children()[i]->notifyKeyInput(ev);
}

void WidgetController::onPointerInput(const PointingEvent& ev)
{
	WidgetModel& m = widgetModel();
	
	m.Pointer2DStatus = ev;

	for( size_t i = 0; i < children().size(); ++i)
		children()[i]->notifyPointerInput(ev);
}

void WidgetController::bindMnemonicToWidget(WidgetController* widget)
{
	_mnemonicWidget = widget;
}

void WidgetController::onMnemonic()
{
	if(widgetModel().Focused.get() != true)
		widgetModel().Focused = true;
}

}}
