#ifndef Pt_Hmi_Controller_Widget_H
#define Pt_Hmi_Controller_Widget_H

#include <Pt/Hmi/PointingEvent.h>
#include <Pt/Hmi/PaintSurface.h>
#include <Pt/Hmi/KeyEvent.h>
#include <Pt/Hmi/Property.h>
#include <Pt/Hmi/Cursor.h>
#include <Pt/Hmi/Margin.h>
#include <Pt/Connectable.h>
#include <Pt/Gfx/Font.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/ARgbColor.h>

namespace Pt{
namespace Hmi{
  
namespace BorderStyle
{
  enum Type
  {
    NoBorder,
    Single,
    Border3D,
    Sizeable,
    Widget
  };
}

namespace ImageLayout
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

namespace Align
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
		BottomRight
	};
}

namespace Docking
{
	enum Type
	{
    None,
		Left,
		Top,
		Right,
		Bottom,
		Fill
	};
}

namespace FlowLayout
{
  enum  Type
  {
    None,
    Vertical,
    Horizontal
  };
}

namespace FlowLayoutDirection
{
  enum  Type
  {
    LeftToRightTopToBottom,
    RightToLeftBottomToTop
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

	const Widget* parent() const
	{
		return _parent;
	}

	Widget* parent()
	{
		return _parent;
	}

	void setParent(Widget* parent)
	{
		_parent = parent;
	}

	void bindMnemonicToWidget(Widget& widget);	

	PaintSurface& paintSurface()
	{
		return _paintSurface;
	}

	bool contains(const Pt::Gfx::PointF& p);
 	
  bool focusNext();
	
  bool focusPrev();		

	void invalidate();

  void render();

  void mnemonic();

	Pt::Gfx::PointF toClient(const Pt::Gfx::PointF& globalPoint);

	Pt::Gfx::PointF fromClient(const Pt::Gfx::PointF& localPoint, bool toRoot);

public:
	static int getMnemonicIndex(const std::string& text);

	static std::string removeMnemonic(const std::string& text);

public:
	Property<bool>									Enabled;		
	Property<bool>									Visible;
	Property<Pt::Gfx::Font>					Font;
	Property<Pt::Gfx::PointF>				Position;
	Property<Pt::Gfx::SizeF>				Size;
	Property<Pt::Gfx::ARgbColor>		BackColor;
  Property<Pt::Gfx::ARgbColor>		HighlightColor;
	Property<Pt::Gfx::ARgbColor>		ForeColor;
  Property<Pt::Gfx::ARgbColor>		DisabledColor;
	Property<Pt::Gfx::ARgbImage>		BackgroundImage;
	Property<ImageLayout::Type>	    BackgroundImageLayout;
	Property<int>										Opacity;
	Property<Cursor>                CursorT;
	Property<Align::Type>		        TextAlign;
	Property<bool>									Focused; 
	Property<bool>									AcceptFocus;
  Property<bool>									HighLight;
	Property<std::string>						FocusedActionKey;
	Property<std::string>						Caption;		
	Property<bool>									UseMnemonic;	
  Property<std::string>						Name;		
  Property<Pt::Hmi::Margin>       Margin;
  Property<Docking::Type>         Dock;
  Property<FlowLayout::Type>	    FlowLayout;	
  Property<FlowLayoutDirection::Type> FlowDirection;

protected:
  Widget();	

protected:
  virtual void onInvalidate();
	virtual void onRender();
  virtual void onLayout();
	virtual void onPointerInput(const PointingEvent& ev);
	virtual void onKeyInput(const KeyEvent& ev);
	virtual void onMnemonic();

private:
  void onSizeChanged(const Property<Pt::Gfx::SizeF>& prop);
  void onFocusChanged(const Property<bool>& prop);
  void onCaptionChanged(const Property<std::string>& prop);

private:			  
  bool focusNextChild(int index);
	bool focusPrevChild(int index);
	int getFocusedChild() const;	
  static void updatePosSizeNoInval(Widget& w, const Pt::Gfx::SizeF& s, const Pt::Gfx::PointF& p);

private:	  
	Widget*								_parent;	
	std::vector<Widget*>  _children;	
	Widget*								_mnemonicWidget;	
	PaintSurface					_paintSurface;	
  std::string           _mnemonicKey;
};

}}

#endif
