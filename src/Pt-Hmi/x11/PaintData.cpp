/*
 * Copyright (C) 2015 Marc Boris Duerner 
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

#include "PaintData.h"
#include "ApplicationImpl.h"
#include "PixmapSurfaceImpl.h"

#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/PixmapSurface.h>

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

    switch( font.style() )
    {
        case Pt::Gfx::Font::Italic:
            slant = XFT_SLANT_ITALIC;
            break;

        case Pt::Gfx::Font::Bold:
            weight = XFT_WEIGHT_BOLD;
            break;

        case Pt::Gfx::Font::BoldItalic:
            slant = XFT_SLANT_ITALIC;
            weight = XFT_WEIGHT_BOLD;
            break;

        default:
            break;
    }

    double sina = 0.0, cosa = 0.0;
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
        return Gfx::FontMetrics(0, 0, 0, 0);

    Display* display = Application::instance().impl()->display();
    
    XGlyphInfo info;
    XftTextExtents32(display, xftFont, (XftChar32*)text.c_str(), text.size(), &info);
    
    // TODO: use XftLockFace() to get FT_face instead

    XftFontClose(display, xftFont);

    return Gfx::FontMetrics(xftFont->ascent, xftFont->descent, info.width, xftFont->height);
#else
    return Gfx::FontMetrics(0, 0, 0, 0);
#endif     
}

} // namespace

} // namespace
