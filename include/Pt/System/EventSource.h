/***************************************************************************
 *   Copyright (C) 2006-2007 Laurentiu-Gheorghe Crisan                     *
 *   Copyright (C) 2006-2008 Marc Boris Duerner                            *
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
#ifndef PT_SYSTEM_EVENTSOURCE_H
#define PT_SYSTEM_EVENTSOURCE_H

#include <Pt/Event.h>
#include <Pt/Signal.h>
#include <Pt/System/Api.h>
#include <Pt/System/Mutex.h>
#include <Pt/System/EventSink.h>
#include <map>
#include <typeinfo>

namespace Pt {

namespace System {

class EventSink;

/** @brief Sends Events to receivers in other threads

    The Signal class is not thread-safe and can only be used for
    intra-thread communication. To pass Events between different threads
    use an %EventSource instead. Thread-safety only refers to the usage
    of the %EventSource itself (connection, disconnecting...) and not the
    slot.
    Construction and destruction must always occur in isolation.
*/
class PT_SYSTEM_API EventSource : protected NonCopyable
{
    friend class EventSink;

    struct Sentry
    {
        Sentry(const EventSource* es);

        ~Sentry();

        void detach();

        bool operator!() const;

        const EventSource* _es;
    };

    public:
        EventSource();

        ~EventSource();

        void send(const Pt::Event& ev);

        void connect(EventSink& sink);

        void disconnect(EventSink& sink);

        template <typename EventT>
        void subscribe(EventSink& sink)
        {
            subscribe( sink, typeid(EventT) );
        }

        template <typename EventT>
        void unsubscribe(EventSink& sink)
        {
            unsubscribe( sink, typeid(EventT) );
        }

    private:
        bool tryDisconnect(EventSink& sink);

        void subscribe(EventSink& sink, const std::type_info& ti);

        void unsubscribe(EventSink& sink, const std::type_info& ti);

    private:
        typedef std::multimap< const std::type_info*,
                               EventSink*,
                               CompareEventTypeInfo > SinkMap;

        mutable Mutex _mutex;
        mutable Mutex* _dmutex;
        mutable SinkMap _handlers;
        mutable Sentry* _sentry;
        mutable bool _sending;
        mutable bool _dirty;
};

} // namespace System

} // namespace Pt

#endif

