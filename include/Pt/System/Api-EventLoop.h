/* Copyright (C) 2005-2013 by Dr. Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
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
