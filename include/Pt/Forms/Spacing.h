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

class Spacing
{
  public:
    Spacing(double all)
    {
      set(all);
    }

    Spacing(double horizontal, double vertical)
    {
      set(horizontal, vertical);
    }

    Spacing(double left, double top, double right, double bottom)
    {
      set(left, top, right, bottom);
    }

    Spacing()    
    {
      set( 0, 0, 0, 0);
    }

    void set(double value)
    {
      set(value, value);
    }

    void set(double horizontal, double vertical)
    {
      _left = horizontal;
      _top = vertical;
      _right = horizontal;
      _bottom = vertical;
    }

    void set(double left, double top, double right, double bottom)
    {
      _left = left;
      _top = top;
      _right = right;
      _bottom = bottom;
    }

    double left() const
    {
      return _left;
    }
    
    void setLeft(double left)
    {
      _left = left;
    }

    double topBottom() const
    {
      return _top + _bottom;
    }

    double leftRight() const
    {
      return _left + _right;
    }

    double top() const
    {
      return _top;
    }
    
    void setTop(double top)
    {
      _top = top;
    } 

    double right() const
    {
      return _right;
    }
    
    void setRight(double right)
    {
      _right = right;
    } 

    double bottom() const
    {
      return _bottom;
    }
    
    void setBottom(double bottom)
    {
      _bottom = bottom;
    }

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
