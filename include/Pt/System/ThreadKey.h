/***************************************************************************
 *   Copyright (C) 2005 by Marc Boris Dürner                               *
 *   Copyright (C) 2004 by Christian Prochnow                              *
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
#include <Pt/Exception.h>
#include <Pt/NonCopyable.h>


namespace Pt {

namespace System {

    class PT_SYSTEM_API ThreadKeyBase : public NonCopyable {
        public:
            ThreadKeyBase() throw(SystemError);

            ThreadKeyBase(const ThreadKeyBase& k) throw(SystemError);

            ~ThreadKeyBase() throw();

            void set(void* ptr) throw(SystemError);

            void* get() const throw(SystemError);

        private:
            class ThreadKeyImpl* _impl;
    };


    //! Thread-specific storage template class
    template <class T>
    class ThreadKey: private ThreadKeyBase {
        public:
            ThreadKey() throw(SystemError)
            : ThreadKeyBase()
            {}

            ThreadKey(const ThreadKey& k) throw(SystemError)
            : ThreadKeyBase()
            { ThreadKeyBase::set( k.get() ); }

            ~ThreadKey() throw()
            {}

            ThreadKey& operator=(const ThreadKey& k) throw(SystemError)
            {
                ThreadKeyBase::set( k.get() );
                return *this;
            }

            ThreadKey& operator=(T* val) throw(SystemError)
            {
                ThreadKeyBase::set( static_cast<void*>(val) );
                return *this;
            }
    
            operator T*() throw(SystemError)
            { return (T*)ThreadKeyBase::get(); }
    };

} // !namespace System

} // !namespace P

#endif
