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
#include <Pt/System/Api.h>
#include <Pt/System/Mutex.h>
#include <list>
#include <map>
#include <typeinfo>

namespace Pt {

namespace System {


struct PT_SYSTEM_API CompareTypeInfo2
{
    bool operator()(const std::type_info* t1, 
                    const std::type_info* t2) const

    {
        return t1->before(*t2) != 0;
    }
};


class IEventHandler
{
    public:
        virtual ~IEventHandler() {}

        virtual IEventHandler* clone() const = 0;

        virtual void send(const Pt::Event& ev) = 0;

        virtual bool involves(const void* object) const = 0;
};


template <typename C, typename EventT>
class EventHandler : public IEventHandler
{
    public:
        EventHandler(Method<void, C, EventT>& m)
        : _method(m)
        {}

        virtual ~EventHandler()
        {}

        virtual IEventHandler* clone() const
        {
            return new EventHandler(*this);
        }

        virtual void send(const Pt::Event& ev)
        {
            const EventT& event = static_cast<const EventT&>(ev);
            _method.call(event);
        }

        virtual bool involves(const void* object) const
        { return object == &(_method.object()); }

    private:
        Method<void, C, EventT> _method;
};


class EventSink;

/** @brief Sends Events to receivers in other threads

    The Signal class is not thread-safe and can only be used for
    intra-thread communication. To pass Events between different threads
    use an %EventSource instead. Thread-safety only refers to the usage
    of the %EventSource itself (connection, disconnecting...) and not the
    slot.
*/
class PT_SYSTEM_API EventSource : public NonCopyable
{
    friend class EventSink;

    public:
        EventSource();

        ~EventSource();

        void connect(EventSink& sink);

        void disconnect(EventSink& sink);

        void send(const Pt::Event& ev);

        template <typename SinkT, typename EventT>
        void subscribe(SinkT& sink)
        {
            MutexLock lock(_mutex);

            const std::type_info& ti = typeid(EventT);
            IEventHandler* handler = new EventHandler<SinkT, EventT>( callable( sink, &SinkT::commitEvent ) );
            _handlers.insert( std::make_pair(&ti, handler) );
            sink.addSource(*this);
        }

        template <typename EventT>
        void unsubscribe(EventSink& sink)
        {
            const std::type_info& ti = typeid(EventT);
            this->disconnect( sink, typeid(EventT) );
        }

    protected:
        void removeSink(EventSink& sink);

        void disconnect(EventSink& sink, const std::type_info& ti);

    private:
        typedef std::multimap< const std::type_info*,
                               IEventHandler*,
                               CompareTypeInfo2 > HandlerMap;

        HandlerMap _handlers;

		struct Sentry
		{
			Sentry(const EventSource* es);
			~Sentry();
			void detach();
			bool operator!() const;
			const EventSource* _es;
		};

        mutable Mutex _mutex;
        mutable std::list<EventSink*> _sinks;
        mutable Sentry* _sentry;
        mutable bool _sending;
        mutable bool _dirty;
};

} // namespace System

} // namespace Pt

#endif

