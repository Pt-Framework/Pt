/* Copyright (C) 2008 Marc Boris Duerner

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

#ifndef PT_API_BASICTYPES_H
#define PT_API_BASICTYPES_H

/** @defgroup BasicTypes Basic Types

    @brief Fixed-size integers, date and time types, and type-erased values.

    The %Pt framework defines a number of signed and unsigned,
    @link basictypes_FixedInt fixed-size integer@endlink types. These
    are typedefs of fundamental integer types and the actual type depends
    on the platform. 

    The classes that represent @link basictypes_DateTime dates and times@endlink,
    namely Pt::Date, Pt::Time, Pt::DateTime and Pt::Timespan, can be used for
    comparison, sorting or calculating dates, times and timespans. These types
    are serializable and conversion to ISO string format is supported as well.

    The class Pt::Any is able to contain any other copyable type. This is
    useful in situations where type erasure is required, for example, to store
    completely unrelated types in a list or vector.
*/

#endif
