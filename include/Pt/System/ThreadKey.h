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
#ifndef Pt_ThreadKey_h
#define Pt_ThreadKey_h

#include <Pt/System/Api.h>
#include <Pt/NonCopyable.h>

namespace Pt {

namespace System {

    class PT_SYSTEM_API ThreadKeyBase : public NonCopyable {
        public:
            ThreadKeyBase();

            ThreadKeyBase(const ThreadKeyBase& k);

            ~ThreadKeyBase();

            void set(void* ptr);

            void* get() const;

        private:
            class ThreadKeyImpl* _impl;
    };


    //! Thread-specific storage template class
    template <class T>
    class ThreadKey: private ThreadKeyBase {
        public:
            ThreadKey()
            : ThreadKeyBase()
            {}

            ThreadKey(const ThreadKey& k)
            : ThreadKeyBase()
            { ThreadKeyBase::set( k.get() ); }

            ~ThreadKey()
            {}

            ThreadKey& operator=(const ThreadKey& k)
            {
                ThreadKeyBase::set( k.get() );
                return *this;
            }

            ThreadKey& operator=(T* val)
            {
                ThreadKeyBase::set( static_cast<void*>(val) );
                return *this;
            }

            T* get()
            { return (T*)ThreadKeyBase::get(); }

            const T* get() const
            { return (T*)ThreadKeyBase::get(); }
    };

} // !namespace System

} // !namespace P

#endif
