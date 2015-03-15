#ifndef Pt_Hmi_Controller_Widget_H
#define Pt_Hmi_Controller_Widget_H

#include <Pt/Hmi/Controller.h>
#include <Pt/Hmi/PointingEvent.h>
#include <Pt/Hmi/WidgetModel.h>
#include <Pt/Hmi/WidgetView.h>

namespace Pt{
namespace Hmi{

class WidgetModel;

class PT_HMI_API Widget  : public Controller
{
public:
	Widget(WidgetModel& model, WidgetView& view);
	virtual ~Widget();		

	const WidgetModel& widgetModel() const; 

	WidgetModel& widgetModel();

	const WidgetView& widgetView() const
	{
		return _view;
	}

	WidgetView& widgetView()
	{
		return _view;
	}
	
	void invalidate();	
				
	inline const Widget* parent() const
	{
		return dynamic_cast<const Widget*>(Controller::widgetParent());
	}

	inline Widget* parent()
	{
		return dynamic_cast<Widget*>(Controller::widgetParent());
	}

	Pt::Gfx::PointF toClient(const Pt::Gfx::PointF& globalPoint);
	Pt::Gfx::PointF fromClient(const Pt::Gfx::PointF& localPoint, bool toRoot);
	void bindMnemonicToWidget(Widget* widget);

	Pt::Signal<Widget&, PaintSurface&> Render;
	Pt::Signal<Widget&> Output;

protected:
	virtual void output();	
	bool onMoveFocusNext();
	bool onMoveFocusPrev();	
	void onFocusChanged(const Property<bool>& prop);	

private:
	bool focusNextChild(int index);
	bool focusPrevChild(int index);
	int getFocusedChild() const;

	inline Widget* childAt(size_t index)
	{
		return dynamic_cast<Widget*> (children()[index]);
	}

	inline const Widget* childAt(size_t index) const
	{
		return dynamic_cast<const Widget*> (children()[index]);
	}


public:
	virtual void onPointerInput(const PointingEvent& ev);
	virtual void onKeyInput(const KeyEvent& ev);
	virtual void onMnemonic();

private:
	Widget* _mnemonicWidget;
	WidgetView& _view;
};

}}
#endif