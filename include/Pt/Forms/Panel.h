#ifndef Pt_Forms_Panel_H
#define Pt_Forms_Panel_H

#include <Pt/Forms/Widget.h>
#include <Pt/Hmi/Panel.h>
#include <Pt/Hmi/PanelModel.h>
#include <Pt/Hmi/PanelView.h>

namespace Pt {
namespace Forms {

class PT_FORMS_API Panel : public Widget 
{
public:
	Panel();
	virtual ~Panel();
	
	virtual void setSize(const Pt::Gfx::SizeF& size);
	virtual const Pt::Gfx::SizeF& size() const;

    virtual void setPosition(const Pt::Gfx::PointF& position);
	virtual const Pt::Gfx::PointF& position() const;
	
	Pt::Hmi::Panel&	panel();
	Pt::Hmi::PanelModel&		panelModel();
	void setPanel(Pt::Hmi::Panel& ctrl);

	const Pt::Hmi::Panel&	panel() const;
	const Pt::Hmi::PanelModel&		panelModel() const;

protected:

	virtual Hmi::Widget& widgetController()
	{
		return *_currController;
	}

private:
	Pt::Hmi::PanelModel		_defModel;
	Pt::Hmi::PanelView		_defView;
	Pt::Hmi::Panel				_defController;
	Pt::Hmi::Panel*	_currController;
};
 
}}
#endif
