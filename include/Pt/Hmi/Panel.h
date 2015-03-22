#ifndef Pt_Hmi_Panel_H
#define Pt_Hmi_Panel_H

#include <Pt/Hmi/Widget.h>
#include <Pt/Hmi/PointingEvent.h>
#include <Pt/Hmi/PanelModel.h>

namespace Pt{
namespace Hmi{


class PT_HMI_API Panel  : public Widget
{
public:

	Panel(PanelModel* model);
	virtual ~Panel();	
	
	const PanelModel* panelModel() const 
	{
		return _panelModel;
	}

	PanelModel* panelModel()
	{
		return _panelModel;
	}

protected:
	virtual void onPointerInput(const PointingEvent& ev);
	virtual void onRender();

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
	PanelModel*     _panelModel;
};

}}

#endif