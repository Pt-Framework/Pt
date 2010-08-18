/***************************************************************************
 *   Copyright (C) 2006-2007 Laurentiu-Gheorghe Crisan                     *
 *   Copyright (C) 2006-2007 Marc Boris Duerner                            *
 *   Copyright (C) 2006-2007 Bjoern Oliver Streule                         *
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
#ifndef PT_SYSTEM_EventLoopImpl_H
#define PT_SYSTEM_EventLoopImpl_H

#include "SelectorImpl.h"
#include "Pt/System/Api.h"
#include "Pt/System/Selectable.h"
#include <iostream>
#include <vector>
#include <set>
#include <windows.h>

namespace Pt {

namespace System {

class EventLoopImpl
{
    public:
        EventLoopImpl();

        ~EventLoopImpl();

        void add( Selectable& s );

        void remove( Selectable& s );

        void changed(Selectable& s);

        WaitResult waitNext(std::size_t msecs);

        void wake();

        void setParent(Application* app)
        {
            _app = app;
        }

    private:
        HANDLE _wakeEvent;
        HANDLE _ioEvent;
        HandleMap _handles;
        std::set<Selectable*>::iterator _current;
        std::set<Selectable*>::iterator _currentAvail;
        std::set<Selectable*> _devices;
        std::set<Selectable*> _dirty;
        std::set<Selectable*> _avail;
        Application* _app;
};

}//namespace System

}//namespace Pt

#endif
