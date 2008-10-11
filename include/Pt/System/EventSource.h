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

    class EventSink;
    class EventSourceSentry;

    struct PT_SYSTEM_API CompareTypeInfo2
    {
        bool operator()(const std::type_info* t1, 
                        const std::type_info* t2) const

        {
            return t1->before(*t2) != 0;
        }
    };

    class PT_SYSTEM_API EventRouter : protected Pt::NonCopyable
    {
        public:
            struct IEventRoute
            {
                virtual ~IEventRoute() {}
                virtual void send(const Event&) = 0;
            };

            EventRouter()
            {}

            virtual ~EventRouter()
            {}

            template <typename EventT>
            IEventRoute* findRoute()
            {
                const std::type_info& ti = typeid(EventT);
                RouteMap::iterator it = _routes.find( &ti );

                if( it != _routes.end() )
                {
                    return it->second;
                }

                return 0;
            }

            void addRoute(IEventRoute& route)
            {}

            void removeRoute(IEventRoute& route)
            {}

            void send(const Event& ev)
            {}

        private:
            typedef std::map< const std::type_info*,
                              IEventRoute*,
                              CompareTypeInfo2 > RouteMap;

            RouteMap _routes;
    };


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
        friend class EventSourceSentry;

        public:
            // TODO call EventRoute
            struct IDispatcher
            {
                virtual ~IDispatcher() {}
                virtual void send(const Event&) = 0;
            };

            template <typename EventT>
            struct Dispatcher : public IDispatcher
            {
                typedef Invokable<EventT> Target;

                virtual void send(const Event& e)
                {
                    const EventT& event = static_cast<const EventT&>(e);
                    typename std::list<Target*>::iterator it;
                    for(it = _sinks.begin(); it != _sinks.end(); ++it)
                    {
                        (*it)->commitEvent(event);
                    }
                }

                void add(Target& target)
                { _sinks.push_back(&target); }

                void remove(Target& target)
                { _sinks.remove(&target); }

                std::list<Target*> _sinks;
            };

        public:
            EventSource();

            ~EventSource();

            void connect(EventSink& sink);

            void disconnect(EventSink& sink);

            void send(const Pt::Event& ev);

            template <typename EventT>
            void subscribe(EventSink& sink)
            {
                MutexLock lock(_mutex);

                const std::type_info& ti = typeid(EventT);
                DispatchTable::iterator it = _dispatchTable.find( &ti );

                Dispatcher<EventT>* disp = 0;
                if( it != _dispatchTable.end() )
                {
                    IDispatcher* d = it->second;
                    disp = static_cast<Dispatcher<EventT>*>(d);
                }
                else
                {
                    disp = new Dispatcher<EventT>;
                    std::pair<const std::type_info*const, EventSource::IDispatcher*> p( &ti, disp);
                    _dispatchTable.insert( p );
                }

                disp->add(sink);
            }

        protected:
            void removeSink(EventSink& sink);

        private:
            typedef std::map< const std::type_info*, 
                              IDispatcher*, 
                              CompareTypeInfo2 > DispatchTable;

            DispatchTable _dispatchTable;

            mutable Mutex _mutex;
            mutable std::list<EventSink*> _sinks;
            mutable EventSourceSentry* _sentry;
            mutable bool _sending;
            mutable bool _dirty;
    };






} // namespace System

} // namespace Pt

#endif

