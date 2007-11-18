/***************************************************************************
 *   Copyright (C) 2006 Marc Boris Duerner                                  *
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
#include "PixmapImpl.h"
#include "Pt/Gui/Pixmap.h"
#include "Pt/Gfx/Rect.h"
#include "Pt/Gfx/FontMetrics.h"
#include <iostream>

namespace Pt {

namespace Gui {

PainterImpl::PainterImpl( )
: _dc(0)
, _font("sans-serif")
{
	_gc = PgCreateGC(0);
}


PainterImpl::~PainterImpl()
{
	PgDestroyGC( _gc );
}


void PainterImpl::setPen(const Gfx::Pen& pen)
{
	PgSetGC(_gc);
	PhDCSetCurrent(_dc);

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
	PgSetGC(_gc);
	PhDCSetCurrent(_dc);

    Gfx::Rgb888Color rgb888;
    assign( rgb888, brush.color() );
	PgSetFillColor( rgb888.value() );

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
	PgSetGC(_gc);
	PhDCSetCurrent(_dc);

    PgDrawILine( from.x(), from.y(), to.x(),to.y() );
}


void PainterImpl::drawText(const Math::Point& to, const Pt::String& text)
{

}


void PainterImpl::drawRect(const Gfx::Rect& rect)
{
	PgSetGC(_gc);
	PhDCSetCurrent(_dc);

	PgDrawIRect( rect.x(), rect.y(), rect.x()+rect.width(), rect.y()+rect.height(), Pg_DRAW_STROKE);
}


void PainterImpl::drawPolyline(const Math::Point* points, const size_t pointCount)
{

}


void PainterImpl::drawEllipse(const Math::Point& topLeft, const Math::Size& size)
{

}


void PainterImpl::fillRect(const Gfx::Rect& rect)
{
	PgSetGC(_gc);
	PhDCSetCurrent(_dc);

	PgDrawIRect(rect.x(), rect.y(), rect.x()+rect.width(), rect.y()+rect.height(), Pg_DRAW_FILL);
}


void PainterImpl::fillEllipse(const Math::Point& topLeft, const Math::Size& size)
{

}


void PainterImpl::fillPolygon(const Math::Point* points, const size_t pointCount)
{

}


void PainterImpl::drawPixmap(const Math::Point& to, Pixmap& pm)
{
	PgSetGC(_gc);
	PhDCSetCurrent(_dc);

	PhPoint_t _to = { to.x(), to.y() };
	PgDrawPhImage(&_to, pm.impl().image(), 0 );
}


void PainterImpl::drawPixmap(const Math::Point& to, Pixmap& pm, const Gfx::Region& region)
{
	PgSetGC(_gc);
	PhDCSetCurrent(_dc);

	PhPoint_t _to = { to.x(), to.y() };
	PhRect_t rect = { region.x(), region.y(), region.x() + region.width(), region.y() + region.height() };
	
	PgDrawPhImageRectv(&_to, pm.impl().image(), &rect, NULL );
	PgFlush(); // TODO: is this necessary ???
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

}

} // namespace Gui

} // namespace Pt


