#ifndef Pt_Hmi_LabelRenderer_H
#define Pt_Hmi_LabelRenderer_H

#include <Pt/Hmi/PanelRenderer.h>
#include <Pt/Hmi/Model.h>

namespace Pt{
namespace Hmi{

class PT_HMI_API LabelRenderer : public PanelRenderer
{
public:
	LabelRenderer();
	virtual ~LabelRenderer();

	virtual void render(Pt::Hmi::Model* model ,Pt::Gfx::Painter* Painter);

private:	

};


}}

#endif