/***************************************************************************
 *   Copyright (C) 2006 by Marc Boris Duerner                              *
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

            explicit RefCounted(atomic_t refs)
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
      mutable volatile atomic_t _refs;

      public:
        AtomicRefCounted()
        : _refs(0)
        { }

        explicit AtomicRefCounted(unsigned refs)
        : _refs(refs)
        { }

        virtual ~AtomicRefCounted()
        { }

        virtual atomic_t addRef()
        { return atomicIncrement(_refs); }

        virtual void release()
        { if (atomicDecrement(_refs) == 0) delete this; }

        atomic_t refs() const
        { return atomicGet(_refs); }
  };

}

#endif // PT_REFCOUNTED_H

