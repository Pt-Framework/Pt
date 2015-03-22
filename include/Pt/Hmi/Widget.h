#ifndef Pt_Hmi_Controller_Widget_H
#define Pt_Hmi_Controller_Widget_H

#include <Pt/Hmi/PointingEvent.h>
#include <Pt/Hmi/WidgetModel.h>
#include <Pt/Connectable.h>

namespace Pt{
namespace Hmi{

class WidgetModel;

class PT_HMI_API Widget : public Pt::Connectable
{
public:
	Widget(WidgetModel* model);
	
	virtual ~Widget();		

	const WidgetModel* widgetModel() const
	{
		return _widgetModel;
	}

	WidgetModel* widgetModel()
	{
		return _widgetModel;
	}
		
				
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
	virtual void onFocusChanged(const Property<bool>& prop);	
	virtual void onPointerInput(const PointingEvent& ev);
	virtual void onKeyInput(const KeyEvent& ev);
	virtual void onMnemonic();
	virtual void onRender();
	virtual void onInvalidate();

	int getFocusedChild() const;

private:
	Widget*								_mnemonicWidget;	
	PaintSurface					_paintSurface;
	std::vector<Widget*>  _children;
	Widget*								_parent;
	WidgetModel*  				_widgetModel;	
};

}}
#endif
