/*
 * Copyright (C) 2008-2013 by Marc Boris Duerner
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

#ifndef PT_EVENT_H
#define PT_EVENT_H

#include <Pt/Api.h>
#include <Pt/Types.h>
#include <Pt/Allocator.h>
#include <typeinfo>

namespace Pt {

/** @brief Base class for all event types.

    Specific Event objects, subclass from Event and implement the onClone(),
    onDestroy() and onTypeInfo() methods. The first two are used to copy event
    objects for example in an EventLoop and the latter one is used to dispatch
    events by type.

    @ingroup CoreTypes
 */
class Event
{
    public:
        /** \brief Destructor.
         */
        virtual ~Event()
        {}

        /** @brief Clones this event using an allocator.
        */
        Event& clone(Allocator& allocator) const
        { return onClone(allocator); }

        /** @brief Destroys this event using an allocator.
        */
        void destroy(Allocator& allocator)
        { onDestroy(allocator); }

        /** @brief Returns the type info for this class of events.
        */
        const std::type_info& typeInfo() const
        { return onTypeInfo(); }

    protected:
        /** \brief Constructor.
         */
        Event()
        {}
        
        /** @brief Clones this event using an allocator.
        */
        virtual Event& onClone(Allocator& allocator) const = 0;

        /** @brief Destroys this event using an allocator.
        */
        virtual void onDestroy(Allocator& allocator) = 0;

        /** @brief Returns the type info for this class of events.
        */
        virtual const std::type_info& onTypeInfo() const = 0;

    public:
        /** @brief Copies an event using an allocator.
        */
        template <typename EventT>
        static Event& copyConstruct(const EventT& ev, Allocator& allocator)
        {
            void* mem = allocator.allocate( sizeof(EventT) );
            EventT* pev = new (mem) EventT(ev);
            return *(pev);
        }

        /** @brief Destructs an event using an allocator.
        */
        template <typename EventT>
        static void destruct(EventT& ev, Allocator& allocator)
        {
            ev.~EventT();
            allocator.deallocate(&ev, sizeof(EventT));
        }
};

template <typename T>
class BasicEvent : public Event
{
    protected:
        BasicEvent()
        {}

        virtual const std::type_info& onTypeInfo() const
        { return typeid(T); }

        virtual Event& onClone(Allocator& allocator) const
        {
            void* pEvent = allocator.allocate(sizeof(T));
            return *(new (pEvent)T(*static_cast<const T*>(this)));
        }

        virtual void onDestroy(Allocator& allocator)
        {
            this->~BasicEvent();
            allocator.deallocate(this, sizeof(T));
        }
};

} // namespace Pt

#endif
