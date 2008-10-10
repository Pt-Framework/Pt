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
#ifndef PT_SYSTEM_EVENTSINK_H
#define PT_SYSTEM_EVENTSINK_H

#include <Pt/Event.h>
#include <Pt/System/Api.h>
#include <Pt/System/Mutex.h>
#include <list>

namespace Pt {

namespace System {

    class EventSource;

    class EventSink : protected NonCopyable
    {
        friend class EventSource;

        public:
            EventSink();

            virtual ~EventSink();

            /** @brief Adds an event and wakes up the loop.
             */
            void commitEvent(const Event& event);

        protected:
            virtual void onCommitEvent(const Event& event) = 0;

            void addSource(EventSource& source);
            void removeSource(EventSource& source);

        private:
            mutable Mutex _mutex;
            std::list<EventSource*> _sources;
    };

} // namespace System

} // namespace Ptv

#endif
