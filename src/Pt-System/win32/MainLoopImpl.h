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
#ifndef PT_SYSTEM_MainLoopImpl_H
#define PT_SYSTEM_MainLoopImpl_H

#include "Pt/System/Api.h"
#include "Pt/System/Selectable.h"
#include "Pt/System/EventLoop.h"
#include <iostream>
#include <vector>
#include <set>
#include <windows.h>

namespace Pt {

namespace System {

class HandleMap
{
    public:
        HandleMap()
        {}

        ~HandleMap()
        { }

        void add(HANDLE h, Selectable* s)
        {
            _handles.push_back(h);
            _selectables.push_back(s);
        }

        HANDLE* handles()
        {
            if(_handles.empty())
                return 0;

            return &_handles[0];
        }

        size_t size() const
        { return _handles.size(); }

        Selectable* at(size_t n)
        { return _selectables[n]; }

        void pop_front()
        {
            _selectables.erase( _selectables.begin() );
            _handles.erase( _handles.begin() );
        }

        void remove(Selectable& s)
        {
            if( _selectables.empty() )
                return;

            std::vector<Selectable*>::iterator it;
            std::vector<HANDLE>::iterator hit =_handles.begin();
            for(it = _selectables.begin(); it != _selectables.end(); )
            {
                if(*it != &s)
                {
                    ++it;
                    ++hit;
                }
                else
                {
                    it = _selectables.erase(it);
                    hit = _handles.erase(hit);
                }
            }
        }

    private:
        std::vector<HANDLE> _handles;
        std::vector<Selectable*> _selectables;
};


struct IOHandle
{
    IOHandle(Selectable& s)
    : sel(&s)
    , _handle(INVALID_HANDLE_VALUE)
    {}

    IOHandle(Selectable& s, HANDLE h)
    : sel(&s)
    , _handle(h)
    {}

    HANDLE handle()
    { return _handle; }

    Selectable* sel;
    HANDLE _handle;
};


class EventLoopImpl : public EventDispatcher
{
    public:
        EventLoopImpl();
        
        ~EventLoopImpl();

        void attach(Selectable& s);

        void detach(Selectable& s);

        void enable(Selectable& s);

        void disable(Selectable& s);

        void idle(Selectable& s);

        void active(Selectable& s);

        void avail(Selectable& s);

        HANDLE beginWait(Selectable& s)
        { 
            _devices.insert(&s);
            return _ioEvent; 
        }

        void endWait(Selectable& s)
        { 
            _devices.erase(&s);
        }

        IOHandle* registerHandle(Selectable& s, HANDLE h)
        {
            IOHandle* iohandle =  new IOHandle(s, h);
            _dirty.push_back(iohandle);
            return iohandle;
        }

        void unregisterHandle(IOHandle* h)
        {
            _dirty.remove(h);
            _handles.remove( *(h->sel) );
            delete h;
        }

        void setAvail(Selectable& s)
        {
            _avail.insert(&s);
        }

    protected:
        virtual void onRun();

        virtual void onWake();

         void waitNext(std::size_t timeout, bool& isActive);

        virtual DWORD waitFor(DWORD numHandles, const HANDLE *handles, DWORD msecs, bool& isTimeout);

    private:
        HANDLE _wakeEvent;
        HANDLE _ioEvent;
        HandleMap _handles;
        std::list<IOHandle*> _dirty;
        std::set<Selectable*>::iterator _current;
        std::set<Selectable*>::iterator _currentAvail;
        std::set<Selectable*> _attached;
        std::set<Selectable*> _devices;
        std::set<Selectable*> _avail;
};


class PT_SYSTEM_API MainLoopImpl : public EventDispatcher
{
    public:
        MainLoopImpl();

        MainLoopImpl(Allocator& a);

        ~MainLoopImpl();

        EventLoopImpl& impl()
        { return _impl; }

        void attach( Selectable& s );

        void detach( Selectable& s );

        void enable( Selectable& s );

        void disable( Selectable& s );

        void idle(Selectable& s);

        void active(Selectable& s);

        void avail(Selectable& s);

    protected:
        virtual void onRun();

        virtual void onWake();

        void waitNext(std::size_t timeout, bool& isActive);

        virtual DWORD waitFor(DWORD numHandles, const HANDLE *handles, DWORD msecs, bool& isTimeout);

    private:
        HANDLE _wakeEvent;
        HANDLE _ioEvent;
        HandleMap _handles;
        EventLoopImpl _impl;
        std::set<Selectable*>::iterator _current;
        std::set<Selectable*>::iterator _currentAvail;
        std::set<Selectable*> _attached;
        std::set<Selectable*> _devices;
        std::set<Selectable*> _dirty;
        std::set<Selectable*> _avail;
};

}//namespace System

}//namespace Pt

#endif
