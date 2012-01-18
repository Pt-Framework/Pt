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

#include "../SelectableList.h"
#include "EventLoopImpl.h"
#include "Pt/System/Api.h"
#include "Pt/System/Selectable.h"
#include "Pt/System/EventLoop.h"
#include "Pt/System/Mutex.h"
#include <iostream>
#include <vector>
#include <set>
#include <deque>
#include <windows.h>

namespace Pt {

namespace System {

class HandleMap
{
    public:
        HandleMap()
        {}

        void add(HANDLE h)
        {
            _handles.push_back(h);
            _selectables.push_back(0);
        }

        void add(IOHandle& handle)
        {
            _dirty.push_back(&handle);
        }

        void remove(IOHandle& handle)
        {
            std::vector<Selectable*>::iterator it = _selectables.begin();
            std::vector<HANDLE>::iterator hit =_handles.begin();
            while( it != _selectables.end() )
            {
                if(*it != handle.sel)
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

            _dirty.remove(&handle);
        }

        HANDLE* buildHandles()
        {
            std::list<IOHandle*>::iterator iter;
            for( iter = _dirty.begin(); iter != _dirty.end(); ++iter )
            {
                IOHandle* handle = *iter;
                _handles.push_back(handle->handle());
                _selectables.push_back(handle->sel);
            }
        
            _dirty.clear();

            if(_handles.empty())
                return 0;

            return &_handles[0];
        }

        size_t size() const
        { return _handles.size(); }

        Selectable* at(size_t n)
        { return _selectables[n]; }

    private:
        std::vector<HANDLE> _handles;
        std::vector<Selectable*> _selectables;
        std::list<IOHandle*> _dirty;
};


class PT_SYSTEM_API EventLoopImpl
{
    public:
        EventLoopImpl(Signal<const Pt::Event&>& eventSignal);
        
        ~EventLoopImpl();

        void run();

        void exit();

        void wake();

        void commitEvent(const Event& event);

        void queueEvent(const Event& event);

        bool processEvents();

        void attach(Timer& timer)
        { _timerQueue.addTimer(timer); }

        void detach(Timer& timer)
        { _timerQueue.removeTimer(timer); }

        void attach(Selectable& s);

        void detach(Selectable& s);

        void idle(Selectable& s);

        void avail(Selectable& s);

        void enableOverlapped(IOHandle& s);

        void disableOverlapped(IOHandle& s);

        void enable(IOHandle& handle);

        void disable(IOHandle& handle);

    protected:
        bool waitNext();

        virtual DWORD waitFor(DWORD numHandles, const HANDLE *handles, DWORD msecs, bool& isTimeout);

    private:
        Mutex _mutex;
        TimerQueue _timerQueue;
        EventQueue _eventQueue;
        Signal<const Event&>* _event;
        HANDLE _wakeEvent;
        HANDLE _ioEvent;
        HandleMap _handles;
        Selectable* _current;
        SelectableList _devices;
        SelectableList _selectables;
        std::vector<Selectable*> _avail;
};


class PT_SYSTEM_API MainLoopImpl : public EventLoopImpl
{
    public:
        MainLoopImpl(Signal<const Pt::Event&>& eventSignal);

        MainLoopImpl(Signal<const Pt::Event&>& eventSignal, Allocator& a);

        ~MainLoopImpl();
};

}//namespace System

}//namespace Pt

#endif
