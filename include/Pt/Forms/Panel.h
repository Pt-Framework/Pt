#ifndef Pt_Hmi_Desktop_Panel_H
#define Pt_Hmi_Desktop_Panel_H

#include <Pt/Hmi/Desktop/Widget.h>
#include <Pt/Hmi/PanelController.h>
#include <Pt/Hmi/PanelModel.h>
#include <Pt/Hmi/PanelRenderer.h>

namespace Pt{
namespace Hmi{
namespace Desktop{

class PT_HMI_DESKTOP_API Panel : public Widget 
{
public:
	Panel();
	virtual ~Panel();
	
	virtual void setSize(const Pt::Gfx::SizeF& size);
	virtual const Pt::Gfx::SizeF& size() const;

    virtual void setPosition(const Pt::Gfx::PointF& position);
	virtual const Pt::Gfx::PointF& position() const;
	
	Pt::Hmi::PanelController&	panelController();
	Pt::Hmi::PanelModel&		panelModel();
	void setPanelController(Pt::Hmi::PanelController& ctrl);

	const Pt::Hmi::PanelController&	panelController() const;
	const Pt::Hmi::PanelModel&		panelModel() const;

protected:

	virtual WidgetController& widgetController()
	{
		return *_currController;
	}

private:
	Pt::Hmi::PanelModel			_defModel;
	Pt::Hmi::PanelRenderer		_defRenderer;
	Pt::Hmi::PanelController	_defController;
	Pt::Hmi::PanelController*	_currController;
};
 
}}}
#endif