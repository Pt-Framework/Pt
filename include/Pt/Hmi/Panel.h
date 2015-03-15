#ifndef Pt_Hmi_Panel_H
#define Pt_Hmi_Panel_H

#include <Pt/Hmi/Widget.h>
#include <Pt/Hmi/PointingEvent.h>
#include <Pt/Hmi/PanelModel.h>

namespace Pt{
namespace Hmi{

class PanelView;

class PT_HMI_API Panel  : public Widget
{
public:

	Panel(PanelModel& model, PanelView& view);
	virtual ~Panel();	
	
	const PanelModel& panelModel() const 
	{
		return static_cast<const PanelModel&>(model());
	}

	PanelModel& panelModel()
	{
		return static_cast<PanelModel&>(model());
	}

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