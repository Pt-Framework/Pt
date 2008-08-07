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
#ifndef PT_SYSTEM_POSIX_SELECTORIMPL_H
#define PT_SYSTEM_POSIX_SELECTORIMPL_H

#include <Pt/System/Api.h>
#include <Pt/System/IODevice.h>
#include <Pt/System/Clock.h>
#include <sys/poll.h>
#include <vector>
#include <set>

namespace Pt {

namespace System {

class Application;

class SelectorImpl
{
    public:
        SelectorImpl();

        ~SelectorImpl();

        void add( Selectable& dev );

        void remove( Selectable& dev );

        bool wait(unsigned int msecs);

        void wake();

        void setApp(Application* app)
        {
            _app = app;
            _isDirty= true;
        }

        void onEnabled(Selectable& s)
        { _isDirty = true; }

        void onDisabled(Selectable& s)
        { _isDirty = true; }

    private:
        static const short POLL_ERROR_MASK;
        int _wakePipe[2];
        bool _isDirty;
        std::vector<pollfd> _pollfds;
        std::set<Selectable*>::iterator _current;
        std::set<Selectable*> _devices;
        Application* _app;
        Clock _clock;
};

}//namespace System

}//namespace Pt

#endif
