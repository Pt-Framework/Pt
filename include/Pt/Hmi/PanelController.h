#ifndef Pt_Hmi_PanelController_H
#define Pt_Hmi_PanelController_H

#include <Pt/Hmi/WidgetController.h>
#include <Pt/Hmi/PointingEvent.h>

namespace Pt{
namespace Hmi{


class PT_HMI_API PanelController  : public WidgetController
{
public:
	PanelController();
	virtual ~PanelController();	
	
protected:
	virtual void onPointerInput(const PointingEvent& ev);

private:
	enum ResizeDirection
	{
		No,
		North,
		NorthEast,
		East,
		SouthEast,
		South,
		SouthWest,
		West,
		NorthWest
	};		

private:
	void recalcPosAndSize(const Pt::Gfx::PointF& p, ResizeDirection dir);	
	void handleResize(const PointingEvent& ev);	

private:
	Pt::Gfx::PointF _lastSizePoint;
	ResizeDirection _resizeDir;
};

}}

#endif