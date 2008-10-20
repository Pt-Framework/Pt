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

        virtual Slot& slot() = 0;
};


template <typename EventT>
class EventHandler : public IEventHandler
{
    public:
        EventHandler(BasicSlot<void, EventT>& slot)
        : _slot(0)
        {
            _slot = slot.clone();
        }

        virtual ~EventHandler()
        {
            delete _slot;
        }

        virtual void send(const Pt::Event& ev)
        {
            const EventT& event = static_cast<const EventT&>(ev);
            if(_slot)
                static_cast< const Invokable<EventT>* >(_slot)->invoke(event);
        }

        virtual Slot& slot()
        { return *_slot; }

    private:
        Slot* _slot;
};


class EventDispatcher
{
    public:
        EventDispatcher()
        {}

        void dispatch(const Pt::Event& ev);

        template <typename EventT>
        void subscribe(Slot& slot)
        {
            const std::type_info& ti = typeid(EventT);
            IEventHandler* handler = new EventHandler<EventT>( slot );
            _handlers.insert( std::make_pair(&ti, handler) );
        }

        template <typename EventT>
        void unsubscribe(Slot& slot)
        {
            const std::type_info& ti = typeid(EventT);
            this->unsubscribe( slot, typeid(EventT) );
        }

        void unsubscribe(const Slot& slot, const std::type_info& ti);

        void unsubscribeAll(const Slot& slot);

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
class PT_SYSTEM_API EventSource : protected Connectable
                                , protected NonCopyable
{
    public:
        EventSource();

        ~EventSource();

        void send(const Pt::Event& ev);

        template <typename EventT>
        void subscribe(EventSink& sink)
        {
            MutexLock lock(_mutex);
            Connection conn(*this, sink, &EventSink::commitEvent);
            _dispatcher.subscribe<EventT>( conn.slot() );
        }

        template <typename EventT>
        void unsubscribe(EventSink& sink)
        {
            MutexLock lock(_mutex);
            this->unsubscribe( sink, typeid(EventT) );
        }

    protected:
        bool opened(const Connection& c);

        void closed(const Connection& c);

        void unsubscribe(EventSink& sink, const std::type_info& ti);

    private:
        typedef std::multimap< const std::type_info*,
                               IEventHandler*,
                               CompareTypeInfo > HandlerMap;

        HandlerMap _handlers;

		EventDispatcher _dispatcher;

		struct Sentry
		{
			Sentry(const EventSource* es);
			~Sentry();
			void detach();
			bool operator!() const;
			const EventSource* _es;
		};

        mutable Mutex _mutex;
        mutable Mutex _dmutex;
        mutable Sentry* _sentry;
        mutable bool _sending;
        mutable bool _dirty;
};

} // namespace System

} // namespace Pt

#endif

