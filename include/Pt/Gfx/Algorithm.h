/* Copyright (C) 2010-2016 Marc Boris Duerner 
   Copyright (C) 2006-2010 by Aloysius Indrayanto

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

#ifndef PT_GFX_ALGORITHM_H
#define PT_GFX_ALGORITHM_H

#include <Pt/Gfx/Api.h>

namespace Pt {

namespace Gfx {

template<typename InputIteratorT, typename OutputIteratorT>
void copy(InputIteratorT from, InputIteratorT fromEnd, OutputIteratorT to)
{
    for( ; from != fromEnd; ++from, ++to)
        *to = *from;
}


template<typename OutputIteratorT, typename T>
void fill(OutputIteratorT to, OutputIteratorT toEnd, const T& value)
{
    for (; to != toEnd; ++to)
        *to = value;
}


template<typename InputIteratorT, typename OutputIteratorT, typename OperationT>
void transform(InputIteratorT from, InputIteratorT fromEnd, 
               OutputIteratorT to, OperationT op)
{
    for( ; from != fromEnd; ++from, ++to)
        op(*to, *from);
}


template<typename IteratorT, typename OperationT>
void transform(IteratorT begin, IteratorT end, OperationT op)
{
    for( ; begin != end; ++begin) 
        op(*begin);
}

} // namespace

} // namespace

#endif
