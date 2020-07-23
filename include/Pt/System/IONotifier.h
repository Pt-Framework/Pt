/*
 * Copyright (C) 2006-2018 Marc Boris Duerner
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

#ifndef Pt_System_IONotifier_h
#define Pt_System_IONotifier_h

#include <Pt/System/Api.h>
#include <Pt/System/IOError.h>
#include <Pt/System/Selectable.h>
#include <Pt/Types.h>
#include <Pt/Signal.h>

namespace Pt {

namespace System {

class PT_SYSTEM_API IONotifier : public Selectable
{
    public:
        enum WaitFlags
        {
            Read = 1,
            Write = 2,
            Except = 4
        };

    public:
        IONotifier();

        explicit IONotifier(void* handle);

        explicit IONotifier(int fd);

        ~IONotifier();

        void setFd(int fd);

        void setHandle(void* handle);

        void beginWait(int flags);

        int endWait();

        Pt::Signal<>& eventReady()
        { return _eventReady; } 

        /** @brief Returns the used event loop.
        */
        EventLoop* loop() const
          { return _loop; }

    protected:
        virtual void onAttach(EventLoop& loop);

        virtual void onDetach(EventLoop& loop);

        virtual void onCancel();

        virtual bool onRun();

    private:
        class IONotifierImpl* _impl;
        EventLoop*            _loop;
        bool                  _isWaiting;
        Pt::Signal<>          _eventReady;
};

} // namespace System

} // namespace Pt

#endif // Pt_System_IODevice_h
