/*
 * Copyright (C) 2006-2007 Laurentiu-Gheorghe Crisan
 * Copyright (C) 2006-2008 Marc Boris Duerner
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

#ifndef PT_SYSTEM_EVENTSOURCE_H
#define PT_SYSTEM_EVENTSOURCE_H

#include <Pt/System/Api.h>
#include <Pt/System/Mutex.h>
#include <Pt/System/EventSink.h>
#include <Pt/Event.h>
#include <Pt/Signal.h>
#include <map>
#include <typeinfo>

namespace Pt {

namespace System {

class EventSink;

/** @brief Sends events to sinks in other threads.

    %Signal is not thread-safe and is for use inside one thread.
    %EventSource sends a %Event to connected %EventSink objects and
    synchronizes connect, disconnect, and send. An %EventLoop is an
    %EventSink, so a source in one thread can queue events for a loop
    in another.

    connect() adds a sink. disconnect() removes it. subscribe() limits
    a sink to one event type. unsubscribe() removes that limit. send()
    delivers to every connected sink that accepts the type.

    @ingroup Pt-System-EventLoop
*/
class PT_SYSTEM_API EventSource : private NonCopyable
{
    friend class EventSink;

    public:
        /** @brief Default Constructor.
        */
        EventSource();

        /** @brief Destructor.
        */
        ~EventSource();

        /** @brief Send the event to all connected sinks.
        */
        void send(const Pt::Event& ev);

        /** @brief Connect to an EventSink.
        */
        void connect(EventSink& sink);

        /** @brief Disonnect from an EventSink.
        */
        void disconnect(EventSink& sink);

        /** @brief Send events of a certain type to the sink.
        */
        template <typename EventT>
        void subscribe(EventSink& sink)
        {
            subscribe( sink, typeid(EventT) );
        }

        /** @brief Do not send events of a certain type to the sink anymore.
        */
        template <typename EventT>
        void unsubscribe(EventSink& sink)
        {
            unsubscribe( sink, typeid(EventT) );
        }

    private:
        //! @internal
        bool tryDisconnect(EventSink& sink);

        //! @internal
        void subscribe(EventSink& sink, const std::type_info& ti);

        //! @internal
        void unsubscribe(EventSink& sink, const std::type_info& ti);

    private:
        struct Sentry;

        typedef std::multimap< const std::type_info*,
                               EventSink*,
                               CompareEventTypeInfo > SinkMap;

        mutable RecursiveMutex _mutex;
        mutable RecursiveMutex* _dmutex;
        mutable SinkMap _sinks;
        mutable Sentry* _sentry;
        mutable bool _dirty;
};

} // namespace System

} // namespace Pt

#endif // PT_SYSTEM_EVENTSOURCE_H
