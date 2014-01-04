/*
 * Copyright (C) 2006 by Marc Boris Duerner
 * Copyright (C) 2010-2010 by Aloysius Indrayanto
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

#ifndef PT_REFCOUNTED_H
#define PT_REFCOUNTED_H

#include <Pt/Api.h>
#include <Pt/Atomicity.h>
#include <Pt/NonCopyable.h>

namespace Pt {

class RefCounted : private NonCopyable
{
    public:
        RefCounted()
        : _refs(0)
        { }

        explicit RefCounted(unsigned refs)
        : _refs(refs)
        { }

        virtual ~RefCounted()
        { }

        virtual unsigned addRef()
        { return ++_refs; }

        virtual void release()
        {
            if(--_refs == 0)
                delete this;
        }

        unsigned refs() const
        { return _refs; }

    private:
        unsigned _refs;
};

class AtomicRefCounted : private NonCopyable
{
    public:
        AtomicRefCounted()
        : _refs(0)
        { }

        explicit AtomicRefCounted(unsigned refs)
        : _refs(refs)
        { }

        virtual ~AtomicRefCounted()
        { }

        virtual int addRef()
        { return atomicIncrement(_refs); }

        virtual void release()
        { if (atomicDecrement(_refs) == 0) delete this; }

        int refs() const
        { return atomicGet(_refs); }

    private:
        mutable volatile atomic_t _refs;
};

} // namespace Pt

#endif // PT_REFCOUNTED_H

