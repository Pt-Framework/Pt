/*
 * Copyright (C) 2006 Marc Boris Duerner
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
 #ifndef _AIX
#include <X11/Xft/Xft.h>
#endif

#include "PainterImpl.h"
#include "PaintSurfaceImpl.h"
#include "Pt/Types.h"
#include <Pt/System/Clock.h>
#include "Pt/Gui/Pixmap.h"
#include "Pt/Gfx/Rect.h"
#include "Pt/Gfx/Region.h"
#include "Pt/Gfx/FontMetrics.h"
#include "Pt/Gfx/Rgb888Color.h"
#include <Pt/Hmi/PaintSurface.h>
#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/NativePaintSurface.h>
#include <iostream>
#include <algorithm>
#include "ApplicationImpl.h"
#include <math.h>

namespace Pt {
namespace Hmi {

PainterImpl::PainterImpl(PaintSurfaceImpl* surface)
: _surface(surface)
, _pen(Gfx::Pen(1))
, _brush(Gfx::Brush(Gfx::ARgbColor(0, 0, 0)))
, _font("sans-serif")
, _penGc(0)
, _brushGc(0)
, _xftDraw(0)
, _xftFont(0)
{
    create();
}


PainterImpl::~PainterImpl()
{
    destroy();
}

void PainterImpl::create()
{
    Display* display = Application::instance().impl()->display();

    unsigned int screen = DefaultScreen(display);

    Visual* visual = XDefaultVisual(display, screen);

    _penGc = XCreateGC( display, drawable(), 0, NULL);

    _brushGc = XCreateGC( display, drawable(),0, NULL);

#ifndef _AIX
    _xftDraw = XftDrawCreate( display, drawable(), visual, DefaultColormap(display, screen) );
#endif

    this->setFont(_font);    
}

void PainterImpl::destroy()
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

    XftDrawDestroy(_xftDraw);
    _xftDraw = 0;
#endif
}


void PainterImpl::setSurface(NativePaintSurface& surface)
{
    destroy();
    create();

    setPen(_pen);
    setBrush(_brush);
    setFont(_font);
}


void PainterImpl::drawText( const Gfx::PointF& to, const Pt::String& text, const Gfx::ARgbColor* outline )
{
    drawText(to, text);
}


long PainterImpl::toXColor(const Gfx::ARgbColor& color)
{
    switch( this->depth() ) {
        case 32:
        case 24:
        {
            Gfx::Rgb888Color rgb888;
            assign(rgb888, color);
            return rgb888.value();
        }

        case 16:
        {
            Gfx::Rgb565Color rgb565;
            assign(rgb565, color);
            return rgb565.value();
        }

        case 15:
        {
            Gfx::Rgb555Color rgb555;
            assign(rgb555, color);
            return rgb555.value();
        }

        default:
            break;
    }

    return 0;
}

void PainterImpl::setPen(const Gfx::Pen& pen)
{
  if (pen == _pen) return;

    if( _pen.color() != pen.color() )
    {
        Display* display = Application::instance().impl()->display();
        XSetForeground( display, _penGc, this->toXColor( pen.color() ) );
        XSetBackground( display, _penGc, this->toXColor( pen.color() ) );
    }

    if( _pen.size() != pen.size() ||
        _pen.style() != pen.style() ||
        _pen.joinStyle() != pen.joinStyle() ||
        _pen.capStyle() != pen.capStyle() )
    {
        int lineStyle = LineSolid;
        switch( _pen.style() )
        {
            case Gfx::Pen::SolidStyle:
                lineStyle = LineSolid;
                break;

            case Gfx::Pen::DashStyle:
                lineStyle = LineOnOffDash;
                break;

            case Gfx::Pen::DoubleDash:
                break;
        }

        int joinStyle = JoinBevel;
        switch( _pen.joinStyle() )
        {
            case Gfx::Pen::RoundJoin:
                joinStyle = JoinRound;
                break;

            case Gfx::Pen::BevelJoin:
                joinStyle = JoinBevel;
                break;
        }

        int capStyle  = CapButt;
        switch( _pen.capStyle() )
        {
            case Gfx::Pen::FlatCap:
                capStyle = CapButt;
            break;

            case Gfx::Pen::RoundCap:
                capStyle = CapRound;
            break;

            case Gfx::Pen::TriangularCap:
            break;

            case Gfx::Pen::ProjectingCap:
            break;

            case Gfx::Pen::ButtCap:
				capStyle  = CapButt;
			break;
        }

        Display* display = Application::instance().impl()->display();
        XSetLineAttributes( display, _penGc, pen.size(), lineStyle, capStyle, joinStyle );
    }

    _pen = pen;
}

const Gfx::Pen& PainterImpl::pen() const
{
    return _pen;
}

void PainterImpl::setBrush(const Gfx::Brush& brush)
{
    if( brush.fillStyle() == Gfx::Brush::SolidFill ) 
    {
        Display* display = Application::instance().impl()->display();
        XSetFillStyle( display, _brushGc, FillSolid );
        XSetForeground( display, _brushGc, this->toXColor( brush.color() ) );
        XSetBackground( display, _brushGc, this->toXColor( brush.color() ) );
    }
    else if( brush.fillStyle() == Gfx::Brush::TextureFill ) 
    {
        Display* display = Application::instance().impl()->display();
        NativePaintSurface tile( Pt::Gfx::SizeF(brush.texture().width(), brush.texture().height()) );
        Painter& painter = tile.painter();
        painter.drawImage( Gfx::PointF(0, 0), brush.texture() );
        XSetFillStyle( display, _brushGc, FillTiled );
        XSetTile( display, _brushGc, tile.impl()->drawable() );
    }

    _brush = brush;;
}

const Gfx::Brush& PainterImpl::brush() const
{
    return _brush;
}

void PainterImpl::setFont(const Gfx::Font& font)
{
    Display* display = Application::instance().impl()->display();
  unsigned int screen = DefaultScreen(display);

#ifndef _AIX
    if(_xftFont) 
    {
        XftFontClose(display, _xftFont);
        _xftFont = 0;
    }

    int slant = XFT_SLANT_ROMAN;
    int weight = XFT_WEIGHT_MEDIUM;

    switch( _font.fontStyle() )
    {
        case Gfx::Font::ItalicStyle:
            slant = XFT_SLANT_ITALIC;
            break;

        case Gfx::Font::BoldStyle:
            weight = XFT_WEIGHT_BOLD;
            break;

        case Gfx::Font::BoldItalicStyle:
            slant = XFT_SLANT_ITALIC;
            weight = XFT_WEIGHT_BOLD;
            break;

        default:
            break;
    }

    double sina = 0.0, cosa = 0.0;
    switch( font.angle() ) {
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

    XftMatrix mx;
    mx.xx = cosa;
    mx.xy = -sina;
    mx.yx = sina;
    mx.yy = cosa;

    _xftFont = XftFontOpen(display, screen,
                           XFT_MATRIX, XftTypeMatrix, &mx,
                           XFT_FAMILY, XftTypeString, font.name().c_str(),
                           XFT_SIZE, XftTypeDouble, double( font.size() ),
                           XFT_SLANT, XftTypeInteger, slant,
                           XFT_WEIGHT, XftTypeInteger, weight,
                           NULL);

    _font = font;
#endif
}


std::string PainterImpl::determinePlatformDefaultFontName()
{
	//Todo:
  return "";
}


const Gfx::Font& PainterImpl::font() const
{
    return _font;
}


Gfx::FontMetrics PainterImpl::fontMetrics() const
{
#ifndef _AIX
    if(!_xftFont)
        return Gfx::FontMetrics(0, 0, 0, 0);

    return Gfx::FontMetrics(_xftFont->ascent, _xftFont->descent, 0, _xftFont->height);
#else
    return Gfx::FontMetrics(0, 0, 0, 0);
#endif
}


Gfx::FontMetrics PainterImpl::fontMetrics(Pt::String text) const
{
#ifndef _AIX
    if(!_xftFont)
        return Gfx::FontMetrics(0, 0, 0, 0);

    Display* display = Application::instance().impl()->display();
    XGlyphInfo info;

    XftTextExtents32(display, _xftFont, (XftChar32*)text.c_str(), text.size(), &info);

    return Gfx::FontMetrics(_xftFont->ascent, _xftFont->descent, info.width, _xftFont->height);
#else
    return Gfx::FontMetrics(0, 0, 0, 0);
#endif     
}


void PainterImpl::addFontName(const std::string& fontName)
{
    _fontList.push_back(fontName);
}

const std::list<std::string>& PainterImpl::fontFamilyNames()
{
#ifndef _AIX
    if( _fontList.empty() )
    {
        Display* display = Application::instance().impl()->display();
        unsigned int screen = DefaultScreen(display);
        char *family;

        XftFontSet* fonts = XftListFonts( display, screen, 0, XFT_FAMILY, (char*)0 );
        for (int i = 0; i < fonts->nfont; i++) {
            if( XftPatternGetString(fonts->fonts[i], XFT_FAMILY, 0, &family) == XftResultMatch )
                _fontList.push_back(family);
        }
        XftFontSetDestroy(fonts);
    }
#endif

    return _fontList;
}


int PainterImpl::depth() const
{
    Display* display = Application::instance().impl()->display();
    return XDefaultDepth(display, XDefaultScreen(display));
}


void PainterImpl::drawPixel(const Pt::Gfx::PointF& toF)
{
    Pt::Gfx::Point to = Application::instance().fromUnit(toF);
    Display* display = Application::instance().impl()->display();
    XDrawPoint( display, drawable(), _penGc, to.x(), to.y() );
}

void PainterImpl::drawLine(const Pt::Gfx::PointF& fromF, const  Pt::Gfx::PointF& toF)
{
    if (_pen.size() == 0) 
            return; // Draw nothing if the pen size is 0.    

    Pt::Gfx::Point to = Application::instance().fromUnit(toF);
    Pt::Gfx::Point from = Application::instance().fromUnit(fromF);

    Display* display = Application::instance().impl()->display();
    XDrawLine(display, drawable(), _penGc, from.x(), from.y(), to.x(), to.y());        
}

void PainterImpl::drawText(const Pt::Gfx::PointF& toF, const Pt::String& text)
{
    Pt::Gfx::Point to = Application::instance().fromUnit(toF);
    
    Display* display = Application::instance().impl()->display();
#ifndef _AIX
    XftColor xftColor;
    xftColor.pixel = 0; // this would be input for XftColorAllocValue
    xftColor.color.red   = _pen.color().red();
    xftColor.color.green = _pen.color().green();
    xftColor.color.blue  = _pen.color().blue();
    xftColor.color.alpha = 0xffff;

    XftDrawString32(_xftDraw, &xftColor, _xftFont, to.x(), to.y(), (XftChar32*)text.c_str(), text.size());
#endif
    XSync(display, false);    
}

void PainterImpl::fillRect(const Pt::Gfx::RectF& rectF)
{    
    Pt::Gfx::Rect rect = Application::instance().fromUnit(rectF);
    Display* display = Application::instance().impl()->display();
    XFillRectangle(display, drawable(), _brushGc, rect.x(), rect.y(), rect.width(), rect.height());
}

void PainterImpl::drawRect(const Pt::Gfx::RectF& rectF)
{
    Pt::Gfx::Rect rect = Application::instance().fromUnit(rectF);

    // Rectangle which has 0 width or height does not have to be drawn.
    // (Its not possible to draw a 0 pixel wide/high rectangle in X11.)
    if (rect.width() == 0 || rect.height() == 0) 
        return;

    Display* display = Application::instance().impl()->display();
    XDrawRectangle(display, drawable(), _penGc, rect.x(), rect.y(), rect.width() - 1, rect.height() - 1);    
}


void PainterImpl::drawEllipse(const Pt::Gfx::PointF& topLeftF, const Pt::Gfx::SizeF& sizeF)
{
    Pt::Gfx::Point topLeft = Application::instance().fromUnit(topLeftF);
    Pt::Gfx::Size size = Application::instance().fromUnit(sizeF);

    // Ellipse which has 0 width or height does not have to be drawn.
    // (Its not possible to draw a 0 pixel wide/high ellipse in X11.)
    if (size.width() == 0 || size.height() == 0) 
           return;

    Display* display = Application::instance().impl()->display();
    XDrawArc(display, drawable(), _penGc, topLeft.x(), topLeft.y(), size.width() - 1, size.height() - 1, 0, 360*64);    
}


void PainterImpl::fillEllipse(const Pt::Gfx::PointF& topLeftF, const Pt::Gfx::SizeF& sizeF)
{
    Pt::Gfx::Point topLeft = Application::instance().fromUnit(topLeftF);
    Pt::Gfx::Size size = Application::instance().fromUnit(sizeF);

    Display* display = Application::instance().impl()->display();
    XFillArc(display, drawable(), _brushGc, topLeft.x(), topLeft.y(), size.width(), size.height(), 0, 360*64);    
}


void PainterImpl::drawPolyline(const Pt::Gfx::PointF* points, const size_t pointCount)
{
    if (_pen.size() == 0) 
        return; // Draw nothing if the pen size is 0.

    Display* display = Application::instance().impl()->display();

    XPoint xpoints[pointCount];
    for(size_t n = 0; n < pointCount; ++n)
    {
        Pt::Gfx::Point point = Application::instance().fromUnit(points[n]);

        xpoints[n].x = point.x();
        xpoints[n].y = point.y();
    }

    XDrawLines(display, drawable(), _penGc, xpoints, pointCount, CoordModeOrigin);    
}


void PainterImpl::fillPolygon(const Pt::Gfx::PointF* points, const size_t pointCount)
{
    Display* display = Application::instance().impl()->display();

    XPoint xpoints[pointCount];

    for(size_t n = 0; n < pointCount; ++n) 
    {
       Pt::Gfx::Point point = Application::instance().fromUnit(points[n]);
        xpoints[n].x = point.x();
        xpoints[n].y = point.y();
    }

    XFillPolygon(display, drawable(), _brushGc, xpoints, pointCount, Complex, CoordModeOrigin);    
}


void PainterImpl::drawSurface(const Pt::Gfx::PointF& toF, PaintSurface& pm, const  Pt::Gfx::Region& pixmapRegion)
{
    NativePaintSurface* npm =  (NativePaintSurface*) &pm;

    Display* display = Application::instance().impl()->display();
    ::Drawable from = npm->impl()->drawable();

    Pt::Gfx::Size size =  Application::instance().fromUnit(pm.size());
    Pt::Gfx::Point to =  Application::instance().fromUnit(toF);

    XCopyArea( display, from, drawable(), _brushGc, pixmapRegion.x(), pixmapRegion.y(), size.width(), size.height(), to.x(), to.y() );
    XSync(display, false);
        
}

void PainterImpl::drawSurface(const Pt::Gfx::PointF& toF, PaintSurface& surface)
{
    NativePaintSurface* npm =  (NativePaintSurface*) &surface;
    Display* display = Application::instance().impl()->display();
    ::Drawable from = npm->impl()->drawable();

    Pt::Gfx::Size size = Application::instance().fromUnit(surface.size());
    Pt::Gfx::Point to =  Application::instance().fromUnit(toF);

    XCopyArea( display, from, drawable(), _brushGc, 0, 0, size.width(), size.height(), to.x(), to.y() );
    XSync(display, false);
}

void PainterImpl::drawImage(const Gfx::PointF& to, const Gfx::ARgbImage& image)
{
    this->drawImage( to.x(), to.y(), image.begin(), image.end(), image.width(), image.height() );
}

void PainterImpl::drawImage(const Gfx::PointF& to, const Gfx::ARgbImage& image, const Gfx::Region& imageRegion)
{
    Gfx::ARgbSubImage subImage(const_cast<Gfx::ARgbImage&>( image ), imageRegion);

    this->drawImage( to.x(), to.y(), subImage.begin(), subImage.end(), subImage.width(), subImage.height() );
}

void PainterImpl::drawImage(size_t x, size_t y, const char* data, size_t width, size_t height)
{    
    Display* display = Application::instance().impl()->display();
    unsigned int screen = DefaultScreen(display);
    Visual* visual = XDefaultVisual(display, screen);
    int depth = XDefaultDepth(display, screen);

    XImage* ximage = XCreateImage(display, visual, depth, ZPixmap, 0, NULL, width, height, 8, 0);
    ximage->data = (char*)data;
    XPutImage(display, drawable(), _brushGc, ximage, 0, 0, x, y, width, height);

    XSync(display, false);
    ximage->data = NULL;
    XDestroyImage(ximage); 
}

::Drawable PainterImpl::drawable()
{
    return  _surface->drawable();
}


}}

