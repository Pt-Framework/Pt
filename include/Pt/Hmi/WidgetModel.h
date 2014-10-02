#ifndef Pt_Hmi_Model_Windget_h
#define Pt_Hmi_Model_Windget_h

#include <Pt/Hmi/Model.h>
#include <Pt/Hmi/Property.h>
#include <Pt/Hmi/PointingEvent.h>
#include <Pt/Hmi/KeyEvent.h>
#include <Pt/Hmi/Cursor.h>
#include <Pt/Hmi/PaintSurface.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Font.h>
#include <Pt/Gfx/ARgbImage.h>

namespace Pt{
namespace Hmi{

    
namespace BorderStyleType
{
    enum Type
    {
        NoBorder,
        Single,
        Border3D,
        Sizeable,
        Widget,
    };
}

namespace ImageLayoutType
{
	enum Type
	{
		NoLayout,		
		Tile,
		Center,
		Strech,
		Zoom
	};
}

namespace TextAlignType
{
	enum Type
	{
		TopLeft,
		TopCenter,
		TopRight,
		MidleLeft,
		MidleCenter,
		MidleRight,
		BottomLeft,
		BottomCenter,
		BottomRight,
	};
}


namespace PainterType
{
	enum Type
	{
		Native,
		Image
	};
}

    
class PT_HMI_API WidgetModel : public Model
{
public:
	WidgetModel();
	virtual ~WidgetModel();

public:
	Property<bool>					Visible;
	Property<Pt::Gfx::Font>			Font;
	Property<Pt::Gfx::PointF>		Position;
	Property<Pt::Gfx::SizeF>		Size;
	Property<Pt::Gfx::ARgbColor>	BackColor;
    Property<Pt::Gfx::ARgbColor>	BackColorHightLight;
	Property<Pt::Gfx::ARgbColor>	ForeColor;
	Property<Pt::Gfx::ARgbImage>	BackgroundImage;
	Property<ImageLayoutType::Type>	BackgroundImageLayout;
	Property<int>					Opacity;
	Property<Pt::Gfx::ARgbColor>	TransparancyKey;
	Property<PointingEvent>			Pointer2DStatus;	
	Property<KeyEvent>				KeyStatus;	
	Property<Cursor>                CursorT;
	Property<TextAlignType::Type>	TextAlign;
	Property<bool>					Focused; 
	Property<bool>					AcceptFocus;
    Property<bool>					HighLight;
	Property<std::string>			FocusedActionKey;
	Property<PainterType::Type>     PainterSurfaceType;
	Property<std::string>			Caption;		
	Property<bool>					UseMnemonic;	

public:
	void move(const Pt::Gfx::SizeF& size);	
    bool contains(const Pt::Gfx::PointF& p);

	double toUnit(int v);
	Pt::Gfx::PointF toUnit(const Pt::Gfx::Point& value);
	Pt::Gfx::SizeF toUnit(const Pt::Gfx::Size& value);
	int fromUnit(double v);
	Pt::Gfx::Point fromUnit(const Pt::Gfx::PointF& value);
	Pt::Gfx::Size fromUnit(const Pt::Gfx::SizeF& value);
	double unitSizeInch() const;
	double unitSizeMm() const;
	
	inline PaintSurface* paintSurface()
	{
		return _paintSurface;
	}

protected:	
	inline void setPaintSurface( PaintSurface* surface)
	{
		if(_paintSurface != 0)
			delete _paintSurface;
		
		_paintSurface = surface;
	}

protected:
	PaintSurface* _paintSurface;

private:
	void onPainterTypeChanged(const Property<PainterType::Type>& prop);

public:
	std::string getMnemonicKey() const;
};

}}

#endif
