#include <Pt/Hmi/WidgetModel.h>
#include <Pt/Hmi/PaintSurface.h>
#include <Pt/Hmi/Application.h>

namespace Pt{
namespace Hmi{

WidgetModel::WidgetModel()
: PT_HMI_INIT_PROPERTY_VALUE(Visible,true)
, PT_HMI_INIT_PROPERTY_VALUE(Font,Pt::Gfx::Font("Sans Serif",12))
, PT_HMI_INIT_PROPERTY_VALUE(Position, Pt::Gfx::PointF(0,0))
, PT_HMI_INIT_PROPERTY_VALUE(Size, Pt::Gfx::SizeF(10,10))
, PT_HMI_INIT_PROPERTY_VALUE(BackColor,Pt::Gfx::ARgbColor(237,237,237))
, PT_HMI_INIT_PROPERTY_VALUE(BackColorHightLight,Pt::Gfx::ARgbColor(200,200,200))
, PT_HMI_INIT_PROPERTY_VALUE(ForeColor, Pt::Gfx::ARgbColor(70,70,70))
, PT_HMI_INIT_PROPERTY_VALUE(BackgroundImage, Pt::Gfx::ARgbImage(0,0))
, PT_HMI_INIT_PROPERTY_VALUE(BackgroundImageLayout, ImageLayoutType::NoLayout)
, PT_HMI_INIT_PROPERTY_VALUE(Opacity,0)
, PT_HMI_INIT_PROPERTY(TransparancyKey)
, PT_HMI_INIT_PROPERTY(Pointer2DStatus)
, PT_HMI_INIT_PROPERTY(KeyStatus)
, PT_HMI_INIT_PROPERTY(CursorT)
, PT_HMI_INIT_PROPERTY_VALUE(TextAlign,TextAlignType::MidleCenter)
, PT_HMI_INIT_PROPERTY_VALUE(Focused, false)
, PT_HMI_INIT_PROPERTY_VALUE(AcceptFocus,true)
, PT_HMI_INIT_PROPERTY_VALUE(HighLight, false)
, PT_HMI_INIT_PROPERTY_VALUE(FocusedActionKey," ")
, PT_HMI_INIT_PROPERTY_VALUE(Caption,"")
, PT_HMI_INIT_PROPERTY_VALUE(UseMnemonic,true)
, _paintSurface(new PaintSurface(Pt::Gfx::SizeF(800,600)))
{
	registerProperty(Caption);
	registerProperty(UseMnemonic);
}

WidgetModel::~WidgetModel()
{
}

std::string WidgetModel::getMnemonicKey() const
{
	std::string mnemonic = "";

	int index = Caption.get().find('&');
	
	if( index < 0 || ((index + 1)> Caption.get().size()))
		return mnemonic;


	mnemonic = "A//";	
	mnemonic+= std::tolower(Caption.get()[index + 1]);
	return mnemonic;	
}


void WidgetModel::move(const Pt::Gfx::SizeF& size)		
{
	Position.get().addX(size.width());
	Position.get().addY(size.height());
}

bool WidgetModel::contains(const Pt::Gfx::PointF& p)
{
	double x1, x2, y1, y2;
	
	x1 = 0;
	x2 = x1  + Size.get().width();

	y1 = 0;
	y2 = y1  + Size.get().height();
	
	return ((p.x() >= x1) && (p.x() < x2) && (p.y() >= y1) && (p.y() < y2));
}

int WidgetModel::fromUnit(double v)
{
	Application* app = (Application*) &Application::instance();
	return app->fromUnit(v);
}

double WidgetModel::toUnit(int v)
{
	Application* app = (Application*) &Application::instance();
	return app->toUnit(v);
}

Pt::Gfx::PointF WidgetModel::toUnit(const Pt::Gfx::Point& value)
{
	Application* app = (Application*) &Application::instance();
	return app->toUnit(value);
}

Pt::Gfx::SizeF WidgetModel::toUnit(const Pt::Gfx::Size& value)
{	
	Application* app = (Application*) &Application::instance();
	return app->toUnit(value);
}

Pt::Gfx::Point WidgetModel::fromUnit(const Pt::Gfx::PointF& value)
{
	Application* app = (Application*) &Application::instance();
	return app->fromUnit(value);
}

Pt::Gfx::Size WidgetModel::fromUnit(const Pt::Gfx::SizeF& value)
{
	Application* app = (Application*) &Application::instance();
	return app->fromUnit(value);
}

double WidgetModel::unitSizeInch() const
{
	Application* app = (Application*) &Application::instance();
	return app->unitSizeInch();
}

double WidgetModel::unitSizeMm() const
{
	Application* app = (Application*) &Application::instance();
	return app->unitSizeMm();
}


}}