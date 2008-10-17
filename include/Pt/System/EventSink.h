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
#include <Pt/Connectable.h>
#include <Pt/System/Api.h>
#include <Pt/System/Mutex.h>
#include <list>

namespace Pt {

namespace System {

    class EventSource;

    struct PT_SYSTEM_API CompareTypeInfo
    {
        bool operator()(const std::type_info* t1, 
                        const std::type_info* t2) const;
    };

    class EventSink : public Connectable
                    , protected NonCopyable
    {
        public:
            EventSink();

            virtual ~EventSink();

            void commitEvent(const Event& event);

        protected:
            virtual void onCommitEvent(const Event& event) = 0;

            bool opened(const Connection& c);

            void closed(const Connection& c);

        private:
            mutable Mutex _mutex;
    };

} // namespace System

} // namespace Ptv

#endif
