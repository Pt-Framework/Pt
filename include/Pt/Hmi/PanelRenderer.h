#ifndef Pt_Hmi_Renderer_Frame_H
#define Pt_Hmi_Renderer_Frame_H

#include <Pt/Hmi/WidgetRenderer.h>
#include <Pt/Hmi/Model.h>

namespace Pt{
namespace Hmi{

class PT_HMI_API PanelRenderer : public WidgetRenderer
{
public:
	PanelRenderer();
	virtual ~PanelRenderer();

	virtual void render(Pt::Hmi::Model* model ,Pt::Gfx::Painter* Painter);

private:	

};


}}

#endif