#ifndef Pt_Hmi_PanelController_H
#define Pt_Hmi_PanelController_H

#include <Pt/Hmi/WidgetController.h>
#include <Pt/Hmi/Event2D.h>

namespace Pt{
namespace Hmi{


class PT_HMI_API PanelController  : public WidgetController
{
public:
	PanelController();
	virtual ~PanelController();	
	
protected:
	virtual void onInput2D(const Event2D& ev);

private:
	void recalcPosSize(const Pt::Gfx::PointF& p);


};

}}

#endif