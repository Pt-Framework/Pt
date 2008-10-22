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
#include <Pt/Method.h>
#include <Pt/Connectable.h>
#include <Pt/Signal.h>
#include <Pt/System/Api.h>
#include <Pt/System/Mutex.h>
#include <Pt/System/EventSink.h>
#include <list>
#include <map>
#include <typeinfo>

namespace Pt {

namespace System {

class IEventHandler
{
    public:
        virtual ~IEventHandler() {}

        virtual void send(const Pt::Event& ev) = 0;
};


template <typename EventT>
class EventHandler : public IEventHandler
{
    public:
        EventHandler(const Connection& target)
        : _target(target)
        { }

        virtual ~EventHandler()
        { }

        virtual void send(const Pt::Event& ev)
        {
            const EventT& event = static_cast<const EventT&>(ev);

        }

    private:
        Connection _target;
};


class EventDispatcher : public IConnectable
                      , protected NonCopyable
{
    public:
        EventDispatcher()
        {}

        void dispatch(const Pt::Event& ev);

        template <typename EventT>
        void subscribe( const BasicSlot<void, const EventT&>& slot )
        {
            Connection conn( *this, slot.clone() );

            IEventHandler* handler = new EventHandler<EventT>( conn );

            const std::type_info& ti = typeid(EventT);
            _handlers.insert( std::make_pair(&ti, handler) );
        }

        template <typename EventT>
        void unsubscribe( const BasicSlot<void, const EventT&>& slot )
        {
            const std::type_info& ti = typeid(EventT);
        }

        virtual bool opened(const Connection& c);

        virtual void closed(const Connection& c);

    private:
        typedef std::multimap< const std::type_info*,
                               IEventHandler*,
                               CompareTypeInfo > HandlerMap;

        HandlerMap _handlers;
};


class EventSink;


/** @brief Sends Events to receivers in other threads

    The Signal class is not thread-safe and can only be used for
    intra-thread communication. To pass Events between different threads
    use an %EventSource instead. Thread-safety only refers to the usage
    of the %EventSource itself (connection, disconnecting...) and not the
    slot.
*/
class PT_SYSTEM_API EventSource : protected NonCopyable
{
    friend class EventSink;

    public:
        EventSource();

        ~EventSource();

        void send(const Pt::Event& ev);

        void connect(EventSink& sink);

        void disconnect(EventSink& sink);

        template <typename EventT>
        void subscribe(EventSink& sink)
        {
        }

        template <typename EventT>
        void unsubscribe(EventSink& sink)
        {
        }

    private:
        /*struct Sentry
        {
            Sentry(const EventSource* es);
            ~Sentry();
            void detach();
            bool operator!() const;
            const EventSource* _es;
        };*/

        typedef std::multimap< const std::type_info*,
                               EventSink*,
                               CompareTypeInfo > HandlerMap;

        mutable Mutex _mutex;
        mutable Mutex _dmutex;
        mutable std::list<EventSink*> _sinks;
        HandlerMap _handlers;
        //mutable Sentry* _sentry;
        mutable bool _sending;
        mutable bool _dirty;
};

} // namespace System

} // namespace Pt

#endif

