/***************************************************************************
 *   Copyright (C) 2006-2007 by Marc Boris Drner                          *
 *   Copyright (C) 2006-2007 by Laurentiu-Gheorghe Crisan                  *
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
#include "Pt/Gfx/Pen.h"
#include <Pt/Gfx/ARgbImage.h>

namespace Pt {

namespace Gfx {

Pen::Pen()
: _penData( new PenData( 1, ARgbColor(0,0,0) ,SolidStyle, RoundCap, RoundJoin  ) )
{ }


Pen::Pen( size_t size )
: _penData( new PenData( size, ARgbColor(0,0,0) ,SolidStyle, RoundCap, RoundJoin  ) )
{ }


Pen::Pen( PenStyle style )
: _penData( new PenData( 1, ARgbColor(0,0,0) ,style, RoundCap, RoundJoin  ) )
{ }


Pen::Pen( const ARgbColor& color )
: _penData( new PenData( 1, color ,SolidStyle, RoundCap, RoundJoin  ) )
{ }


Pen::Pen( size_t size, const ARgbColor& color, PenStyle style, CapStyle cap,  JoinStyle join )
: _penData( new PenData( size, color ,style, cap,  join ) )
{ }


size_t Pen::size() const
{
    return _penData->size();
}


const ARgbColor& Pen::color() const
{
    return _penData->color();
}


Pen::PenStyle Pen::style() const
{
    return _penData->style();
}


Pen::CapStyle Pen::capStyle() const
{
    return _penData->capStyle();
}


Pen::JoinStyle Pen::joinStyle() const
{
    return _penData->joinStyle();
}


const ARgbImage& Pen::buffer() const
{
    return _penData->buffer();
}


bool operator==(const Pen& a, const Pen& b)
{
	return a._penData->size() == b._penData->size() &&
	       a._penData->color() == b._penData->color() &&
	       a._penData->style() == b._penData->style();
}


bool operator<(const Pen& a, const Pen& b)
{
	return a._penData->size() < b._penData->size();
}


void get(const SerializationInfo& si, Gfx::Pen& pen)
{
    size_t              penSize;
    Gfx::ARgbColor      penColor;
    Pt::ssize_t         penStyle;
    Pt::String html;

    Pt::StringStream ss( si.toString() );
    ss >> penSize;
    ss.get();

    getline( ss, html, Pt::Char('-') );

    ss >> penStyle;

    if( ss.fail() )
        throw ConversionError("Pen", PT_SOURCEINFO);

    pen = Gfx::Pen(penSize, Gfx::ARgbColor::fromHtml(html), (Gfx::Pen::PenStyle)penStyle);
}


void put(SerializationInfo& si, const Gfx::Pen& pen)
{
    Pt::StringStream ss;
    ss << pen.size() << Pt::Char('-')
       << pen.color().toHtml() << Pt::Char('-')
       << pen.style();

    si.setValue( ss.str() );
    si.setTypeName("Pen");
}

} // namespace Gfx

} // namespace Pt
