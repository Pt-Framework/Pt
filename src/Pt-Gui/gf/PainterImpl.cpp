/*
 * Copyright (C) 2006 Marc Boris Duerner
 * Copyright (C) 2005-2007 by Aloysius Indrayanto
 * Copyright (C) 2005-2007 by Sebastian Pieck
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
#include "ApplicationImpl.h"
#include "PainterImpl.h"

#include "Pt/Gui/Pixmap.h"
#include "Pt/Gfx/Rect.h"
#include "Pt/Gfx/FontMetrics.h"
#include "Pt/Text/TextStream.h"
#include "Pt/Text/Utf16Codec.h"

#include <iostream>
#include <sstream>
#include <cmath>


namespace Pt {

namespace Gui {


PainterImpl::PainterImpl( )
//: _font("sans-serif")
{
}


PainterImpl::~PainterImpl()
{
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


void PainterImpl::drawPixel(const Gfx::Point& to)
{

}


void PainterImpl::drawLine(const Gfx::Point& from, const Gfx::Point& to)
{

}


void PainterImpl::drawText(const Gfx::Point& to, const Pt::String& text)
{

}


void PainterImpl::drawRect(const Gfx::Rect& rect)
{

}


void PainterImpl::drawPolyline(const Gfx::Point* points, const size_t pointCount)
{

}


void PainterImpl::drawEllipse(const Gfx::Point& topLeft, const Gfx::Size& size)
{

}


void PainterImpl::fillRect(const Gfx::Rect& rect)
{

}


void PainterImpl::fillEllipse(const Gfx::Point& topLeft, const Gfx::Size& size)
{

}


void PainterImpl::fillPolygon(const Gfx::Point* points, const size_t pointCount)
{

}


void PainterImpl::drawPixmap(const Gfx::Point& to, Pixmap& pm)
{

}


void PainterImpl::drawPixmap(const Gfx::Point& to, Pixmap& pm,
                             const Gfx::Region& pmRegion)
{

}


void PainterImpl::drawImage(const Gfx::Point& to, const Gfx::ARgbImage& image)
{
    this->drawImage( to.x(), to.y(), image.begin(), image.end(), image.width(), image.height() );
}


void PainterImpl::drawImage(const Gfx::Point& to, const Gfx::ARgbImage& image,
                            const Gfx::Region& imageRegion)
{

}


void PainterImpl::copyImageData(ssize_t toX, ssize_t toY, const char* data, size_t fromWidth, size_t fromHeight)
{
    size_t pixelSize = Screen::instance().depth() / 8;


    if ( gf_draw_begin( *Screen::instance().drawContext() ) == GF_ERR_OK )
    {

        if (gf_surface_attach(Screen::instance().offscreenSurface(), *Screen::instance().devContext(),
                  fromWidth, fromHeight, fromWidth * pixelSize, GF_FORMAT_PKLE_ARGB1555, NULL,  (uint8_t*)data, 0) != GF_ERR_OK)
                  { throw std::logic_error("Cannot create Offscreen-Surface" + PT_SOURCEINFO);  }

        if ( gf_draw_blit2(*Screen::instance().drawContext(), *Screen::instance().offscreenSurface(),
                NULL, 0, 0, fromWidth-1, fromHeight-1, 0, 0) != GF_ERR_OK )
                { throw std::logic_error("Cannot blit bitmap!" + PT_SOURCEINFO); }

        if (gf_draw_flush( *Screen::instance().drawContext() ) != GF_ERR_OK )
           { throw std::logic_error("Cannot flush context!"+ PT_SOURCEINFO);  }

        gf_draw_end( *Screen::instance().drawContext() );
    }
    else
    {
        throw std::logic_error("Cannot begin draw!"+ PT_SOURCEINFO);
    }
}

} // namespace Gui

} // namespace Pt

