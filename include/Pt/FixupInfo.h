/*
 * Copyright (C) 2005-2013 by Dr. Marc Boris Duerner
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

#ifndef Pt_FixupInfo_h
#define Pt_FixupInfo_h

#include <Pt/Api.h>
#include <Pt/SerializationError.h>
#include <typeinfo>

namespace Pt {

class FixupInfo
{
    public:
        typedef void (*FixupHandler)(void* fixme,
                                     void* target,
                                     const std::type_info& targetType,
                                     unsigned m);

    public:
        FixupInfo(void* target, const std::type_info& targetType, unsigned mid)
        : _target(target)
        , _type(&targetType)
        , _mid(mid)
        {}

        ~FixupInfo()
        {}

        void* target() const
        { return _target; }

        const std::type_info& targetType() const
        { return *_type; }

        unsigned memberId() const
        { return _mid; }

        bool isNull() const
        { return _target == 0; }

        template <typename T>
        T* getTarget() const
        {
            if( _target == 0 || typeid(T) != *_type )
            {
                throw SerializationError("fixup type mismatch");
            }

            return static_cast<T*>( _target );
        }

        /** @internal

            This is needed as a workaround for some compilers (GCC 3.x) to
            allow access to 'T* getTarget() const'.
         */
        template <typename T>
        friend T getTarget(FixupInfo* fi);

    private:
        void* _target;
        const std::type_info* _type;
        unsigned _mid;
};


template <typename T>
struct FixupThunk
{
    static void fixupPointer(void* fixme,
                             void* target,
                             const std::type_info& targetType,
                             unsigned mid)
    {
        T** from = static_cast<T**>(fixme);
        FixupInfo fi(target, targetType, mid);
        fixup(fi, *from);
    }

    static void fixupReference(void* fixme,
                               void* target,
                               const std::type_info& targetType,
                               unsigned mid)
    {
        T* from = static_cast<T*>(fixme);
        FixupInfo fi(target, targetType, mid);
        fixup(fi, *from);
    }
};


template <typename T>
inline void fixup(const FixupInfo& fixup, T*& fixme)
{
    fixme = 0;

    if( ! fixup.isNull() )
    {
        fixme = fixup.getTarget<T>();
    }
}


template <typename T>
inline void fixup(const FixupInfo& fixup, T& fixme)
{
    T* to = fixup.getTarget<T>();
    fixme = *to;
}

} // namespace Pt

#endif
