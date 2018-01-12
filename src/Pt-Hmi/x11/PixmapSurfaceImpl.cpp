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
#include "PictureImpl.h"

#include <Pt/Hmi/Painter.h>
#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/PixmapSurface.h>

#ifndef _AIX
#include <X11/Xft/Xft.h>
#endif

namespace Pt { 

namespace Hmi { 

PixmapSurfaceImpl::PixmapSurfaceImpl()
: _size(10, 10)
, _painter(0)
, _drawable(0)
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
    unsigned int depth = DefaultDepth( display, DefaultScreen(display) );
    
    _drawable = XCreatePixmap(display, XDefaultRootWindow(display), 
                              lround(size.width()), lround(size.height()), depth);
    
    XSync(display, false);  
}


void PixmapSurfaceImpl::destroy()
{
    Display* display = Application::instance().impl()->display();
    
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


void PixmapSurfaceImpl::begin(Painter& painter)
{
    _painter = &painter;
}


void PixmapSurfaceImpl::finish()
{
    _painter = 0;
}


const Gfx::ImageFormat& PixmapSurfaceImpl::format() const
{
    return Gfx::ImageFormat::argb32();
}


void PixmapSurfaceImpl::setPen(const Gfx::Pen& pen)
{
}


Gfx::FontMetrics PixmapSurfaceImpl::fontMetrics(const Pt::String& text) const
{
    if( ! _painter )
        return Gfx::FontMetrics(0, 0, 0, 0);

#ifndef _AIX
    _XftFont* font = _painter->impl().font();
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

    GC& penGc = _painter->impl().pen();

    Display* display = Application::instance().impl()->display();
    XDrawLine(display, _drawable, penGc, fromX, fromY, toX, toY); 
}


void PixmapSurfaceImpl::drawText(const Gfx::PointF& to, const Pt::String& text)
{
    if( ! _painter ) 
        return; 

    int toX = lround( to.x() );
    int toY = lround( to.y() );
    
    Display* display = Application::instance().impl()->display();


    Gfx::Color penColor = _painter->pen().color();
#ifndef _AIX
    XftColor xftColor;
    xftColor.pixel = 0; // this would be input for XftColorAllocValue
    xftColor.color.red   = _pen.color().red();
    xftColor.color.green = _pen.color().green();
    xftColor.color.blue  = _pen.color().blue();
    xftColor.color.alpha = 0xffff;

    XftFont* font = _painter->impl().font();
    XftDraw* draw = _painter->impl().xftDraw();

    XftDrawString32(draw, &xftColor, font, toX, toY, 
                    (XftChar32*) text.c_str(), text.size());
#endif

    XSync(display, false);    
}


void PixmapSurfaceImpl::drawRect(const Gfx::RectF& rect)
{
    if( ! _painter ) 
        return; 

    Pt::Gfx::Rect rect = Gfx::round(rectF);

    if( rect.width() == 0 || rect.height() == 0 ) 
        return;

    Display* display = Application::instance().impl()->display();
    GC& penGc = _painter->impl().pen();

    XDrawRectangle(display, _drawable, penGc, 
                   rect.x(), rect.y(), rect.width() - 1, rect.height() - 1);    
}


void PixmapSurfaceImpl::fillRect(const Gfx::RectF& rect)
{    
    if( ! _painter ) 
        return; 

    Pt::Gfx::Rect rect = Gfx::round(rectF);

    if( rect.width() == 0 || rect.height() == 0 ) 
        return;

    Display* display = Application::instance().impl()->display();
    GC& brushGc = _painter->impl().brush();

    XFillRectangle(display, _drawable, brushGc, 
                   rect.x(), rect.y(), rect.width(), rect.height());
}


void PainterImpl::drawEllipse(const Pt::Gfx::PointF& topLeftF, const Pt::Gfx::SizeF& sizeF)
{
    if( ! _painter ) 
        return; 

    Pt::Gfx::Point topLeft = Gfx::round(topLeftF);
    Pt::Gfx::Size size = Gfx::round(sizeF);

    if (size.width() == 0 || size.height() == 0) 
        return;

    Display* display = Application::instance().impl()->display();
    GC& penGc = _painter->impl().pen();

    XDrawArc(display, _drawable, penGc, 
             topLeft.x(), topLeft.y(), 
             size.width() - 1, size.height() - 1, 
             0, 360*64);    
}


void PainterImpl::fillEllipse(const Pt::Gfx::PointF& topLeftF, const Pt::Gfx::SizeF& sizeF)
{
    if( ! _painter ) 
        return; 

    Pt::Gfx::Point topLeft = Gfx::round(topLeftF);
    Pt::Gfx::Size size = Gfx::round(sizeF);

    if (size.width() == 0 || size.height() == 0) 
           return;

    Display* display = Application::instance().impl()->display();
    GC& brushGc = _painter->impl().brush();

    XFillArc(display, _drawable, brushGc, 
             topLeft.x(), topLeft.y(), 
             size.width(), size.height(), 
             0, 360 * 64);    
}


void PainterImpl::drawPolyline(const Pt::Gfx::PointF* points, size_t pointCount)
{
    if( ! _painter ) 
        return; 

    Display* display = Application::instance().impl()->display();
    GC& penGc = _painter->impl().pen();

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


void PainterImpl::fillPolygon(const Pt::Gfx::PointF* points, size_t pointCount)
{
    if( ! _painter ) 
        return; 

    Display* display = Application::instance().impl()->display();
    GC& brushGc = _painter->impl().brush();

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


void PainterImpl::drawSurface(const Pt::Gfx::PointF& toF, PixmapSurface& pm)
{
    if( ! _painter ) 
        return; 

    Display* display = Application::instance().impl()->display();
    GC& brushGc = _painter->impl().brush();
    ::Drawable from = pm.pixmapImpl()->drawable();

    Pt::Gfx::Size size = Gfx::round(surface.size());
    Pt::Gfx::Point to =  Gfx::round(toF);

    XCopyArea(display, from, _drawable, brushGc, 
            0, 0, size.width(), size.height(), to.x(), to.y() );
    
    XSync(display, false);
}


void PainterImpl::drawSurface(const Pt::Gfx::PointF& toF, 
                              PixmapSurface& pm, 
                              const Gfx::RectF& pmRect))
{
    if( ! _painter ) 
        return; 

	Display* display = Application::instance().impl()->display();
    GC& brushGc = _painter->impl().brush();
	::Drawable from = pm.impl()->drawable();

	Pt::Gfx::Size size =  Gfx::round(pm.size());
	Pt::Gfx::Point to =  Gfx::round(toF);
    const Gfx::Point from = Gfx::round( pmRect.topLeft() );

	XCopyArea(display, from, _drawable, brushGc, 
              from.x(), from.y(), 
              size.width(), size.height(), 
              to.x(), to.y() );
	
    XSync(display, false);        
}


void PainterImpl::drawImage(const Gfx::PointF& toF, const Gfx::Image& image)
{
    if( ! _painter ) 
        return; 

    Gfx::Point to = Gfx::round(toF);

    Display* display = Application::instance().impl()->display();
    GC& brushGc = _painter->impl().brush();
    Visual* visual = XDefaultVisual(display, screen);
    unsigned int screen = DefaultScreen(display);
    int depth = XDefaultDepth(display, screen);

    Gfx::Image rgb24Image( Gfx::ImageFormat::rgb24Format(), image.size() );
    Gfx::copy( image.begin(), image.end(), rgb24Image.begin() );

    XImage* ximage = XCreateImage(display, visual, depth, ZPixmap, 
                                  0, NULL, image.width(), image.height(), 8, 0);
    ximage->data = (char*) rgb24Image.data();
    
    XPutImage( display, _drawable, brushGc, ximage, 
               0, 0, to.x(), to.y(), 
               rgb24Image.width(), rgb24Image.height() );

    XSync(display, false);
    
    ximage->data = NULL;
    XDestroyImage(ximage); 
}


void PainterImpl::drawImage(const Gfx::PointF& toF, 
                            const Gfx::Image& image, 
                            const Gfx::RectF& imgRect))
{
    // TODO
    throw std::runtime_error("not implemented");
}

void PixmapSurfaceImpl::drawPicture(const Gfx::PointF& toF, const Picture& pic)
{
    const PictureImpl* picImpl = pic.impl();

    Gfx::Point to = Gfx::round(toF);

    if( picImpl->empty() )
      return;
}

} // namespace

} // namespace
