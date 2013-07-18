#ifndef Pt_Hmi_Output_Gfx_H
#define Pt_Hmi_Output_Gfx_H

#include <Pt/Hmi/Model.h>
#include <Pt/Hmi/OutputDevice.h>

namespace Pt{
namespace Hmi{

class GfxOutputImpl;

class PT_HMI_API GfxOutput : public OutputDevice
{
public:
	GfxOutput();
	virtual ~GfxOutput();	

	virtual void output(Pt::Hmi::Model* model);
	
protected:
    virtual void onCancel()
	{
	}

    virtual bool onRun()
	{
		return true;
	}
	
	Pt::Gfx::Painter* nativePainter();
	
private:
	GfxOutputImpl* _impl;
};

}}}
#endif