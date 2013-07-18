#ifndef Pt_Hmi_Renderer_Window_H
#define Pt_Hmi_Renderer_Window_H

#include <Pt/Hmi/Renderer.h>
#include <Pt/Hmi/Model.h>

namespace Pt{
namespace Hmi{

class PT_HMI_API WindowRenderer : public Renderer
{
public:
	WindowRenderer();
	virtual ~WindowRenderer();

	virtual void render(Pt::Hmi::Model* model);

private:	

};

}}

#endif