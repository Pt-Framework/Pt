#ifndef Pt_Hmi_Renderer_TitelBar_H
#define Pt_Hmi_Renderer_TitelBar_H

#include <Pt/Hmi/WidgetRenderer.h>
#include <Pt/Hmi/Model.h>

namespace Pt{
namespace Hmi{

class PT_HMI_API TitleBarRenderer : public WidgetRenderer
{
public:
	TitleBarRenderer();
	virtual ~TitleBarRenderer();

	virtual void render(Pt::Hmi::Model* model);

private:	

};


}}

#endif