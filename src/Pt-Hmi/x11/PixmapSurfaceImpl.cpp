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

#include "PixmapSurfaceImpl.h"
#include "PaintData.h"
#include "ApplicationImpl.h"

#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/PixmapSurface.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/Gfx/Algorithm.h>

#ifndef _AIX
#include <X11/Xft/Xft.h>
#endif

namespace Pt { 

namespace Hmi { 

PixmapSurfaceImpl::PixmapSurfaceImpl()
: _size(10, 10)
, _paintData(0)
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


void PixmapSurfaceImpl::begin(Gfx::Painter& painter)
{
    _painter = &painter;

    Gfx::PaintData* pd = painter.paintData();
    _paintData = dynamic_cast<PaintData*>(pd);

    if (_paintData == 0)
    {
        delete pd;

        _paintData = new PaintData();
        painter.setPaintData(_paintData);
    }
}


void PixmapSurfaceImpl::finish()
{
    _paintData = 0;
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


    // TODO: only keep native clip rect in paint data
    _paintData->setClip(rectF);
}


void PixmapSurfaceImpl::resetClip()
{
    XftDrawSetClip(_xftDraw, 0);

    // TODO: only keep native clip rect in paint data
    _paintData->resetClip();
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
    _paintData->setPen(pen);
}


void PixmapSurfaceImpl::setBrush(const Gfx::Brush& brush)
{
    _paintData->setBrush(brush);
}


void PixmapSurfaceImpl::setFont(const Gfx::Font& font)
{
    _paintData->setFont(font);
}


Gfx::FontMetrics PixmapSurfaceImpl::fontMetrics(const Pt::String& text) const
{
    if( ! _paintData )
        return Gfx::FontMetrics(0, 0, 0, 0);

#ifndef _AIX
    _XftFont* font = _paintData->font();
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


void PixmapSurfaceImpl::drawText(const Gfx::PointF& to, const Pt::String& text)
{
    if( ! _paintData || ! _painter ) 
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

    XftFont* font = _paintData->font();

    XftDrawString32(_xftDraw, &xftColor, font, toX, toY,
                    (XftChar32*) text.c_str(), text.size());
#endif

    //XSync(display, false);    
}


void PixmapSurfaceImpl::drawLine(const Gfx::PointF& from, const Gfx::PointF& to)
{
    if( ! _paintData ) 
        return;   

    int fromX = lround( from.x() - 0.4999 );
    int fromY = lround( from.y() - 0.4999 );
    int toX   = lround( to.x() - 0.4999 );
    int toY   = lround( to.y() - 0.4999 );

    GC& penGc = _paintData->pen();

    Display* display = Application::instance().impl()->display();
    XDrawLine(display, _drawable, penGc, fromX, fromY, toX, toY); 
}


void PixmapSurfaceImpl::drawRect(const Gfx::RectF& rectF)
{
    if( ! _paintData ) 
        return; 

    Pt::Gfx::Rect rect( Gfx::Point( lround(rectF.x() - 0.4999),
                                    lround(rectF.y() - 0.4999) ),
                        Gfx::Size( lround(rectF.width() - 0.4999),
                                   lround(rectF.height() - 0.4999) ) );

    if( rect.width() == 0 || rect.height() == 0 ) 
        return;

    Display* display = Application::instance().impl()->display();
    GC& penGc = _paintData->pen();

    XDrawRectangle( display, _drawable, penGc, 
                    rect.x(), rect.y(), rect.width(), rect.height() );
}


void PixmapSurfaceImpl::fillRect(const Gfx::RectF& rectF)
{    
    if( ! _paintData ) 
        return; 

    Pt::Gfx::Rect rect(Gfx::Point( lround(rectF.x() ),
                                   lround(rectF.y() ) ),
                       Gfx::Size( lround(rectF.width() + 0.001 ),
                                  lround(rectF.height() + 0.001 ) ) );

    if( rect.width() == 0 || rect.height() == 0 ) 
        return;

    Display* display = Application::instance().impl()->display();
    GC& brushGc = _paintData->brush();

    XFillRectangle(display, _drawable, brushGc, 
                   rect.x(), rect.y(), rect.width(), rect.height());

    XSync(display, False);
}


void PixmapSurfaceImpl::drawEllipse(const Pt::Gfx::PointF& topLeftF, 
                              const Pt::Gfx::SizeF& sizeF)
{
    if( ! _paintData ) 
        return; 

    Pt::Gfx::Point topLeft = Gfx::round(topLeftF);
    Pt::Gfx::Size size = Gfx::round(sizeF);

    if (size.width() == 0 || size.height() == 0) 
        return;

    Display* display = Application::instance().impl()->display();
    GC& penGc = _paintData->pen();

    XDrawArc(display, _drawable, penGc, 
             topLeft.x(), topLeft.y(), 
             size.width(), size.height(), 
             0, 360*64);    
}


void PixmapSurfaceImpl::fillEllipse(const Pt::Gfx::PointF& topLeftF, const Pt::Gfx::SizeF& sizeF)
{
    if( ! _paintData ) 
        return; 

    Pt::Gfx::Point topLeft = Gfx::round(topLeftF);
    Pt::Gfx::Size size = Gfx::round(sizeF);

    if (size.width() == 0 || size.height() == 0) 
           return;

    Display* display = Application::instance().impl()->display();
    GC& brushGc = _paintData->brush();

    XFillArc(display, _drawable, brushGc, 
             topLeft.x(), topLeft.y(), 
             size.width(), size.height(), 
             0, 360 * 64);    
}


void PixmapSurfaceImpl::drawPolyline(const Pt::Gfx::PointF* points, size_t pointCount)
{
    if( ! _paintData ) 
        return; 

    Display* display = Application::instance().impl()->display();
    GC& penGc = _paintData->pen();

    XPoint xpoints[pointCount];
    
    for(size_t n = 0; n < pointCount; ++n)
    {
        xpoints[n].x = Pt::lround( points[n].x() - 0.4999 );
        xpoints[n].y = Pt::lround( points[n].y() - 0.4999 );
    }

    XDrawLines(display, _drawable, penGc, 
            xpoints, pointCount, CoordModeOrigin);    
}


void PixmapSurfaceImpl::fillPolygon(const Pt::Gfx::PointF* points, size_t pointCount)
{
    if( ! _paintData ) 
        return; 

    Display* display = Application::instance().impl()->display();
    GC& brushGc = _paintData->brush();

    XPoint xpoints[pointCount];

    for(size_t n = 0; n < pointCount; ++n) 
    {
        xpoints[n].x = Pt::lround( points[n].x() - 0.4999 );
        xpoints[n].y = Pt::lround( points[n].y() - 0.4999 );
    }

    XFillPolygon(display, _drawable, brushGc, 
                 xpoints, pointCount, Complex, CoordModeOrigin);    
}


void PixmapSurfaceImpl::drawSurface(const Pt::Gfx::PointF& toF, 
                                    const PixmapSurface& pm)
{
    if( ! _paintData ) 
        return; 

    Display* display = Application::instance().impl()->display();
    GC& brushGc = _paintData->brush();
    ::Drawable from = pm.pixmapImpl()->drawable();

    Pt::Gfx::Size size = Gfx::round( pm.size() );
    Pt::Gfx::Point to =  Gfx::round( toF );

    XCopyArea(display, from, _drawable, brushGc, 
            0, 0, size.width(), size.height(), to.x(), to.y() );
    
    //XSync(display, false);
}


void PixmapSurfaceImpl::drawSurface(const Pt::Gfx::PointF& toF, 
                                    const PixmapSurface& pm, 
                                    const Gfx::RectF& pmRect)
{
    if( ! _paintData )
        return; 

    Display* display = Application::instance().impl()->display();
    GC& brushGc = _paintData->brush();
    ::Drawable source = pm.pixmapImpl()->drawable();

    Pt::Gfx::Size size =  Gfx::round( pm.size() );
    Pt::Gfx::Point to =  Gfx::round( toF );
    const Gfx::Point from = Gfx::round( pmRect.topLeft() );

    XCopyArea(display, source, _drawable, brushGc, 
              from.x(), from.y(), 
              size.width(), size.height(), 
              to.x(), to.y() );

    //XSync(display, false);        
}


void PixmapSurfaceImpl::drawImage(const Gfx::PointF& toF, const Gfx::Image& image)
{
    Pt::Gfx::RectF imageRect( Pt::Gfx::PointF(0, 0),
                              Pt::Gfx::SizeF(image.width(), image.height()) );
    
    drawImage(toF, image, imageRect);
}


void PixmapSurfaceImpl::drawImage(const Gfx::PointF& toF, 
                                  const Gfx::Image& image, 
                                  const Gfx::RectF& imgRectF)
{
    if( ! _paintData ) 
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
    GC& brushGc = _paintData->brush();

    //std::clog << "XPutImage" << to.x() << " " << to.y() << std::endl;
    XPutImage( display, _drawable, brushGc, ximage, 
               imageRect.x(), imageRect.y(), to.x(), to.y(), 
               imageRect.width(), imageRect.height() );
    
    ximage->data = NULL;

    //std::clog << "XDestroyImage" << std::endl;
    XDestroyImage(ximage); 
}


Gfx::Image PixmapSurfaceImpl::toImage(const Gfx::ImageFormat& iformat) const
{
    return Gfx::Image();
}


void PixmapSurfaceImpl::set(const Gfx::Image& image)
{
    resize( Gfx::SizeF( image.size().width(), 
                        image.size().height() ) );

    Gfx::PointF origin(0, 0);
    drawImage(origin, image);
}


std::string PixmapSurfaceImpl::defaultFont()
{
    return getDefaultFont();
}


void PixmapSurfaceImpl::setDefaultFont(const std::string& f)
{
    getDefaultFont() = f;
}


std::string& PixmapSurfaceImpl::getDefaultFont()
{ 
    static std::string _defaultFont;
    return _defaultFont; 
}


std::vector<std::string> PixmapSurfaceImpl::fontNames()
{
    std::vector<std::string> fonts;

#ifndef _AIX
    Display* display = Application::instance().impl()->display();
    unsigned int screen = DefaultScreen(display);
    char* family = 0;

    XftFontSet* fontSet = XftListFonts(display, screen, 0, XFT_FAMILY, (char*)0 );
    for(int i = 0; i < fontSet->nfont; i++) 
    {
        if( XftPatternGetString(fontSet->fonts[i], XFT_FAMILY, 0, &family) == XftResultMatch )
            fonts.push_back(family);
    }
    
    XftFontSetDestroy(fontSet);
#endif

    return fonts;
}


void PixmapSurfaceImpl::setFontDir(const System::Path& path)
{
}


Gfx::FontMetrics PixmapSurfaceImpl::fontMetrics(const Gfx::Font& font, const Pt::String& text)
{
    return PaintData::fontMetrics(font, text);
}

} // namespace

} // namespace
