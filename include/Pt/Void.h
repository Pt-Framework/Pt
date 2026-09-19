/*
 * Copyright (C) 2005-2007 Marc Boris Duerner
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

#ifndef Pt_Void_h
#define Pt_Void_h

#include <Pt/TypeTraits.h>

namespace Pt {

/** @brief Marker type for an unused template argument.

    Some templates cannot use @c void as a parameter: @c void is not a
    complete type, and it cannot be a function argument or a member.
    %Void is an empty complete type that fills that slot. A traits
    specialization, a signal with no value, or a tuple-like parameter
    pack can name %Void to mean "this parameter is absent" without
    leaving the type system.

    %Void has no data and no operations of its own. Compare it with
    @c typeid or a template specialization, do not construct values
    of it as application data.

    @ingroup Pt-Core
*/
struct PT_API Void
{};

} // namespace Pt

#endif // Pt_Void_h
