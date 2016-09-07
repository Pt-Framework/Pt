/*
 * Copyright (C) 2015 Laurentiu-Gheorghe Crisan
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA*/
#include <Pt/Gfx/Color.h>
#include <Pt/StringStream.h>
#include <Pt/SerializationError.h>
#include <Pt/SourceInfo.h>
#include <Pt/ConversionError.h>
#include <iostream>
#include <iomanip> 

namespace Pt{
namespace Gfx{

Color::Color()
: _a(1)
, _r(0)
, _g(0)
, _b(0)
{
}


Color::Color(Pt::uint16_t a, Pt::uint16_t r, Pt::uint16_t g, Pt::uint16_t b )
: _a(a)
, _r(r)
, _g(g)
, _b(b) 
{
}	


Color::Color(Pt::uint16_t r, Pt::uint16_t g, Pt::uint16_t b )
: _a(1)
, _r(r)
, _g(g)
, _b(b) 
{
}


Pt::String Color::toHtml() const
{
  Pt::StringStream ss;

  ss << std::hex << Pt::Char('#')
      << std::hex << std::setw(2) << std::setfill( Pt::Char('0') ) << (int)(red() / 257)
      << std::hex << std::setw(2) << std::setfill( Pt::Char('0') ) << (int)(green()/ 257)
      << std::hex << std::setw(2) << std::setfill( Pt::Char('0') ) << (int)(blue() / 257);

  return ss.str();
}


Color Color::fromHtml(const Pt::String& s)
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
    ss2.str( Pt::String() );

    ss.get(token, 3);
    ss2 << token;
    ss2 >> std::hex >> g;
    ss2.clear();
    ss2.str( Pt::String()  );

    ss.get(token, 3);
    ss2 << token;
    ss2 >> std::hex >> b;
    ss2.clear();
    ss2.str( Pt::String()  );

    if( ss.fail() )
        throw Pt::ConversionError( "conversion from HTML color code failed" );

    return Color(r*257,g*257, b*257);
}


}}
