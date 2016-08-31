
/* Copyright (C) 2015 Laurentiu-Gheorghe Crisan
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
 #include <Pt/Gfx/Rgb565Format.h>

 namespace Pt{
 namespace Gfx{

Rgb565Format::Rgb565Format()
: ImageFormat(2, 3)
{
}


void Rgb565Format::setColor(Pt::uint8_t* pixel, const Color& c) const
{
	Pt::uint16_t* val = (Pt::uint16_t*) pixel;			
	*val  =  (Pt::uint16_t) (c.red() * 32.0f);
	*val  |=  ((Pt::uint16_t) (c.green() * 64.0f))  << 5;
	*val  |=  ((Pt::uint16_t) (c.blue() * 32.0f))  << 11;
}


Color Rgb565Format::color(const Pt::uint8_t* pixel) const
{
	const Pt::uint16_t* val = (const Pt::uint16_t*) pixel;

	const float r = ((*val & 0xF800) >> 11) / 32.0f;
	const float g = ((*val & 0x07E0) >> 5) / 64.0f;
	const float b = (*val & 0x001F) / 32.0f;

	return Color(1, r, g, b );
}

} // namespace

} // namespace
