/***************************************************************************
 *   Copyright (C) 2006 Marc Boris Dürner                                  *
 *   Copyright (C) 2005-2007 by Aloysius Indrayanto                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "PainterImpl.h"

#include "Pt/Gui/Pixmap.h"
#include "Pt/Math/Rect.h"
#include "Pt/Gfx/FontMetrics.h"
#include "Pt/Text/TextStream.h"
#include "Pt/Text/Utf16Codec.h"

#include <iostream>
#include <sstream>
#include <cmath>


namespace Pt {

namespace Gui {


PainterImpl::PainterImpl()
: _fd(-1)
, _buffer(0)
, _bufferSize(0)
, _font("sans-serif")
{
    // Open the frame buffer device
    _fd = open ("/dev/fb0", O_RDWR);
    if(_fd < 0)
        throw std::runtime_error("Could not open framebuffer device" + PT_SOURCEINFO);

/*
    if( 0 > ioctl(_fd, FBIOGET_VSCREENINFO, &_screenInfo) )
        throw std::runtime_error("FBIOGET_VSCREENINFO failed" + PT_SOURCEINFO);

    _screenInfo.bits_per_pixel = 16;
    _screenInfo.xres           = 640;
    _screenInfo.yres           = 480;

    if( 0 > ioctl(_fd, FBIOPUT_VSCREENINFO, &_screenInfo) )
        throw std::runtime_error("FBIOPUT_VSCREENINFO failed" + PT_SOURCEINFO);
*/

    if( 0 > ioctl(_fd, FBIOGET_VSCREENINFO, &_screenInfo) )
        throw std::runtime_error("FBIOGET_VSCREENINFO failed" + PT_SOURCEINFO);

    // Get the fixed state
    //if( ioctl(_fd, FBIOGET_FSCREENINFO, &_fixedInfo) < 0 )
    //    throw std::runtime_error("FBIOGET_FSCREENINFO failed" + PT_SOURCEINFO);

    //_fixedInfo.type;   // 0 -> Packed pixels
                         // 1 -> Non interleaved planes
                         // 2 -> Interleaved planes
                         // 3 -> Text/attributes
                         // 4 -> EGA/VGA planes

    //_fixedInfo.visual; // 0 -> Mono (1=black, 0=white)
                         // 1 -> Mono (1=white, 0=black)
                         // 2 -> True color
                         // 3 -> Pseudo color (like atari)
                         // 4 -> Direct color
                         // 5 -> Pseudo color readonly

    // Memory map the display
    unsigned _pitch = _screenInfo.xres * _screenInfo.bits_per_pixel / 8;
    _bufferSize     = _pitch * _screenInfo.yres;
    _buffer         =  mmap(NULL, _bufferSize, PROT_READ | PROT_WRITE, MAP_SHARED, _fd, 0);
}


PainterImpl::~PainterImpl()
{
    if(_buffer)
        munmap(_buffer, _bufferSize);

    if(_fd > 0)
        close(_fd);
}


void PainterImpl::begin()
{
}


void PainterImpl::end()
{

}


void PainterImpl::setPen(const Gfx::Pen& pen)
{
    _pen = pen;
}


const Gfx::Pen& PainterImpl::pen() const
{
    return _pen;
}


void PainterImpl::setBrush(const Gfx::Brush& brush)
{
    _brush = brush;
}


const Gfx::Brush& PainterImpl::brush() const
{
    return _brush;
}


const Gfx::Font& PainterImpl::font() const
{
    return _font;
}


void PainterImpl::setFont(const Gfx::Font& font)
{
    _font = font;
}


Gfx::FontMetrics PainterImpl::fontMetrics() const
{
    return Gfx::FontMetrics(0, 0, 0, 0);
}


Gfx::FontMetrics PainterImpl::fontMetrics(const Pt::String& text) const
{
    return Gfx::FontMetrics(0, 0, 0, 0);
}


const std::list<std::string>& PainterImpl::fontFamilyNames()
{
    static const std::list<std::string> _fontList;
    return _fontList;
}


void PainterImpl::drawPixel(const Math::Point& to)
{

}


void PainterImpl::drawLine(const Math::Point& from, const Math::Point& to)
{

}


void PainterImpl::drawText(const Math::Point& to, const Pt::String& text)
{

}


void PainterImpl::drawRect(const Math::Rect& rect)
{

}


void PainterImpl::drawPolyline(const Math::Point* points, const size_t pointCount)
{

}


void PainterImpl::drawEllipse(const Math::Point& topLeft, const Math::Size& size)
{

}


void PainterImpl::fillRect(const Math::Rect& rect)
{

}


void PainterImpl::fillEllipse(const Math::Point& topLeft, const Math::Size& size)
{

}


void PainterImpl::fillPolygon(const Math::Point* points, const size_t pointCount)
{

}


void PainterImpl::drawPixmap(const Math::Point& to, Pixmap& pm)
{

}


void PainterImpl::drawPixmap(const Math::Point& to, Pixmap& pm,
                             const Gfx::Region& pmRegion)
{

}


void PainterImpl::drawImage(const Math::Point& to, const Gfx::ARgbImage& image)
{
    this->drawImage( to.x(), to.y(), image.begin(), image.end(), image.width(), image.height() );
}


void PainterImpl::drawImage(const Math::Point& to, const Gfx::ARgbImage& image,
                            const Gfx::Region& imageRegion)
{

}


} // namespace Gui

} // namespace Pt

