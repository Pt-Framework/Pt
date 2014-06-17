#ifndef Pt_Hmi_ImagePaintSurface_h
#define Pt_Hmi_ImagePaintSurface_h

#include <Pt/Hmi/Api.h>
#include <Pt/Hmi/PaintSurface.h>
#include <Pt/Hmi/ImagePainter.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/ARgbImage.h>
#include <Pt/Gfx/Painter.h>

namespace Pt{
namespace Hmi{

class PT_HMI_API ImagePaintSurface : public PaintSurface
{
public:		
	ImagePaintSurface(const Pt::Gfx::SizeF& size);
	virtual ~ImagePaintSurface();

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

	Pt::Hmi::Painter& painter()
	{
		return _painter;
	}

private:
	Pt::Gfx::ARgbImage _image;
	ImagePainter _painter;
};

}}

#endif
