
#ifndef Pt_Hmi_ButtonRenderer_H
#define Pt_Hmi_ButtonRenderer_H

#include <Pt/Hmi/LabelRenderer.h>
#include <Pt/Hmi/Model.h>

namespace Pt{
namespace Hmi{

class PT_HMI_API ButtonRenderer : public LabelRenderer
{
public:
	ButtonRenderer();
	virtual ~ButtonRenderer();

	virtual void render(Pt::Hmi::Model* model);

private:	

};


}}

#endif