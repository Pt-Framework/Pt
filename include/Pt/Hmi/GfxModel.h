#ifndef Pt_Hmi_Model_Gfx_h
#define Pt_Hmi_Model_Gfx_h

#include <Pt/Hmi/Model.h>
#include <Pt/Hmi/Property.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/Gfx/Gfx.h>
#include <Pt/Gfx/Font.h>
#include <Pt/Gfx/ARgbImage.h>
#include <Pt/Gfx/Painter.h>


namespace Pt{
namespace Hmi{

namespace ImageLayout
{
enum  Layout
{
	None,
	Tile,
	Center,
	Strech,
	Zoom
};
}

class PT_HMI_API GfxModel : public Model
{
protected:
	GfxModel();

public:
	Property<bool>					Invalid;
	Property<Pt::Gfx::Font>			Font;
	Property<Pt::Gfx::PointF>		Position;
	Property<Pt::Gfx::SizeF>		Size;
	Property<Pt::Gfx::ARgbColor>	BackColor;
	Property<Pt::Gfx::ARgbColor>	ForeColor;
	Property<Pt::Gfx::ARgbImage>	BackgroundImage;
	Property<ImageLayout::Layout>	BackgroundImageLayout;
	Property<int>					Opacity;
	Property<Pt::Gfx::ARgbColor>	TransparancyKey;

public:
	virtual ~GfxModel();

	void move(const Pt::Gfx::SizeF& size);	
    bool contains(const Pt::Gfx::PointF& p);

	//Unit handling
	double toUnit(int v);
	Pt::Gfx::PointF toUnit(const Pt::Gfx::Point& value);
	Pt::Gfx::SizeF toUnit(const Pt::Gfx::Size& value);
	int fromUnit(double v);
	Pt::Gfx::Point fromUnit(const Pt::Gfx::PointF& value);
	Pt::Gfx::Size fromUnit(const Pt::Gfx::SizeF& value);
	double unitSizeInch() const;
	double unitSizeMm() const;
};

}}

#endif