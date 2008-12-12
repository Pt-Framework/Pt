/*
 * Copyright (C) 2005 by Marc Boris Duerner
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

#ifndef Pt_Reflex_CallableInfo_h
#define Pt_Reflex_CallableInfo_h

#include <Pt/Reflex/Api.h>
#include <Pt/Reflex/MemberInfo.h>
#include <Pt/Types.h>
#include <Pt/Any.h>


namespace Pt {

class Slot;

namespace Reflex {

    /** @brief Reflection support for callable entities
        @ingroup Reflection
    */
    class CallableInfo : public MemberInfo
    {
        public:
            virtual size_t argSize() const = 0;

            virtual const std::type_info& argType(size_t index) const = 0;

            virtual const std::type_info& retType() const = 0;

            virtual Pt::Any call(const Any* args, size_t argCount) = 0;

            virtual Slot* createSlot()
            { return 0; }

        protected:
            CallableInfo()
            {}
    };

} // namespace Reflex

} // namespace Pt

#endif
