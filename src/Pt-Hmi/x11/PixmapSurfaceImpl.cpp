/*
 * Copyright (C) 2017 Marc Boris Duerner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
 * MA 02110-1301 USA
 */

#include "PainterImpl.h"
#include "PaintSurfaceImpl.h"
#include "PixmapSurfaceImpl.h"
#include "ApplicationImpl.h"
#include "PictureImpl.h"

#include <Pt/Hmi/Painter.h>
#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/PixmapSurface.h>
#include <Pt/Gfx/Algorithm.h>
#include <Pt/Byteorder.h>

#ifndef _AIX
#include <X11/Xft/Xft.h>
#endif

namespace Pt { 

namespace Hmi { 

PixmapSurfaceImpl::PixmapSurfaceImpl()
: _size(10, 10)
, _painter(0)
, _drawable(0)
, _xftDraw(0)
{
	create(_size);    
}


PixmapSurfaceImpl::~PixmapSurfaceImpl()
{
    destroy();
}


void PixmapSurfaceImpl::create(const Pt::Gfx::SizeF& size)
{
    _size = size;

    Display* display = Application::instance().impl()->display();
    int depth = Application::instance().impl()->depth();
    
    int width = lround( size.width() );
    if(width <= 0) 
        width = 1;
    
    int height = lround( size.height() );
    if(height <= 0)
        height = 1;

    _drawable = XCreatePixmap(display, XDefaultRootWindow(display), 
                              width, height, depth);

#ifndef _AIX
    ::Visual* visual = Application::instance().impl()->visual();
    unsigned int screen = XDefaultScreen(display);
    Colormap colorMap = XDefaultColormap(display, screen);
    _xftDraw = XftDrawCreate(display, _drawable, visual, colorMap);
#endif  

    XSync(display, False);
}


void PixmapSurfaceImpl::destroy()
{
    Display* display = Application::instance().impl()->display();
    
#ifndef _AIX
    XftDrawDestroy(_xftDraw);
    _xftDraw = 0;
#endif

    XFreePixmap(display, _drawable);
    
    _drawable = 0;
    _size = Gfx::SizeF(0, 0);
}


void PixmapSurfaceImpl::clear(const Gfx::Color& c)
{
}


void PixmapSurfaceImpl::resize(const Pt::Gfx::SizeF& size)
{ 
    if( _size == size )
        return;

   destroy();
   create(size);
}


const Pt::Gfx::SizeF& PixmapSurfaceImpl::size() const
{
    return _size;
}


Gfx::Image PixmapSurfaceImpl::getImage() const
{
    int width = lround( _size.width() );
    int height = lround( _size.height() );

    Display* display = Application::instance().impl()->display();

    XImage* ximage = XGetImage( display, _drawable,
                                0, 0, width, height,
                                AllPlanes, ZPixmap );

    if( ! ximage )
        return Gfx::Image();

    Gfx::Image image;
    const Gfx::ImageFormat* format = 0;
    
    if(ximage->depth == 24 ||  ximage->depth == 32)
        format = &Gfx::ImageFormat::argb32();
    else if(ximage->depth == 16)
        format = &Gfx::ImageFormat::rgb16();

    if( format )
    { 
        if( (ximage->byte_order == LSBFirst && isLittleEndian()) ||
            (ximage->byte_order == MSBFirst && isBigEndian()) )
        {
            Pt::size_t padding = ximage->bytes_per_line - (width * ximage->bits_per_pixel / 8);
            image = Gfx::Image(*format,
                               reinterpret_cast<Pt::uint8_t*>(ximage->data),
                               Gfx::Size(width, height), padding);
        }
        else 
        {
            image = Gfx::Image(*format, Gfx::Size(width, height));
            for(std::size_t y = 0; y < ximage->height; ++y)
            {
                for(std::size_t x = 0; x < ximage->width; ++x )
                {
                    unsigned long pixel = XGetPixel(ximage, x, y);
                    Gfx::Image::PixelIterator it  = image.pixel(x,y);
                    std::memcpy(it->base(), &pixel, image.format().pixelStride());
                } 
            }
        }
    }

    XDestroyImage(ximage);

    return image;
}


void PixmapSurfaceImpl::begin(Painter& painter)
{
    _painter = &painter;
}


void PixmapSurfaceImpl::finish()
{
    _painter = 0;
}


void PixmapSurfaceImpl::setClip(const Gfx::RectF& rectF)
{
    Gfx::Rect rect = round(rectF);

    XRectangle xrect;
    xrect.x      = rect.x();
    xrect.y      = rect.y();
    xrect.width  = rect.width();
    xrect.height = rect.height();

    XftDrawSetClipRectangles(_xftDraw, 0, 0, &xrect, 1);

    _painter->impl()->setClip(rectF);
}


void PixmapSurfaceImpl::resetClip()
{
    XftDrawSetClipRectangles(_xftDraw, 0, 0, 0, 0);

    _painter->impl()->resetClip();
}


const Gfx::ImageFormat& PixmapSurfaceImpl::format() const
{
    return Gfx::ImageFormat::argb32();
}


void PixmapSurfaceImpl::setCompositionMode(const Gfx::CompositionMode& mode)
{
}


void PixmapSurfaceImpl::setPen(const Gfx::Pen& pen)
{
}


void PixmapSurfaceImpl::setBrush(const Gfx::Brush& brush)
{
}


void PixmapSurfaceImpl::setFont(const Gfx::Font& font)
{
}


Gfx::FontMetrics PixmapSurfaceImpl::fontMetrics(const Pt::String& text) const
{
    if( ! _painter )
        return Gfx::FontMetrics(0, 0, 0, 0);

#ifndef _AIX
    _XftFont* font = _painter->impl()->font();
    if( ! font )
        return Gfx::FontMetrics(0, 0, 0, 0);
    
    Display* display = Application::instance().impl()->display();
    
    XGlyphInfo info;
    XftTextExtents32(display, font, (XftChar32*)text.c_str(), text.size(), &info);

    return Gfx::FontMetrics(font->ascent, font->descent, info.width, font->height);
#else
    return Gfx::FontMetrics(0, 0, 0, 0);
#endif     
}


void PixmapSurfaceImpl::drawLine(const Gfx::PointF& from, const Gfx::PointF& to)
{
    if( ! _painter ) 
        return;   

    int fromX = lround(from.x());
    int fromY = lround(from.y());
    int toX   = lround(to.x());
    int toY   = lround(to.y());

    GC& penGc = _painter->impl()->pen();

    Display* display = Application::instance().impl()->display();
    XDrawLine(display, _drawable, penGc, fromX, fromY, toX, toY); 
}


void PixmapSurfaceImpl::drawText(const Gfx::PointF& to, const Pt::String& text)
{
    if( ! _painter ) 
        return; 

    int toX = lround( to.x() );
    int toY = lround( to.y() );

    Gfx::Color penColor = _painter->pen().color();
#ifndef _AIX
    XftColor xftColor;
    xftColor.pixel = 0; // this would be input for XftColorAllocValue
    xftColor.color.red   = penColor.red();
    xftColor.color.green = penColor.green();
    xftColor.color.blue  = penColor.blue();
    xftColor.color.alpha = 0xffff;

    XftFont* font = _painter->impl()->font();

    XftDrawString32(_xftDraw, &xftColor, font, toX, toY,
                    (XftChar32*) text.c_str(), text.size());
#endif

    //XSync(display, false);    
}


void PixmapSurfaceImpl::drawRect(const Gfx::RectF& rectF)
{
    if( ! _painter ) 
        return; 

    Pt::Gfx::Rect rect = Gfx::round(rectF);

    if( rect.width() == 0 || rect.height() == 0 ) 
        return;

    Display* display = Application::instance().impl()->display();
    GC& penGc = _painter->impl()->pen();

    XDrawRectangle(display, _drawable, penGc, 
                   rect.x(), rect.y(), rect.width() - 1, rect.height() - 1);    
}


void PixmapSurfaceImpl::fillRect(const Gfx::RectF& rectF)
{    
    if( ! _painter ) 
        return; 

    Pt::Gfx::Rect rect = Gfx::round(rectF);

    if( rect.width() == 0 || rect.height() == 0 ) 
        return;

    Display* display = Application::instance().impl()->display();
    GC& brushGc = _painter->impl()->brush();

    XFillRectangle(display, _drawable, brushGc, 
                   rect.x(), rect.y(), rect.width(), rect.height());

    XSync(display, False);
}


void PixmapSurfaceImpl::drawEllipse(const Pt::Gfx::PointF& topLeftF, 
                              const Pt::Gfx::SizeF& sizeF)
{
    if( ! _painter ) 
        return; 

    Pt::Gfx::Point topLeft = Gfx::round(topLeftF);
    Pt::Gfx::Size size = Gfx::round(sizeF);

    if (size.width() == 0 || size.height() == 0) 
        return;

    Display* display = Application::instance().impl()->display();
    GC& penGc = _painter->impl()->pen();

    XDrawArc(display, _drawable, penGc, 
             topLeft.x(), topLeft.y(), 
             size.width() - 1, size.height() - 1, 
             0, 360*64);    
}


void PixmapSurfaceImpl::fillEllipse(const Pt::Gfx::PointF& topLeftF, const Pt::Gfx::SizeF& sizeF)
{
    if( ! _painter ) 
        return; 

    Pt::Gfx::Point topLeft = Gfx::round(topLeftF);
    Pt::Gfx::Size size = Gfx::round(sizeF);

    if (size.width() == 0 || size.height() == 0) 
           return;

    Display* display = Application::instance().impl()->display();
    GC& brushGc = _painter->impl()->brush();

    XFillArc(display, _drawable, brushGc, 
             topLeft.x(), topLeft.y(), 
             size.width(), size.height(), 
             0, 360 * 64);    
}


void PixmapSurfaceImpl::drawPolyline(const Pt::Gfx::PointF* points, size_t pointCount)
{
    if( ! _painter ) 
        return; 

    Display* display = Application::instance().impl()->display();
    GC& penGc = _painter->impl()->pen();

    XPoint xpoints[pointCount];
    
    for(size_t n = 0; n < pointCount; ++n)
    {
        Pt::Gfx::Point point = Gfx::round( points[n] );

        xpoints[n].x = point.x();
        xpoints[n].y = point.y();
    }

    XDrawLines(display, _drawable, penGc, 
            xpoints, pointCount, CoordModeOrigin);    
}


void PixmapSurfaceImpl::fillPolygon(const Pt::Gfx::PointF* points, size_t pointCount)
{
    if( ! _painter ) 
        return; 

    Display* display = Application::instance().impl()->display();
    GC& brushGc = _painter->impl()->brush();

    XPoint xpoints[pointCount];

    for(size_t n = 0; n < pointCount; ++n) 
    {
        Pt::Gfx::Point point = Gfx::round( points[n] );
        
        xpoints[n].x = point.x();
        xpoints[n].y = point.y();
    }

    XFillPolygon(display, _drawable, brushGc, 
                 xpoints, pointCount, Complex, CoordModeOrigin);    
}


void PixmapSurfaceImpl::drawSurface(const Pt::Gfx::PointF& toF, 
                                    const PixmapSurface& pm)
{
    if( ! _painter ) 
        return; 

    Display* display = Application::instance().impl()->display();
    GC& brushGc = _painter->impl()->brush();
    ::Drawable from = pm.pixmapImpl()->drawable();

    Pt::Gfx::Size size = Gfx::round(pm.size());
    Pt::Gfx::Point to =  Gfx::round(toF);

    XCopyArea(display, from, _drawable, brushGc, 
            0, 0, size.width(), size.height(), to.x(), to.y() );
    
    //XSync(display, false);
}


void PixmapSurfaceImpl::drawSurface(const Pt::Gfx::PointF& toF, 
                                    const PixmapSurface& pm, 
                                    const Gfx::RectF& pmRect)
{
    if( ! _painter )
        return; 

    Display* display = Application::instance().impl()->display();
    GC& brushGc = _painter->impl()->brush();
	::Drawable source = pm.pixmapImpl()->drawable();

	Pt::Gfx::Size size =  Gfx::round(pm.size());
	Pt::Gfx::Point to =  Gfx::round(toF);
    const Gfx::Point from = Gfx::round( pmRect.topLeft() );

	XCopyArea(display, source, _drawable, brushGc, 
              from.x(), from.y(), 
              size.width(), size.height(), 
              to.x(), to.y() );
	
    //XSync(display, false);        
}


void PixmapSurfaceImpl::drawImage(const Gfx::PointF& toF, const Gfx::Image& image)
{
    if( ! _painter ) 
        return; 

    Pt::Gfx::RectF imageRect( Pt::Gfx::PointF(0, 0),
                              Pt::Gfx::SizeF(image.width(), image.height()) );
    
    drawImage(toF, image, imageRect);
}


void PixmapSurfaceImpl::drawImage(const Gfx::PointF& toF, 
                                  const Gfx::Image& image, 
                                  const Gfx::RectF& imgRectF)
{
    if( ! _painter ) 
        return; 

    Display* display = Application::instance().impl()->display();
    ::Visual* visual = Application::instance().impl()->visual();
    int depth = Application::instance().impl()->depth();

    //std::clog << "XCreateImage" << std::endl;
    XImage* ximage = XCreateImage(display, visual, depth, ZPixmap, 0, 
                                  (char*)image.data(), 
                                  image.width(), image.height(), 
                                  32, 0);
    
    Gfx::Point to = Gfx::round(toF);
    Gfx::Rect imageRect = Gfx::round(imgRectF);
    GC& brushGc = _painter->impl()->brush();

    //std::clog << "XPutImage" << to.x() << " " << to.y() << std::endl;
    XPutImage( display, _drawable, brushGc, ximage, 
               imageRect.x(), imageRect.y(), to.x(), to.y(), 
               imageRect.width(), imageRect.height() );
    
    ximage->data = NULL;

    //std::clog << "XDestroyImage" << std::endl;
    XDestroyImage(ximage); 
}

void PixmapSurfaceImpl::drawPicture(const Gfx::PointF& toF, const Picture& pic)
{
    const PictureImpl* picImpl = pic.impl();

    //Gfx::Point to = Gfx::round(toF);

    if( picImpl->empty() )
      return;
}

} // namespace

} // namespace
