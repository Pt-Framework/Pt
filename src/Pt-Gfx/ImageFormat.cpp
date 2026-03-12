/* Copyright (C) 2015 Marc Boris Duerner

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  As a special exception, you may use this file as part of a free
  software library without restriction. Specifically, if other files
  instantiate templates or use macros or inline functions from this
  file, or you compile this file and link it with other files to
  produce an executable, this file does not by itself cause the
  resulting executable to be covered by the GNU General Public
  License. This exception does not however invalidate any other
  reasons why the executable file might be covered by the GNU Library
  General Public License.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
  MA 02110-1301 USA
*/

#include <Pt/Gfx/ImageFormat.h>
#include <Pt/Gfx/Argb32.h>
#include <Pt/Gfx/Rgb16.h>
#include <Pt/Gfx/Rgb32.h>
#include <cassert>

//
// TODO:
// Color template for 8bit und 16-bit pro kanal
// Color Color8 Color32
// 
// TODO:
// if a PixelView is assigned to a BasicImage<F>, then we can check if F == image.format()
// and memcpy if equal, or convert if neccessary:
// BasicImage<F> bi;
// Image i;
// PixelView v(i);
// bi = v;
//

namespace Pt {

namespace Gfx {

const ImageFormat& ImageFormat::get(int type) 
{
  static const Argb32 _argb32;
	return _argb32;
}


const ImageFormat& ImageFormat::rgb16()
{
  static const Rgb16 _rgb16;
	return _rgb16;
}


const ImageFormat& ImageFormat::rgb32()
{
  static const Rgb32 _rgb32;
	return _rgb32;
}


const ImageFormat& ImageFormat::argb32() 
{
  static const Argb32 _argb32;
	return _argb32;
}

} // namespace

} // namespace
