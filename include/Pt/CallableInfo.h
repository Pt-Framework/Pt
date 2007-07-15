/***************************************************************************
 *   Copyright (C) 2005 by Marc Boris Dürner                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef Pt_CallableInfo_h
#define Pt_CallableInfo_h

#include <Pt/MemberInfo.h>

//! \addtogroup Pt
namespace Pt {

    class Args;
    class SerializationData;

    /** @brief Interface for callable entities
        @ingroup Reflection
    */
    class PT_API CallableInfo : public MemberInfo
    {
        public:
            virtual size_t argSize() const = 0;

            virtual const char* argName(size_t index) const = 0;

            virtual const std::type_info& argType(size_t index) const = 0;

            virtual const char* retName() const = 0;

            virtual const std::type_info& retType() const = 0;

            virtual Pt::Any call(const Any* args, size_t argCount) = 0;

            virtual void call(const SerializationData& args) const = 0;

        protected:
            CallableInfo()
            {}
    };

} // namespace Pt

#endif
