#ifndef Pt_Hmi_ImagePaintSurface_h
#define Pt_Hmi_ImagePaintSurface_h

#include <Pt/Hmi/Api.h>
#include <Pt/Hmi/PaintSurface.h>
#include <Pt/Ui/Size.h>
#include <Pt/Ui/Image.h>
#include <Pt/Ui/Painter.h>

namespace Pt{
namespace Hmi{

class PaintSurfaceImpl
{
public:		
	PaintSurfaceImpl();
	virtual ~PaintSurfaceImpl();

	virtual void resize(const Ui::SizeF& size);	
	virtual  Ui::SizeF size() const;

	Ui::Image& image()
	{
		return *_image; 
	}

private:
	Ui::Image* _image;	
};

}}

#endif
