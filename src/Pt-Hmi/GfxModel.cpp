#include <Pt/Hmi/GfxModel.h>
#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/Controller.h>
#include <Pt/Hmi/NativePaintSurface.h>
#include <Pt/Hmi/ImagePaintSurface.h>
#include <Pt/Slot.h>
#include <Pt/Hmi/GfxController.h>

namespace Pt{
namespace Hmi{

GfxModel::GfxModel()
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
, PT_HMI_INIT_PROPERTY_VALUE(PainterSurfaceType, PainterType::Native)
, _paintSurface(new NativePaintSurface(Pt::Gfx::SizeF(800,600)))
{
	Position.set(Pt::Gfx::PointF(0,0));

	registerProperty(Visible);
	registerProperty(Font);
	registerProperty(Position);
	registerProperty(Size);
	registerProperty(BackColor);
	registerProperty(BackColorHightLight);
	registerProperty(ForeColor);
	registerProperty(BackgroundImage);
	registerProperty(BackgroundImageLayout);
	registerProperty(Opacity);
	registerProperty(TransparancyKey);
	registerProperty(Pointer2DStatus);
	registerProperty(KeyStatus);
	registerProperty(CursorT);
	registerProperty(TextAlign);
	registerProperty(Focused);
	registerProperty(AcceptFocus);
	registerProperty(HighLight);	
	registerProperty(FocusedActionKey);
	registerProperty(PainterSurfaceType);

	PainterSurfaceType.Changed += Pt::slot(*this, &GfxModel::onPainterTypeChanged);
}

GfxModel::~GfxModel()
{
	if(_paintSurface != 0)
			delete _paintSurface;
}

void GfxModel::onPainterTypeChanged(const Property<PainterType::Type>& prop)
{
	switch(PainterSurfaceType.get())
	{
		case PainterType::Native:
			setPaintSurface( new NativePaintSurface(Size.get()));
		break;
		
		case PainterType::Image:
			setPaintSurface( new ImagePaintSurface(Size.get())); 
		break;
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
