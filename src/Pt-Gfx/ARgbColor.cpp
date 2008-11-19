/***************************************************************************
 *   Copyright (C) 2006 by Aloysius Indrayanto                             *
 *   Copyright (C) 2006 by Marc Boris Dürner                               *
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

#include <Pt/Gfx/ARgbColor.h>
#include <Pt/Gfx/ARgb8888Color.h>
#include <Pt/StringStream.h>


namespace Pt {

namespace Gfx {

Pt::String Color<ARgb>::toHtml() const
{
    ARgb8888Color rgb8;
    assign(rgb8, *this);

    Pt::StringStream ss;
    ss << std::hex << Pt::Char('#')
       << std::hex << std::setw(2) << std::setfill( Pt::Char('0') ) << (int)rgb8.red()
       << std::hex << std::setw(2) << std::setfill( Pt::Char('0') ) << (int)rgb8.green()
       << std::hex << std::setw(2) << std::setfill( Pt::Char('0') ) << (int)rgb8.blue();

    return ss.str();
}


Color<ARgb> Color<ARgb>::fromHtml(const Pt::String& s)
{
    Pt::StringStream ss(s);
    Pt::StringStream ss2;

    if( ss.get() != '#' )
        throw SerializationError( PT_ERROR_MSG("Invalid color value") );

    int r,g,b;

    Pt::Char token[3];
    ss.get(token, 3);
    ss2 << token;
    ss2 >> std::hex >> r;
    ss2.clear();
    ss2.str( L"" );

    ss.get(token, 3);
    ss2 << token;
    ss2 >> std::hex >> g;
    ss2.clear();
    ss2.str( L"" );

    ss.get(token, 3);
    ss2 << token;
    ss2 >> std::hex >> b;
    ss2.clear();
    ss2.str( L"" );

    if( ss.fail() )
        throw Pt::ConversionError( PT_CONVERSIONERROR(Pt::Gfx::ARgbColor, Pt::String) );

    ARgb8888Color rgb8( (uint8_t)r, (uint8_t)g, (uint8_t)b);
    Color<Gfx::ARgb> color;
    assign(color, rgb8);

    return color;
}


void operator >>=(const SerializationInfo& si, Gfx::Color<Pt::Gfx::ARgb>& color)
{
    Pt::String s = si.toValue<Pt::String>();
    color = Gfx::ARgbColor::fromHtml(s);
}


void operator <<=(SerializationInfo& si, const Gfx::Color<Pt::Gfx::ARgb>& color)
{
    si.setTypeName("ARgbColor");
    si.setValue( color.toHtml() );
}

}

}
