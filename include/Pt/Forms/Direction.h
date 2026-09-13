/* Copyright (C) 2015-2026 Marc Boris Duerner
  
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

#ifndef Pt_Forms_Direction_H
#define Pt_Forms_Direction_H

#include <Pt/Forms/Api.h>
#include <Pt/Types.h>

namespace Pt {

namespace Forms {

/** @brief Names a flow direction.

    %Left and %Right are horizontal. %Top and %Bottom are vertical. The
    default is %Left.

    @code
    Pt::Forms::FlowLayout row(Pt::Forms::Direction::Left);
    row.setDirection(Pt::Forms::Direction::Top);
    @endcode

    @ingroup Pt-Forms-Layouts
*/
class Direction
{
  public:
    /** @brief Flow axis and sense.
    */
    enum Mode
    {
        /** @brief Left to right.
        */
        Left,

        /** @brief Right to left.
        */
        Right,

        /** @brief Top to bottom.
        */
        Top,

        /** @brief Bottom to top.
        */
        Bottom
    };

    /** @brief Creates a direction with mode @a m.
    */
    Direction(Mode m = Left)
    : _mode(m)
    {}

    /** @brief Sets the direction to @a m and returns this object.
    */
    Direction& operator=(Mode m)
    {
        _mode = m;
        return *this;
    }

    /** @brief Returns the direction as an unsigned integer value.
    */
    operator Pt::uint32_t() const
    {
        return _mode;
    }

  private:
    Pt::uint32_t _mode;
};

} // namespace

} // namespace

#endif
