#include "PaintSurfaceImpl.h"
#include "PainterImpl.h"
#include <tchar.h>
#include <Pt/Hmi/Application.h>

namespace Pt{
namespace Hmi{

PaintSurfaceImpl::PaintSurfaceImpl(const Pt::Gfx::SizeF& size)
: _size(size)
{
	createDC();
}


PaintSurfaceImpl::~PaintSurfaceImpl()
{
	freeDC();
}


void PaintSurfaceImpl::freeDC()
{

    HPEN oldPen = (HPEN)SelectObject(_deviceContext, _oldPen);
    DeleteObject(oldPen);

    HPEN oldBrush = (HPEN)SelectObject(_deviceContext, _oldBrush);
    DeleteObject(oldBrush);

    HPEN oldFont = (HPEN)SelectObject(_deviceContext, _oldFont);
    DeleteObject(oldFont);

    // Delete the bitmap/pixmap.
    DeleteObject(_bitmapHandle);

	 DeleteDC(_deviceContext);
}

void PaintSurfaceImpl::createDC()
{
	 _deviceContext = CreateCompatibleDC(CreateDC(_T("DISPLAY"), NULL, NULL, NULL));

	 Pt::Gfx::Size size = Pt::Hmi::Application::instance().fromUnit(_size);

    // Device context of the display. ("DISPLAY" == Predefined Windows device.)
    HDC screenDC = CreateDC(_T("DISPLAY"), NULL, NULL, NULL);

    // Create a Bitmap compatible to the current display.
    _bitmapHandle = CreateCompatibleBitmap(screenDC, size.width(), size.height());

    // Free the screen context.
    DeleteDC(screenDC);

    setupDeviceContext();

}
HBITMAP PaintSurfaceImpl::bitmapHandle() const
{
    return _bitmapHandle;
}


HDC PaintSurfaceImpl::beginPaint()
{
    return _deviceContext;
}


void PaintSurfaceImpl::endPaint()
{
}


HDC PaintSurfaceImpl::deviceContext() const
{
    return _deviceContext;
}


bool PaintSurfaceImpl::isPainting() const
{
    return true;
}

void PaintSurfaceImpl::setupDeviceContext()
{
    _oldPen   = (HPEN)  GetCurrentObject(_deviceContext, OBJ_PEN);
    _oldBrush = (HBRUSH)GetCurrentObject(_deviceContext, OBJ_BRUSH);
    _oldFont  = (HFONT) GetCurrentObject(_deviceContext, OBJ_FONT);

    // Activate the pixmap for the device context.
    SelectObject(_deviceContext, _bitmapHandle);
	SetBkMode(_deviceContext, TRANSPARENT);
}


Pt::Gfx::ARgbImage PaintSurfaceImpl::toImage()
{//TODO:
	Pt::Gfx::Size size = Application::instance().fromUnit(_size);

	Pt::Gfx::ARgbImage	image(size.width(), size.height());
	return image;
}
	
void PaintSurfaceImpl::resize(const Pt::Gfx::SizeF& size)
{
	_size = size;
	freeDC();
	createDC();
}

const Pt::Gfx::SizeF& PaintSurfaceImpl::size() const
{
	return _size;
}

}}

