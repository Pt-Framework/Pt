/* Copyright (C) 2006-2015 Laurentiu-Gheorghe Crisan
   Copyright (C) 2006-2015 Marc Boris Duerner
   Copyright (C) 2010 Aloysius Indrayanto

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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
  02110-1301 USA
*/

#ifndef PT_GFX_FONTMETRICS_H
#define PT_GFX_FONTMETRICS_H

#include <Pt/Gfx/Api.h>
#include <Pt/Types.h>
#include <cstddef>

namespace Pt {

namespace Gfx {

class PT_GFX_API FontMetrics
{
  public:
      FontMetrics();

      FontMetrics(std::size_t ascent, std::size_t descent,
                  std::size_t width, std::size_t height);

      std::size_t ascent() const;

      std::size_t descent() const;

      std::size_t width() const;

      std::size_t height() const;

  private:
      std::size_t _ascent;
      std::size_t _descent;
      std::size_t _width;
      std::size_t _height;
};

} // namespace

} // namespace

#endif
