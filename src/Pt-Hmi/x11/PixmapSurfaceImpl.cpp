 /* Copyright (C) 2015 Marc Boris Duerner 
    Copyright (C) 2015 Laurentiu-Gheorghe Crisan
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  
  As a special exception, you may use this file as part of a free
  software library without restriction. Specifically, if other files
  instantiate templates or use macros or inline functions from this
  file, or you compile this file and link it with other files to
  produce an executable, this file does not by itself cause the
  resulting executable to be covered by the GNU General Public
  License. This exception does not however invalidate any other
  reasons why the executable file might be covered by the GNU Library
  General Public License.
  
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
  MA 02110-1301 USA
*/

#include "PixmapSurfaceImpl.h"

#ifdef PT_HMI_X11_RASTER

#include "ApplicationImpl.h"

#include <Pt/Hmi/Application.h>
#include <Pt/Gfx/ImageFormat.h>

namespace Pt {

namespace Hmi {

const Gfx::ImageFormat& getScreenFormat()
{
    int depth = Application::instance().impl()->depth();

    switch(depth)
    {
        case 16:
            return Gfx::ImageFormat::rgb16();

        case 24:
        case 32:
            return Gfx::ImageFormat::argb32();
    }

    return Gfx::ImageFormat::argb32();
}


PixmapSurfaceImpl::PixmapSurfaceImpl()
: ImageSurface(_image)
, _image( getScreenFormat() )
{
}


PixmapSurfaceImpl::~PixmapSurfaceImpl()
{
}


void PixmapSurfaceImpl::drawSurface(const Gfx::PointF& toF, const PixmapSurface& surface)
{
    Gfx::ImageSurface::drawSurface(toF, *surface.impl());
}


void PixmapSurfaceImpl::drawSurface(const Gfx::PointF& toF, const PixmapSurface& surface, const Gfx::RectF& pmRect)
{
    Gfx::ImageSurface::drawSurface(toF, *surface.impl(), pmRect);
}

} // namespace

} // namespace

#endif // PT_HMI_X11_RASTER

#ifdef PT_HMI_X11_CORE
#include "ApplicationImpl.h"

#include <Pt/Hmi/Application.h>
#include <Pt/Gfx/ImageFormat.h>
#include <Pt/Hmi/PixmapSurface.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/Gfx/Algorithm.h>

#include <cmath>

#ifndef _AIX
#include <X11/Xft/Xft.h>
#endif

namespace {

_XftFont* openFont(const Pt::Gfx::Font& font)
{
#ifndef _AIX
    Display* display = Pt::Hmi::Application::instance().impl()->display();
    unsigned int screen = XDefaultScreen(display);
    int slant = XFT_SLANT_ROMAN;
    int weight = XFT_WEIGHT_MEDIUM;

    if(font.style() == "Italic")
        slant = XFT_SLANT_ITALIC;
            

    if (font.style() == "Bold")
        weight = XFT_WEIGHT_BOLD;

    if (font.style()  == "BoldItalic")
    {
        slant = XFT_SLANT_ITALIC;
        weight = XFT_WEIGHT_BOLD;
    }


    double sina = 0.0, cosa = 0.0;
    /*
    switch( font.angle() ) 
    {
        case    0: sina =  0; cosa =  1; break;
        case  900: sina =  1; cosa =  0; break;
        case 1800: sina =  0; cosa = -1; break;
        case 2700: sina = -1; cosa =  0; break;
        default:
            double a = font.angle() * 3.14 / 1800.0;
            sina = sin(a);
            cosa = cos(a);
            break;
    }
    */
    sina = 0; cosa = 1;


    XftMatrix mx;
    mx.xx = cosa;
    mx.xy = -sina;
    mx.yx = sina;
    mx.yy = cosa;

    const char* fontName = font.name().empty() ? 
          Pt::Hmi::PixmapSurface::defaultFont().c_str()
        : font.name().c_str();

    XftFont* xftFont = XftFontOpen(display, screen,
                                   XFT_MATRIX, XftTypeMatrix, &mx,
                                   XFT_FAMILY, XftTypeString, fontName,
                                   XFT_SIZE, XftTypeDouble, double( font.size() ),
                                   XFT_SLANT, XftTypeInteger, slant,
                                   XFT_WEIGHT, XftTypeInteger, weight,
                                   NULL);

    return xftFont;
#else
    return 0;
#endif
}

} // namespace

