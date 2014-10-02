#ifndef Pt_Hmi_PaintSurface_h
#define Pt_Hmi_PaintSurface_h

#include <Pt/Hmi/Api.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/ARgbImage.h>

namespace Pt {
namespace Hmi {

class Painter;

class PT_HMI_API PaintSurface
{
public:		
	PaintSurface()
	{
	}
	
	virtual ~PaintSurface()
	{
	}

	virtual Pt::Gfx::ARgbImage toImage() const = 0;
	virtual void resize(const Pt::Gfx::SizeF& size)= 0;	
	virtual Pt::Gfx::SizeF size() const = 0;
	virtual Pt::Hmi::Painter& painter() = 0;
};

}}

#endif
