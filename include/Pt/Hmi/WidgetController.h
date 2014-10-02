#ifndef Pt_Hmi_Controller_Widget_H
#define Pt_Hmi_Controller_Widget_H

#include <Pt/Hmi/Controller.h>
#include <Pt/Hmi/PointingEvent.h>
#include <Pt/Hmi/WidgetModel.h>
#include <Pt/Hmi/WidgetRenderer.h>

namespace Pt{
namespace Hmi{

class WidgetModel;

class PT_HMI_API WidgetController  : public Controller
{
public:
	WidgetController(WidgetModel& model, WidgetRenderer& renderer);
	virtual ~WidgetController();		

	const WidgetModel& widgetModel() const; 

	WidgetModel& widgetModel();

	virtual void render();
	virtual void output();
	virtual void invalidate();
				
	inline const WidgetController* parent() const
	{
		return dynamic_cast<const WidgetController*>(Controller::widgetParent());
	}

	inline WidgetController* parent()
	{
		return dynamic_cast<WidgetController*>(Controller::widgetParent());
	}

	Pt::Gfx::PointF toClient(const Pt::Gfx::PointF& globalPoint);
	Pt::Gfx::PointF fromClient(const Pt::Gfx::PointF& localPoint, bool toRoot);
	void bindMnemonicToWidget(WidgetController* widget);

	Pt::Signal<WidgetController&, PaintSurface&> Render;
	Pt::Signal<WidgetController&> Output;

protected:
	bool onMoveFocusNext();
	bool onMoveFocusPrev();	
	void onFocusChanged(const Property<bool>& prop);	

private:
	bool focusNextChild(int index);
	bool focusPrevChild(int index);
	int getFocusedChild() const;

	inline WidgetController* childAt(size_t index)
	{
		return dynamic_cast<WidgetController*> (children()[index]);
	}

	inline const WidgetController* childAt(size_t index) const
	{
		return dynamic_cast<const WidgetController*> (children()[index]);
	}


public:
	virtual void onPointerInput(const PointingEvent& ev);
	virtual void onKeyInput(const KeyEvent& ev);
	virtual void onMnemonic();

private:
	WidgetController* _mnemonicWidget;
	WidgetRenderer& _renderer;
};

}}
#endif