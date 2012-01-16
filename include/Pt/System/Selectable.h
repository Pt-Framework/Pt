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
#ifndef PT_SELECTABLE_H
#define PT_SELECTABLE_H

#include <Pt/Types.h>
#include <Pt/Atomicity.h>
#include <Pt/NonCopyable.h>
#include <Pt/System/Api.h>
#include <Pt/System/EventLoop.h>
#include <cassert>

namespace Pt {

namespace System {

//! @brief Dispatches operations through an event loop
class PT_SYSTEM_API Selectable : protected NonCopyable
{
    friend class SelectableList;

    public:
        //! @brief Destructor
        virtual ~Selectable();

        //! @brief Sets the parent loop, so that operations can be run
        void setActive(EventLoop& parent);

        //! @brief Returns true if operations can be run
        bool isActive() const;

        //! @brief Remove from event loop and cancels outstanding operations
        void detach();

        //! @brief Returns the parent event loop in which operations are running
        EventLoop* parent() const;

        //! @brief Cancels all operations
        void cancel();

        //! @brief Run operation if it is ready
        bool run();

        //! @brief Notify readiness to the parent EventLoop 
        void setReady();

    protected:
        //! @brief Default Constructor
        Selectable();

        //! @brief Attached to loop
        //virtual void onAttach(EventLoop& loop) = 0;

        //! @brief Detached from loop
        //virtual void onDetach(EventLoop& loop) = 0;

        //! @brief Blocks until operation has cancelled
        virtual void onCancel() = 0;

        //! @brief Check if ready and run
        virtual bool onRun() = 0;

    private:
        EventLoop* _parent;
        Selectable* _prev;
        Selectable* _next;
};


class SelectableList
{
    public:
        class Iterator
        {
            public:
                Iterator()
                : _sel(0)
                {}

                explicit Iterator(Selectable* s)
                : _sel(s)
                {}

                Iterator& operator=(const Iterator& it)
                {
                    _sel = it._sel;
                    return *this;
                }
        
                Iterator& operator++()
                {
                    assert(_sel);
                    _sel = _sel->_next;
                    return *this;
                }
        
                Selectable& operator*() const
                { return *_sel; }
        
                Selectable* operator->() const
                { return _sel; }

                bool operator ==(const Iterator& it) const
                { return _sel == it._sel; }

                bool operator !=(const Iterator& it) const
                { return _sel != it._sel; }

            private:
                Selectable* _sel;
        };

    public:
        SelectableList()
        : _first(0)
        , _last(0)
        {}

        ~SelectableList()
        {}

        bool empty() const
        { return ! _first; }

        Iterator end()
        { return Iterator(); }

        Iterator begin()
        { return Iterator(_first); }

        void push_back(Selectable& s)
        {
            assert(s._prev == 0);
            assert(s._next == 0);

            s._prev = _last;
            _last = &s;

            if( ! _first)
                _first = &s;
        }

        void remove(Selectable& s)
        {
            assert(s._prev || _first == &s);
            Selectable* prev = s._prev;
            Selectable* next = s._next;
            s._prev = 0;
            s._next = 0;

            if(next)
            {
                next->_prev = prev;
            }
            else // last element
            {
                assert(&s == _last);
                _last = prev;
            }

            if(prev)
            {
                prev->_next = next;
            }
            else // first element
            {
                assert(&s == _first);
                _first = next;
            }
        }

    private:
        Selectable* _first;
        Selectable* _last;
};

} // namespace System

} // namespace Pt

#endif

