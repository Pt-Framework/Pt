/* Copyright (C) 2015-2024 Laurentiu-Gheorghe Crisan

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

#include <Pt/Gfx/Paint.h>

namespace Pt {

namespace Gfx {

Paint::Paint()
{
}


Paint::~Paint()
{
}


const Gfx::CompositionMode& Paint::compositionMode() const
{
    return _compositionMode;
}


void Paint::setCompositionMode(const Gfx::CompositionMode& mode)
{
    _compositionMode = mode;
}


const Gfx::Pen& Paint::pen() const
{
    return _pen;
}


void Paint::setPen(const Gfx::Pen& pen)
{
    _pen = pen;
}


const Gfx::Brush& Paint::brush() const
{
    return _brush;
}


void Paint::setBrush(const Gfx::Brush& brush)
{
    _brush = brush;
}


const Gfx::Font& Paint::font() const
{
    return _font;
}


void Paint::setFont(const Gfx::Font& font)
{
    _font = font;
}

} // namespace

} // namespace
