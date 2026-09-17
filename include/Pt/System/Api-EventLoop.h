/*
 * Copyright (C) 2005-2013 by Dr. Marc Boris Duerner
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

#ifndef PT_SYSTEM_API_EVENTLOOP_H
#define PT_SYSTEM_API_EVENTLOOP_H

/** @addtogroup Pt-System-EventLoop

    @brief Event loops, timers and console applications.

    %Application is the console process root. There is one instance.
    It uses an %EventLoop. run() enters that loop. exit() leaves it.
    Command line arguments, environment variables, the working
    directory, and C signals are %Application services.

    %EventLoop is the dispatch core of a thread or process.
    It is an %EventSink. commitEvent() queues an event and wakes the
    loop. queueEvent() queues without waking. wake() starts processing.
    Events are delivered on eventReceived in the thread that called
    run(). %MainLoop is the platform %EventLoop.

    A %Selectable attaches with setActive so the loop can wait for it.
    detach() removes it and cancels outstanding operations. %Timer is
    not a %Selectable. It registers with setActive and emits timeout
    at an interval.

    %EventSource sends events to %EventSink objects in other threads.
    %Signal is not thread-safe. %EventSource is.
*/

#endif
