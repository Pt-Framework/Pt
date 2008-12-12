/*
 * Copyright (C) 2006 PTV AG
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

#include "Pt/Gfx/FontMetrics.h"


namespace Pt {

namespace Gfx {

FontMetrics::FontMetrics()
: _ascent( 0)
, _descent(0)
, _width(0)
, _height(0)
{ }

FontMetrics::FontMetrics(size_t ascent, size_t descent, size_t width, size_t height)
: _ascent(ascent)
, _descent(descent)
, _width(width)
, _height(height)
{
}

size_t FontMetrics::ascent() const
{
    return _ascent;
}

size_t FontMetrics::descent() const
{
    return _descent;
}

size_t FontMetrics::width() const
{
    return _width;
}

size_t FontMetrics::height() const
{
    return _height;
}


} // namespace Gfx

} // namespace Pt
