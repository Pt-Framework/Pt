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
#include <Pt/Slot.h>
#include <Pt/Connectable.h>
#include <Pt/System/Api.h>
#include <Pt/System/Mutex.h>
#include <Pt/System/EventLoop.h>
#include <list>

namespace Pt {

namespace System {

    class EventLoop;

    /** @brief Sends Events to receivers in other threads

        The Signal class is not thread-safe and can only be used for
        intra-thread communication. To pass Events between different threads
        use an %EventSource instead. Thread-safety only refers to the usage
        of the %EventSource itself (connection, disconnecting...) and not the
        slot.
    */
    /*class PT_SYSTEM_API EventSource : public Connectable, public NonCopyable
    {
        public:
            //! @brief Constructs a new EventSource
            EventSource();

            //! @brief Destructs the EventSource
            ~EventSource();

            //! @brief Connects to a EventLoop in another thread
            Connection connect( EventLoopBase& receiver );

            //! @brief Connects to a slot in another thread
            Connection connect( const Slot& s );

            //! @internal
            virtual bool opened( const Connection& c );

            //! @internal
            virtual void closed( const Connection& c );

            //! @brief Send an Event to all receivers
            void send(const Pt::Event& ev) const;

        private:
            mutable Mutex _mutex;
    };*/


    class PT_SYSTEM_API EventSource : public Connectable, public NonCopyable
    {
        friend class EventSink;

        public:
            EventSource();

            ~EventSource();

            void connect(EventSink& sink);

            void disconnect(EventSink& sink);

            void send(const Pt::Event& ev) const;

        protected:
            void removeSink(EventSink& sink);

        private:
            mutable Mutex _mutex;
            std::list<EventSink*> _sinks;
    };

} // namespace System

} // namespace Pt

#endif

