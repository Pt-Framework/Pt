/*
 * Copyright (C) 2006-2012 Marc Boris Duerner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef PT_SYSTEM_SELECTOR_H
#define PT_SYSTEM_SELECTOR_H

#include "../SelectableList.h"
#include <Pt/WinVer.h>
#include <Pt/System/Api.h>
#include <Pt/System/IOError.h>
#include <Pt/System/Mutex.h>
#include <Pt/System/EventLoop.h>
#include <vector>
#include <list>
#include <windows.h>

namespace Pt {

namespace System {

class Selectable;

struct IOHandle
{
    IOHandle()
    : sel(0)
    , _handle(INVALID_HANDLE_VALUE)
    {}

    explicit IOHandle(Selectable& s)
    : sel(&s)
    , _handle(INVALID_HANDLE_VALUE)
    {}

    IOHandle(Selectable& s, HANDLE h)
    : sel(&s)
    , _handle(h)
    {}

    void init(Selectable& s)
    { sel = &s; }

    HANDLE handle()
    { return _handle; }

    void setHandle(HANDLE h)
    { _handle = h; }

    Selectable* sel;
    HANDLE _handle;
};

class IOTable
{
    public:
        IOTable()
        {}

        void add(HANDLE h)
        {
            _handles.push_back(h);
            _selectables.push_back(0);
        }

        void add(IOHandle& handle)
        {
            if(_handles.size() + _dirty.size() > MAXIMUM_WAIT_OBJECTS)
                throw IOError("too many I/O handles");

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

        DWORD size() const
        { return static_cast<DWORD>( _handles.size() ); }

        Selectable* at(size_t n)
        { return _selectables[n]; }

    private:
        std::vector<HANDLE> _handles;
        std::vector<Selectable*> _selectables;
        std::list<IOHandle*> _dirty;
};


class PT_SYSTEM_API Selector
{
    public:
        Selector();
        
        ~Selector();

        void wake();

        void attach(Selectable& s);

        void detach(Selectable& s);

        void enableOverlapped(IOHandle& s);

        void disableOverlapped(IOHandle& s);

        void enable(IOHandle& handle);

        void disable(IOHandle& handle);

        bool waitForWake(size_t msecs);

    protected:
        virtual DWORD waitFor(DWORD numHandles, const HANDLE *handles, DWORD msecs, bool& isTimeout);

    private:
        HANDLE _wakeEvent;
        HANDLE _ioEvent;
        IOTable _handles;
        Selectable* _current;
        SelectableList _devices;
        SelectableList _selectables;
};

} // namespace System

} // namespace Pt

#endif
