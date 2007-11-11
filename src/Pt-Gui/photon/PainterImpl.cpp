/***************************************************************************
 *   Copyright (C) 2006 Marc Boris Duerner                                  *
 *   Copyright (C) 2005-2007 by Aloysius Indrayanto                        *
 *   Copyright (C) 2005-2007 by Sebastian Pieck                            *
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
#include "ApplicationImpl.h"
#include "PainterImpl.h"

//#include "Pt/Gui/Pixmap.h"
#include "Pt/Math/Rect.h"
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
    PgSetRegion( _rid );
}


void PainterImpl::end()
{
    PtFlush();
}


void PainterImpl::setPen(const Gfx::Pen& pen)
{
    _pen = pen;
    
    Gfx::Rgb888Color rgb888;
    assign( rgb888, pen.color() );
	PgSetStrokeColor( rgb888.value() );
	
	PgSetStrokeWidth( pen.size() );

	switch( pen.joinStyle() )
	{
		case Pt::Gfx::Pen::RoundJoin:
			PgSetStrokeJoin( Pg_ROUND_JOIN);
			break;
		
		case Pt::Gfx::Pen::BevelJoin:
			PgSetStrokeJoin( Pg_BEVEL_JOIN);
			break;
			
		default:
			PgSetStrokeJoin( Pg_MITER_JOIN);
			PgSetStrokeJoin( Pg_BUTT_JOIN);
	}
	
	switch( pen.capStyle() )
	{
		case Pt::Gfx::Pen::FlatCap:
			PgSetStrokeCap(Pg_BUTT_CAP);
			break;
		
		case Pt::Gfx::Pen::RoundCap:
			PgSetStrokeCap(Pg_ROUND_CAP);
			break;
		
		case Pt::Gfx::Pen::TriangularCap:
			PgSetStrokeCap(Pg_POINT_CAP);
			break;
		
		case Pt::Gfx::Pen::ProjectingCap:
			PgSetStrokeCap(Pg_SQUARE_CAP);
			break;
		
		case Pt::Gfx::Pen::ButtCap:
			PgSetStrokeCap(Pg_BUTT_CAP);
			break;
	}
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
    PgDrawILine( from.x(), from.y(),to.x(),to.y() );
}


void PainterImpl::drawText(const Math::Point& to, const Pt::String& text)
{

}


void PainterImpl::drawRect(const Gfx::Rect& rect)
{

}


void PainterImpl::drawPolyline(const Math::Point* points, const size_t pointCount)
{

}


void PainterImpl::drawEllipse(const Math::Point& topLeft, const Math::Size& size)
{

}


void PainterImpl::fillRect(const Gfx::Rect& rect)
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


void PainterImpl::copyImageData(ssize_t toX, ssize_t toY, const char* data, size_t fromWidth, size_t fromHeight)
{
/*    size_t pixelSize = Screen::instance().depth() / 8;


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
    }*/
}

} // namespace Gui

} // namespace Pt

