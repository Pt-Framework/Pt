/*
 * Copyright (C) 2008-2012 Marc Boris Duerner
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

#ifndef PT_SELECTABLELIST_H
#define PT_SELECTABLELIST_H

#include <Pt/System/Api.h>
#include <Pt/System/Selectable.h>
#include <cassert>

namespace Pt {

namespace System {

class SelectableList : private Selectable
{
    public:
        SelectableList()
        {
            this->_next = 0;
            this->_prev = 0;
        }

        bool empty() const
        { return this->_next == 0; }

        Selectable* first()
        { return this->next(); }

        Selectable* head()
        { return this; }

        void insert(Selectable& s)
        {
            if(s._prev || s._next)
                unlink(s);
        
            Selectable* second = this->first();
            if(second)
                second->_prev = &s;
        
            this->head()->_next = &s;
            s._prev = this->head();
            s._next = second;
        }

        static void unlink(Selectable& s)
        {
            if(0 == s._prev)
                return;
        
            assert(s._prev);
        
            s._prev->_next = s._next;
        
            if(s._next)
                s._next->_prev = s._prev;
        
            s._next = 0;
            s._prev = 0;
        }

    protected:
        virtual void onAttach(EventLoop& loop)
        {}

        virtual void onDetach(EventLoop& loop)
        {}

        virtual void onCancel()
        { }

        virtual bool onRun()
        { return false; }
};

/*inline void unlink(Selectable& s)
{
    if(0 == s._prev)
        return;

    assert(s._prev);

    s._prev->_next = s._next;

    if(s._next)
        s._next->_prev = s._prev;

    s._next = 0;
    s._prev = 0;
}

inline void link(Selectable& s, SelectableList& list)
{
    assert(0 == s._prev);
    assert(0 == s._next);
    if(s._prev)
    {
        unlink(s);
    }

    Selectable* second = list.first();
    if(second)
        second->_prev = &s;

    list.head()->_next = &s;
    s._prev = list.head();
    s._next = second;
}*/

} // namespace System

} // namespace Pt

#endif // PT_SELECTABLELIST_H
