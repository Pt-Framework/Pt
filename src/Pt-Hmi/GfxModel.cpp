#include <Pt/Hmi/GfxModel.h>
#include <Pt/Hmi/Application.h>

namespace Pt{
namespace Hmi{

GfxModel::GfxModel()
: Visible(false)
, BackColor(Pt::Gfx::ARgbColor(197,197,197))
, ForeColor(Pt::Gfx::ARgbColor(70,70,70))
{
	Position = toUnit(Pt::Gfx::Point(20,20));
	Size = toUnit(Pt::Gfx::Size(200,200));
}

GfxModel::~GfxModel()
{

}
void GfxModel::move(const Pt::Gfx::SizeF& size)		
{
	Position.get().addX(size.width());
	Position.get().addY(size.height());
}

bool GfxModel::contains(const Pt::Gfx::PointF& p)
{
	double x1, x2, y1, y2;

	x1 = Position.get().x();
	x2 = x1  + Size.get().width();

	y1 = Position.get().y();
	y2 = y1  + Size.get().height();

	return ((p.x() > x1) && (p.x() < x2) && (p.y() > y1) && (p.y() < y2));
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
