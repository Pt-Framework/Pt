/***************************************************************************
 *   Copyright (C) 2006-2007 Laurentiu-Gheorghe Crisan                     *
 *   Copyright (C) 2006-2007 Marc Boris Duerner                            *
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

#include <Pt/Connectable.h>
#include <Pt/System/Api.h>
#include <Pt/Event.h>
#include <Pt/Slot.h>
#include <Pt/System/Mutex.h>
#include <Pt/System/MutexLock.h>
#include <Pt/System/EventLoop.h>
#include <list>

namespace Pt {
namespace System {

	class EventLoop;

    /** Sends Events to receivers in other threads

        The Signal class is not thread-safe and can only be used for
        intra-thread communication. To pass Events between different threads
        use an %EventSource instead. Thread-safety only refers to the usage
        of the %EventSource itself (connection, disconnecting...) and not the
        slot.
    */
    class EventSource : public Connectable, public NonCopyable
    {
        public:
            typedef Pt::Invokable< const Pt::Event&, Pt::Void, Pt::Void> Invokable;

        public:
            /** @brief Constructs a new EventSource
            */
            EventSource()
            : _mutex(Pt::System::Mutex::Normal)
            { }

            /** @brief Destructs the EventSource
            */
            ~EventSource()
            {
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

            /** @brief Connects to a EventLoop in another thread
            */
            Connection connect( EventLoop& receiver )
            {
                // Do not lock here, the Connection will call
                // Connectable::opened on this object
                return Connection( *this, slot(receiver, &EventLoop::commitEvent).clone() );
            }

            /** @brief Connects to a slot in another thread
            */
            Connection connect( const Slot& aSlot )
            {
                // Do not lock here, the Connection will call
                // Connectable::opened on this object
                return Connection( *this, aSlot.clone() );
            }

            //! @internal
            virtual bool opened( const Connection& c )
            {
                MutexLock lock(_mutex);
                bool accept = Connectable::opened(c);
                return accept;
            }

            //! @internal
            virtual void closed( const Connection& c )
            {
                MutexLock lock(_mutex);
                Connectable::closed(c);
            }

            /** @brief Send an Event to all receivers
            */
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

        private:
            mutable Mutex _mutex;
    };

} // namespace System
} // namespace Ptv

#endif

