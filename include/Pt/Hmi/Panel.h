#ifndef Pt_Hmi_Panel_H
#define Pt_Hmi_Panel_H

#include <Pt/Hmi/Widget.h>
#include <Pt/Hmi/PointingEvent.h>

namespace Pt{
namespace Hmi{

class PT_HMI_API Panel  : public Widget
{
public:
	Panel();
	virtual ~Panel();	

public:
	Property<BorderStyle::Type>    PanelBorderStyle;
	Property<bool>				         PanelBorderRoundEdge;	
	Property<double>			         PanelBorderWidth;	  
  Property<Pt::Gfx::ARgbColor>	 BorderColor;

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
};

}}

#endif