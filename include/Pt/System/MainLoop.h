/*
 * Copyright (C) 2006-2010 Marc Boris Duerner
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

#ifndef PT_SYSTEM_MAINLOOP_H
#define PT_SYSTEM_MAINLOOP_H

#include <Pt/System/Api.h>
#include <Pt/System/EventLoop.h>

namespace Pt {

namespace System {

class MainLoopImpl;

/** @brief Thread-safe event loop supporting I/O multiplexing and Timers.

    The following example uses a %MainLoop to wait on acitvity on
    a Timer, which is set to time-out after 1000 msecs.

    @code
    // slot to handle timer activity
    void onTimer();

    int main()
    {
        using Pt::System;

        MainLoop loop;

        Timer timer;
        timer.setActive(loop);
        timer.start(1000);
        timer.timeout() += Pt::slot(onTimer);

        loop.run();
        return 0;
    }
    @endcode
*/   
class PT_SYSTEM_API MainLoop : public EventLoop
{
    public:
        /** @brief Default Constructor
        */
        MainLoop();

        /** @brief Construct with allocator.
        */
        MainLoop(Allocator& a);

        /** @brief Destructor
        */
        virtual ~MainLoop();

        //! @internal
        Selector& selector();

        //! @internal
        bool waitNext();

        MainLoopImpl* impl()
        { return _impl; }

    protected:
        virtual void onAttachSelectable(Selectable&);

        virtual void onDetachSelectable(Selectable&);

        virtual void onCancel(Selectable& s);

        virtual void onReady(Selectable& s);

        virtual void onRun();

        virtual void onExit();
    
        virtual void onCommitEvent(const Event& ev);
    
        virtual void onQueueEvent(const Event& ev);
    
        virtual void onWake();

        virtual void onProcessEvents();
    
        virtual void onAttachTimer(Timer& timer);
    
        virtual void onDetachTimer(Timer& timer);

    private:
        MainLoopImpl* _impl;
};

} // namespace System

} // namespace Pt

#endif // PT_SYSTEM_MAINLOOP_H
