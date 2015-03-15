#ifndef Pt_Hmi_PaintSurface_h
#define Pt_Hmi_PaintSurface_h

#include <Pt/Hmi/Api.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/ARgbImage.h>
#include <Pt/Hmi/Painter.h>

namespace Pt {
namespace Hmi {

class PaintSurfaceImpl;

class PT_HMI_API PaintSurface
{
public:		
	PaintSurface();
	virtual ~PaintSurface();

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
	PaintSurfaceImpl*	_impl;
	Painter				_painter;
};

}}

#endif
