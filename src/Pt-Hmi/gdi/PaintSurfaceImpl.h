#ifndef Pt_Hmi_PaintSurfaceImpl_h
#define Pt_Hmi_PaintSurfaceImpl_h

#include <Pt/Hmi/WidgetModel.h>
#include <Pt/Hmi/Property.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/ARgbImage.h>

#include <Windows.h>

namespace Pt{
namespace Hmi{

class PainterImpl;

class PaintSurfaceImpl
{
public:		
	PaintSurfaceImpl(const Pt::Gfx::SizeF& size);
	virtual ~PaintSurfaceImpl();

	Pt::Gfx::ARgbImage toImage();
	
	void resize(const Pt::Gfx::SizeF& size);	
	const Pt::Gfx::SizeF& size() const;

    HBITMAP bitmapHandle() const;

    HDC beginPaint();

    void endPaint();

    HDC deviceContext() const;

    bool isPainting() const;

private:
	void setupDeviceContext();
	void createDC();
	void freeDC();

private:
    Gfx::SizeF    _size;
    HDC           _deviceContext;
    HBITMAP       _bitmapHandle;            
    HPEN		  _oldPen;
    HBRUSH		  _oldBrush;
	HFONT	      _oldFont;
};

}}

#endif
