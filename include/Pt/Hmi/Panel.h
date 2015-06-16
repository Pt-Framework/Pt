#ifndef Pt_Hmi_Panel_H
#define Pt_Hmi_Panel_H

#include <Pt/Hmi/Widget.h>
#include <Pt/Hmi/WindowProperties.h>

namespace Pt{
namespace Hmi{

class PT_HMI_API Panel  : public Widget
{
	public:
		Panel();
		virtual ~Panel();	

	public:
		ValueProperty<BorderStyle::Type>   PanelBorderStyle;
		ValueProperty<bool>				         PanelBorderRoundEdge;	
		ValueProperty<double>			         PanelBorderWidth;	  
		ValueProperty<Ui::Color>					 BorderColor;

	protected:
		virtual void onRender(PaintSurface& paintSurface);
};

}}

#endif