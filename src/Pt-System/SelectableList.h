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

#include "Pt/System/Api.h"
#include "Pt/System/Selectable.h"
#include <iostream>
#include <cassert>

namespace Pt {

namespace System {

class SelectableListIterator
{
    public:
        SelectableListIterator()
        : _sel(0)
        {}

        explicit SelectableListIterator(Selectable* s)
        : _sel(s)
        {}

        SelectableListIterator& operator=(const SelectableListIterator& it)
        {
            _sel = it._sel;
            return *this;
        }

        SelectableListIterator& operator++()
        {
            assert(_sel);
            _sel = _sel->_next;
            return *this;
        }

        Selectable& operator*() const
        { return *_sel; }

        Selectable* operator->() const
        { return _sel; }

        bool operator ==(const SelectableListIterator& it) const
        { return _sel == it._sel; }

        bool operator !=(const SelectableListIterator& it) const
        { return _sel != it._sel; }

    private:
        Selectable* _sel;
};


class SelectableList : public Selectable
{
    public:
        typedef SelectableListIterator Iterator;

    public:
        SelectableList()
        {
            this->_next = this;
            this->_prev = this;
        }

        ~SelectableList()
        {
            this->_next = 0;
            this->_prev = 0;
        }

        bool empty() const
        { return this->_next == this; }

        Iterator end()
        { return Iterator(this); }

        Iterator begin()
        { return Iterator(this->_next); }

        void insert(Selectable& s)
        {
            Selectable* second = this->_next;
            this->_next = &s;
            second->_prev = &s;

            s._prev = this;
            s._next = second;
        }

        void remove(Selectable& s)
        {
            if(0 == s._next)
                return;

            assert(s._next && s._prev);
            s._prev->_next = s._next;
            s._next->_prev = s._prev;
            s._next = 0;
            s._prev = 0;
        }

        virtual void onCancel()
        { }

        virtual bool onRun()
        { return false; }
};

} // namespace System

} // namespace Pt

#endif

