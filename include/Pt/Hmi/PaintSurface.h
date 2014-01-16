#ifndef Pt_Hmi_PaintSurface_h
#define Pt_Hmi_PaintSurface_h

#include <Pt/Hmi/Api.h>
#include <Pt/Hmi/Painter.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/ARgbImage.h>

namespace Pt{
namespace Hmi{

class PaintSurfaceImpl;

class PT_HMI_API PaintSurface
{
public:		
	PaintSurface(const Pt::Gfx::SizeF& size);
	virtual ~PaintSurface();

	Pt::Gfx::ARgbImage toImage();
	
	void resize(const Pt::Gfx::SizeF& size);	
	const Pt::Gfx::SizeF& size() const;
	
	inline PaintSurfaceImpl* impl()
	{
		return _impl;
	}

private:
	PaintSurfaceImpl* _impl;
};

}}

#endif
