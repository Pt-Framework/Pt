#ifndef Pt_Hmi_PaintSurface_h
#define Pt_Hmi_PaintSurface_h

#include <Pt/Hmi/Api.h>
#include <Pt/Hmi/Painter.h>
#include <Pt/Ui/Size.h>
#include <Pt/Ui/Image.h>

namespace Pt {
namespace Hmi {

class PaintSurfaceImpl;

class PT_HMI_API PaintSurface
{
	public:		
		PaintSurface();
		virtual ~PaintSurface();

		virtual void resize(const Ui::SizeF& size);
		virtual Ui::SizeF size() const;

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
