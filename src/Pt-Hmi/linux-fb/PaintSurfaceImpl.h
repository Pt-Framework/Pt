#ifndef Pt_Hmi_ImagePaintSurface_h
#define Pt_Hmi_ImagePaintSurface_h

#include <Pt/Hmi/Api.h>
#include <Pt/Hmi/PaintSurface.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/ARgbImage.h>
#include <Pt/Gfx/Painter.h>

namespace Pt{
namespace Hmi{

class PaintSurfaceImpl
{
public:		
	PaintSurfaceImpl();
	virtual ~PaintSurfaceImpl();

	virtual Pt::Gfx::ARgbImage toImage() const
	{
		return _image;
	}

	virtual void resize(const Pt::Gfx::SizeF& size);	
	virtual  Pt::Gfx::SizeF size() const;

	Pt::Gfx::ARgbImage& image()
	{
		return _image; 
	}

private:
	Pt::Gfx::ARgbImage _image;	
};

}}

#endif
