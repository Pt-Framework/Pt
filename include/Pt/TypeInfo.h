/*
 * Copyright (C) 2004-2013 Marc Boris Duerner
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

#ifndef PT_TYPEINFO_H
#define PT_TYPEINFO_H

#include <Pt/Api.h>
#include <Pt/Void.h>
#include <typeinfo>

namespace Pt {

/** @brief Extended API for std::type_info.

    @ingroup CoreTypes
*/
class TypeInfo
{
    public:
        TypeInfo()
        : m_ti(0)
        {
            m_ti = &typeid(Void);
        }

        TypeInfo(const std::type_info& ti)
        : m_ti(&ti)
        { }

        const std::type_info& get() const
        { return *m_ti; }

        bool before(const TypeInfo& rhs) const
        { return m_ti->before(*rhs.m_ti) != 0; }
        
    private:
        const std::type_info* m_ti;
};

inline bool operator==(const TypeInfo& lhs, const TypeInfo& rhs)
{ return (lhs.get() == rhs.get()) != 0; }

inline bool operator!=(const TypeInfo& lhs, const TypeInfo& rhs)
{ return !(lhs == rhs); }

inline bool operator<(const TypeInfo& lhs, const TypeInfo& rhs)
{ return lhs.before(rhs); }

inline bool operator>(const TypeInfo& lhs, const TypeInfo& rhs)
{ return rhs < lhs; }

inline bool operator<=(const TypeInfo& lhs, const TypeInfo& rhs)
{ return !(lhs > rhs); }

inline bool operator>=(const TypeInfo& lhs, const TypeInfo& rhs)
{ return !(lhs < rhs); }

} // namespace Pt

#endif // PT_TYPEINFO_H
