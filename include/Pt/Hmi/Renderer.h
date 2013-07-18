#ifndef Pt_Hmi_Renderer_Base_H
#define Pt_Hmi_Renderer_Base_H

#include <Pt/Gfx/Painter.h>
#include <Pt/Hmi/Model.h>
#include <Pt/Hmi/OutputDevice.h>
#include <Pt/Hmi/Api.h>

namespace Pt{
namespace Hmi{

class PT_HMI_API Renderer
{
public:
	virtual ~Renderer()
	{ }

	virtual void render(Pt::Hmi::Model* model ,Pt::Gfx::Painter* Painter) = 0;

protected:
	Renderer()
	{ }
};

}}

#endif