#include <Pt/Hmi/GfxModel.h>
#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/Controller.h>
#include <Pt/Slot.h>

namespace Pt{
namespace Hmi{

GfxModel::GfxModel()
: DefinePropertyInitMacro(Visible,true)
, DefinePropertyInitMacro(Font,Pt::Gfx::Font("",12))
, DefinePropertyInitMacro(Position, Pt::Gfx::PointF(0,0))
, DefinePropertyInitMacro(Size, Pt::Gfx::SizeF(10,10))
, DefinePropertyInitMacro(BackColor,Pt::Gfx::ARgbColor(237,237,237))
, DefinePropertyInitMacro(BackColorHightLight,Pt::Gfx::ARgbColor(200,200,200))
, DefinePropertyInitMacro(ForeColor, Pt::Gfx::ARgbColor(70,70,70))
, DefinePropertyInitMacro(BackgroundImage, Pt::Gfx::ARgbImage(0,0))
, DefinePropertyInitMacro(BackgroundImageLayout, ImageLayoutType::NoLayout)
, DefinePropertyInitMacro(Opacity,0)
, DefinePropertyDefaultMacro(TransparancyKey)
, DefinePropertyDefaultMacro(Pointer2DStatus)
, DefinePropertyDefaultMacro(KeyStatus)
, DefinePropertyDefaultMacro(CursorT)
, DefinePropertyInitMacro(TextAlign,TextAlignType::MidleCenter)
, DefinePropertyInitMacro(Focused, false)
, DefinePropertyInitMacro(AcceptFocus,true)
, DefinePropertyInitMacro(HighLight, false)
, DefinePropertyInitMacro(FocusedActionKey," ")
{
	Position = toUnit(Pt::Gfx::Point(0,0));
	Focused.PropertyChanged += Pt::slot(*this, &GfxModel::onFocusChanged);
}

GfxModel::~GfxModel()
{

}

void GfxModel::onFocusChanged(const void* sender, const PropertyBase& prop)
{
	if(Focused.get())
	{//True
		Pt::Hmi::Controller* ctrl = this->controller();
		Pt::Hmi::Controller* par = ctrl->widgetParent();
	
		if( par != 0)
		{
			GfxModel* parMod = (GfxModel*) par->model();
			
			//All parents set to true.
			parMod->Focused.set(true);
			parMod->Focused.PropertyChanged.send(parMod, parMod->Focused);

			//All sibling set to false. Only me let it true
			for( size_t i = 0; i < par->children().size(); i++)
			{
				GfxModel* childModel = (GfxModel*) par->children()[i]->model();
				
				if(childModel != this)
					childModel->Focused = false;
			}
		}
	}
	else
	{//False  
		Pt::Hmi::Controller* ctrl = this->controller();
		for( size_t i = 0; i < ctrl->children().size(); ++i)
		{//All childs set to false
			GfxModel* m = (GfxModel*) ctrl->children()[i]->model();
			m->Focused.set(false);						
			m->Focused.PropertyChanged.send(m, m->Focused);
		}
	}
}

void GfxModel::move(const Pt::Gfx::SizeF& size)		
{
	Position.get().addX(size.width());
	Position.get().addY(size.height());
}

bool GfxModel::contains(const Pt::Gfx::PointF& p)
{
	double x1, x2, y1, y2;
	
	x1 = 0;
	x2 = x1  + Size.get().width();

	y1 = 0;
	y2 = y1  + Size.get().height();
	
	return ((p.x() >= x1) && (p.x() < x2) && (p.y() >= y1) && (p.y() < y2));
}

int GfxModel::fromUnit(double v)
{
	Application* app = (Application*) &Application::instance();
	return app->fromUnit(v);
}

double GfxModel::toUnit(int v)
{
	Application* app = (Application*) &Application::instance();
	return app->toUnit(v);
}

Pt::Gfx::PointF GfxModel::toUnit(const Pt::Gfx::Point& value)
{
	Application* app = (Application*) &Application::instance();
	return app->toUnit(value);
}

Pt::Gfx::SizeF GfxModel::toUnit(const Pt::Gfx::Size& value)
{	
	Application* app = (Application*) &Application::instance();
	return app->toUnit(value);
}

Pt::Gfx::Point GfxModel::fromUnit(const Pt::Gfx::PointF& value)
{
	Application* app = (Application*) &Application::instance();
	return app->fromUnit(value);
}

Pt::Gfx::Size GfxModel::fromUnit(const Pt::Gfx::SizeF& value)
{
	Application* app = (Application*) &Application::instance();
	return app->fromUnit(value);
}

double GfxModel::unitSizeInch() const
{
	Application* app = (Application*) &Application::instance();
	return app->unitSizeInch();
}

double GfxModel::unitSizeMm() const
{
	Application* app = (Application*) &Application::instance();
	return app->unitSizeMm();
}

}}
