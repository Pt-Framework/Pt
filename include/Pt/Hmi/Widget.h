#ifndef Pt_Hmi_Controller_Widget_H
#define Pt_Hmi_Controller_Widget_H

#include <Pt/Hmi/PointingEvent.h>
#include <Pt/Hmi/PaintSurface.h>
#include <Pt/Hmi/KeyEvent.h>
#include <Pt/Hmi/Property.h>
#include <Pt/Hmi/Cursor.h>
#include <Pt/Connectable.h>
#include <Pt/Gfx/Font.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/ARgbColor.h>

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


class PT_HMI_API Widget : public Pt::Connectable
{
public:	
	
	virtual ~Widget();		
				
	void addChild(Widget* child);

	void removeChild(Widget* child);

	const std::vector<Widget*>& children() const
	{
		return _children;
	}

	inline const Widget* parent() const
	{
		return _parent;
	}

	inline Widget* parent()
	{
		return _parent;
	}

	inline void setParent(Widget* parent)
	{
		_parent = parent;
	}

	void bindMnemonicToWidget(Widget* widget);

	inline PaintSurface& paintSurface()
	{
		return _paintSurface;
	}

	void pointerInput(const PointingEvent& ev)
	{
		onPointerInput(ev);
	}

	void keyInput(const KeyEvent& ev)
	{
		onKeyInput(ev);
	}

	void mnemonic()
	{
		onMnemonic();
	}

	void invalidate();	

	void render();

	Pt::Gfx::PointF toClient(const Pt::Gfx::PointF& globalPoint);

	Pt::Gfx::PointF fromClient(const Pt::Gfx::PointF& localPoint, bool toRoot);

protected:
	Widget();

public:
	Property<bool>									Enabled;		
	Property<bool>									Visible;
	Property<Pt::Gfx::Font>					Font;
	Property<Pt::Gfx::PointF>				Position;
	Property<Pt::Gfx::SizeF>				Size;
	Property<Pt::Gfx::ARgbColor>		BackColor;
  Property<Pt::Gfx::ARgbColor>		BackColorHightLight;
	Property<Pt::Gfx::ARgbColor>		ForeColor;
	Property<Pt::Gfx::ARgbImage>		BackgroundImage;
	Property<ImageLayoutType::Type>	BackgroundImageLayout;
	Property<int>										Opacity;
	Property<Cursor>                CursorT;
	Property<TextAlignType::Type>		TextAlign;
	Property<bool>									Focused; 
	Property<bool>									AcceptFocus;
  Property<bool>									HighLight;
	Property<std::string>						FocusedActionKey;
	Property<std::string>						Caption;		
	Property<bool>									UseMnemonic;	
	
protected:
	virtual void onPointerInput(const PointingEvent& ev);
	virtual void onKeyInput(const KeyEvent& ev);
	virtual void onMnemonic();
	virtual void onRender();
	virtual void onInvalidate();

protected:
	bool contains(const Pt::Gfx::PointF& p);

private:		
	void onFocusChanged(const Property<bool>& prop);		
	int getFocusedChild() const;
	std::string getMnemonicKey() const;

private:	
	Widget*								_parent;	
	std::vector<Widget*>  _children;	
	Widget*								_mnemonicWidget;	
	PaintSurface					_paintSurface;	
};

}}
#endif
