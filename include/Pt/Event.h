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
#include <new>

namespace Pt {

/** @brief Base class for typed events.

    %Event is the base of every event that an event loop can copy and
    dispatch. A loop does not know the concrete type when it stores an
    event, so the event clones itself into an %Allocator, destroys
    itself from that allocator, and reports a @c std::type_info that
    identifies the dynamic type. Slots subscribe to that type.

    %clone() and %destroy() forward to %onClone() and %onDestroy().
    %typeInfo() forwards to %onTypeInfo(). A derived class that does
    not use %BasicEvent must implement those three hooks. %clone()
    returns a new object of the same dynamic type, allocated from the
    given allocator. %destroy() undoes that allocation. The allocator
    is the loop's; the event does not own it.

    %copyConstruct() and %destruct() are helpers for those hooks.
    They placement-new and destroy a concrete %EventT through the
    allocator. Prefer %BasicEvent, which already calls them.

    Do not slice an %Event. Copying the base is not allowed as a
    value; the type is meant to be cloned through the allocator.
    Destroy an event only with the allocator that cloned it.

    @ingroup Pt-Core
*/
class Event
{
    public:
        /** @brief Destructor.
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
        /** @brief Constructor.
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

/** @brief Event base that clones, destroys and identifies a derived type T.

    Derive as @c class MyEvent : public BasicEvent<MyEvent>. @a T is
    the derived type. %onTypeInfo() returns @c typeid(T). %onClone()
    allocates @c sizeof(T) from the allocator and copy-constructs
    @a T. %onDestroy() runs the %BasicEvent destructor and returns
    that storage to the allocator.

    The derived class must be copy-constructible. It does not override
    the three hooks unless it needs a different allocation size or
    copy. Construction of %BasicEvent is protected, so it is only a
    base.

    @ingroup Pt-Core
*/
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
