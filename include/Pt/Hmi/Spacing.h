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
#ifndef Pt_Hmi_Spacing_H
#define Pt_Hmi_Spacing_H

#include <Pt/Types.h>
#include <Pt/Hmi/Api.h>

namespace Pt{
namespace Hmi{

class Spacing
{
  public:
    Spacing(Pt::ssize_t all)
    {
      set(all);
    }

    Spacing(Pt::ssize_t horizontal, Pt::ssize_t vertical)
    {
      set(horizontal, vertical);
    }

    Spacing(Pt::ssize_t left, Pt::ssize_t top, Pt::ssize_t right, Pt::ssize_t bottom)
    {
      set(left, top, right, bottom);
    }

    Spacing()    
    {
      set( 0, 0, 0, 0);
    }

    void set(Pt::ssize_t value)
    {
      set(value, value);
    }

    void set(Pt::ssize_t horizontal, Pt::ssize_t vertical)
    {
      _left = horizontal;
      _top = vertical;
      _right = horizontal;
      _bottom = vertical;
    }

    void set(Pt::ssize_t left, Pt::ssize_t top, Pt::ssize_t right, Pt::ssize_t bottom)
    {
      _left = left;
      _top = top;
      _right = right;
      _bottom = bottom;
    }

    Pt::ssize_t left() const
    {
      return _left;
    }
    
    void setLeft(Pt::ssize_t left)
    {
      _left = left;
    }

    Pt::ssize_t topBottom() const
    {
      return _top + _bottom;
    }

    Pt::ssize_t leftRight() const
    {
      return _left + _right;
    }

    Pt::ssize_t top() const
    {
      return _top;
    }
    
    void setTop(Pt::ssize_t top)
    {
      _top = top;
    } 

    Pt::ssize_t right() const
    {
      return _right;
    }
    
    void setRight(Pt::ssize_t right)
    {
      _right = right;
    } 

    Pt::ssize_t bottom() const
    {
      return _bottom;
    }
    
    void setBottom(Pt::ssize_t bottom)
    {
      _bottom = bottom;
    } 

  private:
    Pt::ssize_t _left;
    Pt::ssize_t _top;
    Pt::ssize_t _right;
    Pt::ssize_t _bottom;        
};

}}

#endif
