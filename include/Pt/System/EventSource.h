/***************************************************************************
 *   Copyright (C) 2006 Marc Boris Drner                                  *
 *                                                                         *
 ***************************************************************************/

#ifndef PT_SYSTEM_EVENTSOURCE_H
#define PT_SYSTEM_EVENTSOURCE_H

#include <Pt/Connectable.h>

#include <Pt/System/Api.h>
#include <Pt/System/Event.h>
#include <Pt/System/Mutex.h>
#include <Pt/System/MutexLock.h>

#include <list>


namespace Pt {

namespace System {

	class EventLoop;

    class EventSource : public Connectable, public NonCopyable
    {
        public:
            typedef Pt::Invokable< const Pt::Event&, Pt::Void, Pt::Void> Invokable;

        public:
            EventSource()
            { }

            ~EventSource()
            { this->shutDown(); }

            Connection connect( EventLoop& receiver )
            {
                // Do not lock here, the Connection will call
                // Connectable::opened on this object
                return Connection(*this, slot(&receiver, &EventLoop::commitEvent).clone() );
            }

            virtual void opened(const Connection& c)
            {
                MutexLock lock(_mutex);
                Connectable::opened(c);
            }

            virtual void closed(const Connection& c)
            {
                MutexLock lock(_mutex);
                Connectable::closed(c);
            }

            inline void send(const Pt::Event& ev) const
            {
                MutexLock lock(_mutex);

                std::list<Connection>::const_iterator it = Connectable::connections().begin();
                for(; it != _connections.end(); ++it)
                {
                    if( false == it->valid() || &( it->sender() ) != this  )
                        continue;

                    const Invokable* invokable = static_cast<const Invokable*>( it->slot().callable() );
                    invokable->invoke(ev);
                }
            }


        protected:
            void shutDown()
            {
                Connectable::shutDown();

                Connection connection;

                while( true )
                {
                    {
                        MutexLock lock( _mutex );

                         if( _connections.empty() )
                            break;

                         connection = _connections.front();
                        _connections.remove( connection );
                    }

                    connection.close();
                }

            }

        private:
            mutable Mutex _mutex;
    };

} // namespace System

} // namespace Ptv

#endif
