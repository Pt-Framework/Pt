/* Copyright (C) 2015 Marc Boris Duerner 
   Copyright (C) 2015 Laurentiu-Gheorghe Crisan
  
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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA*/
#ifndef Pt_Forms_Spacing_H
#define Pt_Forms_Spacing_H

#include <Pt/Forms/Api.h>

namespace Pt{
namespace Forms{

/** @brief Stores four-sided inset values.

    A control uses %Spacing as margin or padding. Construct it with one value
    for every side, a horizontal and vertical pair, or four sides.

    @code
    +------------------------+
    |        top             |
    |   +--------------+     |
    | l |   content    | r   |
    |   +--------------+     |
    |        bottom          |
    +------------------------+
    @endcode

    @code
    label.setMargin(Pt::Forms::Spacing(8));
    layout.setPadding(Pt::Forms::Spacing(12, 8));
    @endcode

    @ingroup Pt-Forms-Layouts
*/
class Spacing
{
  public:
    /** @brief Creates equal spacing of @a all on every side.
    */
    Spacing(double all)
    {
      set(all);
    }

    /** @brief Creates spacing of @a horizontal and @a vertical.
    */
    Spacing(double horizontal, double vertical)
    {
      set(horizontal, vertical);
    }

    /** @brief Creates spacing from @a left, @a top, @a right, and @a bottom.
    */
    Spacing(double left, double top, double right, double bottom)
    {
      set(left, top, right, bottom);
    }

    /** @brief Creates zero spacing on every side.
    */
    Spacing()    
    {
      set( 0, 0, 0, 0);
    }

    /** @brief Sets every side to @a value.
    */
    void set(double value)
    {
      set(value, value);
    }

    /** @brief Sets left and right to @a horizontal, top and bottom to @a vertical.
    */
    void set(double horizontal, double vertical)
    {
      _left = horizontal;
      _top = vertical;
      _right = horizontal;
      _bottom = vertical;
    }

    /** @brief Sets @a left, @a top, @a right, and @a bottom.
    */
    void set(double left, double top, double right, double bottom)
    {
      _left = left;
      _top = top;
      _right = right;
      _bottom = bottom;
    }

    /** @brief Returns the left inset.
    */
    double left() const
    {
      return _left;
    }

    /** @brief Sets the left inset to @a left.
    */
    void setLeft(double left)
    {
      _left = left;
    }

    /** @brief Returns the sum of the top and bottom insets.
    */
    double topBottom() const
    {
      return _top + _bottom;
    }

    /** @brief Returns the sum of the left and right insets.
    */
    double leftRight() const
    {
      return _left + _right;
    }

    /** @brief Returns the top inset.
    */
    double top() const
    {
      return _top;
    }

    /** @brief Sets the top inset to @a top.
    */
    void setTop(double top)
    {
      _top = top;
    } 

    /** @brief Returns the right inset.
    */
    double right() const
    {
      return _right;
    }

    /** @brief Sets the right inset to @a right.
    */
    void setRight(double right)
    {
      _right = right;
    } 

    /** @brief Returns the bottom inset.
    */
    double bottom() const
    {
      return _bottom;
    }

    /** @brief Sets the bottom inset to @a bottom.
    */
    void setBottom(double bottom)
    {
      _bottom = bottom;
    }

    /** @brief Returns true when both values compare equal.
    */
    bool operator==(const Spacing& s) const
    {
        return (s._left == _left)   &&
               (s._top == _top)     &&
               (s._right == _right) &&
               (s._bottom == _bottom);
    }

  private:
    double _left;
    double _top;
    double _right;
    double _bottom;        
};

}}

#endif