namespace Pt {

namespace Hmi {

PaintData::PaintData()
: _penGc(0)
, _brushGc(0)
, _xftFont(0)
{
    create();
}


PaintData::~PaintData()
{
    destroy();
}


void PaintData::create()
{
    Display* display = Application::instance().impl()->display();
    ::Window root = XDefaultRootWindow(display);

    _penGc = XCreateGC(display, root, 0, NULL);
    _brushGc = XCreateGC(display, root, 0, NULL);
}


void PaintData::destroy()
{
    Display* display = Application::instance().impl()->display();

    XFreeGC(display, _penGc);
    _penGc = 0;

    XFreeGC(display, _brushGc);
    _brushGc = 0;

#ifndef _AIX
    if(_xftFont) 
    {
        XftFontClose(display, _xftFont);
        _xftFont = 0;
    }
#endif
}


long PaintData::toXColor(const Gfx::Color& color)
{
    Display* display = Application::instance().impl()->display();
    unsigned int screen = DefaultScreen(display);
    Colormap colorMap = DefaultColormap(display, screen);

    XColor xcolor;
    xcolor.red = color.red();
    xcolor.green = color.green();
    xcolor.blue = color.blue();
    xcolor.flags = DoRed | DoGreen | DoBlue;

    XAllocColor(display, colorMap, &xcolor);

    return xcolor.pixel;
}


void PaintData::setPen(const Gfx::Pen& pen)
{
    Display* display = Application::instance().impl()->display();

    XSetForeground( display, _penGc, this->toXColor( pen.color() ) );
    XSetBackground( display, _penGc, this->toXColor( pen.color() ) );

    int lineStyle = LineSolid;
    switch( pen.style() )
    {
        default:
        case Gfx::Pen::Solid:
        {
            lineStyle = LineSolid;
            break;
        }
        
        case Gfx::Pen::Dash:
        {
            if(pen.capStyle() == Gfx::Pen::RoundCap ||
               pen.capStyle() == Gfx::Pen::SquareCap)
            {
                char size = static_cast<char>( 2 * pen.size() );
                char dashList[] = { size, size };
                XSetDashes(display, _penGc, 0, dashList, 2);
            }
            else
            {
                char size = static_cast<char>( 3 * pen.size() );
                char spacing = static_cast<char>( pen.size() );
                char dashList[] = { size, spacing };
                XSetDashes(display, _penGc, 0, dashList, 2);
            }

            lineStyle = LineOnOffDash;
            break;
        }

        case Gfx::Pen::Dot:
        {
            if(pen.capStyle() == Gfx::Pen::RoundCap ||
               pen.capStyle() == Gfx::Pen::SquareCap)
            {
                char spacing = static_cast<char>( 2 * pen.size() );
                char dashList[] = { 1, spacing };
                XSetDashes(display, _penGc, 0, dashList, 2);
            }
            else
            {
                char size = static_cast<char>( pen.size() );
                char dashList[] = { size, size };
                XSetDashes(display, _penGc, 0, dashList, 2);
            }

            lineStyle = LineOnOffDash;
            break;
        }
    }

    int joinStyle = JoinBevel;
    switch( pen.joinStyle() )
    {
        default:
        case Gfx::Pen::BevelJoin:
            joinStyle = JoinBevel;
            break;

        case Gfx::Pen::RoundJoin:
            joinStyle = JoinRound;
            break;

        case Gfx::Pen::MiterJoin:
            joinStyle = JoinMiter;
            break;
    }

    int capStyle  = CapButt;
    switch( pen.capStyle() )
    {
        default:
        case Gfx::Pen::FlatCap:
            capStyle = CapButt;
            break;

        case Gfx::Pen::RoundCap:
            capStyle = CapRound;
            break;

        case Gfx::Pen::SquareCap:
            capStyle = CapProjecting;
            break;
    }

    XSetLineAttributes(display, _penGc, pen.size(), lineStyle, capStyle, joinStyle);
}


GC& PaintData::pen()
{
    return _penGc;
}


void PaintData::setBrush(const Gfx::Brush& brush)
{
    Display* display = Application::instance().impl()->display();

    if( brush.fillStyle() == Gfx::Brush::Solid ) 
    {
        XSetFillStyle( display, _brushGc, FillSolid );
        XSetForeground( display, _brushGc, this->toXColor( brush.color() ) );
        XSetBackground( display, _brushGc, this->toXColor( brush.color() ) );
    }
    else if( brush.fillStyle() == Gfx::Brush::Texture ) 
    {
        PixmapSurface tile;
        tile.resize( Pt::Gfx::SizeF(brush.texture().width(), 
                                    brush.texture().height()) );
        
        Gfx::Painter painter(tile);
        painter.drawImage( Gfx::PointF(0, 0), brush.texture() );
        
        XSetFillStyle(display, _brushGc, FillTiled);
        XSetTile(display, _brushGc, tile.pixmapImpl()->drawable() );
    }
}


GC& PaintData::brush()
{
    return _brushGc;
}


void PaintData::setClip(const Gfx::RectF& rectF)
{
    Display* display = Application::instance().impl()->display();
    Gfx::Rect rect = round(rectF);

    XRectangle xrect;
    xrect.x      = rect.x();
    xrect.y      = rect.y();
    xrect.width  = rect.width();
    xrect.height = rect.height();

    XSetClipRectangles(display, _penGc, 0, 0, &xrect, 1, Unsorted);
    XSetClipRectangles(display, _brushGc, 0, 0, &xrect, 1, Unsorted);
}


void PaintData::resetClip()
{
    Display* display = Application::instance().impl()->display();
    XSetClipMask(display, _penGc, None);
    XSetClipMask(display, _brushGc, None);
}


void PaintData::setFont(const Gfx::Font& font)
{
    Display* display = Application::instance().impl()->display();

#ifndef _AIX
    if(_xftFont) 
    {
        XftFontClose(display, _xftFont);
        _xftFont = 0;
    }

    _xftFont = openFont(font);
#endif
}


_XftFont* PaintData::font()
{
    return _xftFont;
}


Gfx::FontMetrics PaintData::fontMetrics(const Gfx::Font& font, 
                                          const Pt::String& text)
{
#ifndef _AIX
    _XftFont* xftFont = openFont(font);
    if( ! xftFont )
        return Gfx::FontMetrics();

    Display* display = Application::instance().impl()->display();
    
    XGlyphInfo info;
    XftTextExtents32(display, xftFont, (XftChar32*)text.c_str(), text.size(), &info);
    
    // TODO: use XftLockFace() to get FT_face instead

    XftFontClose(display, xftFont);

    Gfx::FontMetrics fm;
    fm.setAscent(xftFont->ascent);
    fm.setDescent(xftFont->descent);
    fm.setCapHeight(xftFont->ascent - xftFont->descent / 2.0);
    fm.setLeading(xftFont->descent / 2.0);
    fm.setWidth(info.width);
    return fm;
#else
    return Gfx::FontMetrics();
#endif     
}

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
        return Gfx::FontMetrics();

#ifndef _AIX
    _XftFont* font = _paintData->font();
    if( ! font )
        return Gfx::FontMetrics();
    
    Display* display = Application::instance().impl()->display();
    
    XGlyphInfo info;
    XftTextExtents32(display, font, (XftChar32*)text.c_str(), text.size(), &info);

    Gfx::FontMetrics fm;

    fm.setAscent(font->ascent);
    fm.setDescent(font->descent);
    fm.setWidth(info.width);
    fm.setCapHeight(2);

    return fm; //Gfx::FontMetrics(font->ascent, font->descent, info.width, font->height);
#else
    return Gfx::FontMetrics();
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

#endif // PT_HMI_X11_CORE
