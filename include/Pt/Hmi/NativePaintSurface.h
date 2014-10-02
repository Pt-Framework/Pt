#ifndef Pt_Hmi_NativePaintSurface_h
#define Pt_Hmi_NativePaintSurface_h

#include <Pt/Hmi/Api.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/ARgbImage.h>
#include <Pt/Hmi/PaintSurface.h>
#include <Pt/Hmi/NativePainter.h>

namespace Pt {
namespace Hmi {

class PaintSurfaceImpl;

class PT_HMI_API NativePaintSurface : public PaintSurface
{
public:		
	NativePaintSurface(const Pt::Gfx::SizeF& size);
	virtual ~NativePaintSurface();

	virtual Pt::Gfx::ARgbImage toImage() const;
	virtual void resize(const Pt::Gfx::SizeF& size);	
	virtual Pt::Gfx::SizeF size() const;

	virtual Pt::Hmi::Painter& painter()
	{
		return _painter;
	}

	inline PaintSurfaceImpl* impl()
	{
		return _impl;
	}

private:
	PaintSurfaceImpl* _impl;
	NativePainter _painter;
};

}}

#endif
