/***************************************************************************
 *   Copyright (C) 2005-2008 by Dr Marc Boris Duerner                       *
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
#if !defined(PT_SYSTEM_THREAD_H)
#define PT_SYSTEM_THREAD_H

#include <Pt/System/Api.h>
#include <Pt/NonCopyable.h>
#include <Pt/System/Runnable.h>

namespace Pt {

namespace System {

    /** @brief Platform independent threads

        A Thread represents a separate thread of control within the program.
        It shares data with all the other threads within the process but
        executes independently in the way that a separate program does on a
        multitasking operating system. Threads can either run as Joinable,
        so you can wait for them, or be Detached, so they run indepentently.

        The execution of a thread starts either by calling its virtual method
        Thread::run(), which can be reimplemented in a derived class.
        Alternatively, an instance of Runnable object can be passed to the
        constructor. To create your own threads, subclass %Runnable and
        reimplement run().

        For example:
        @code
        class MyRunnable : public Runnable
        {
            public:
                void run();
        };

        MyRunnable runnable;
        Thread     thread(runnable);
        thread.start();
        @endcode

        A Thread can be easily given its own Eventloop, since the EventLoop
        is a %Runnable. This makes it possible to use certain classes that
        require the presence of an %EventLoop such as a Timer or perform
        I/O multiplexing in a dedicated thread. At the same time the %Thread
        can be controlled by sending it %Events.

        @code
        class AsyncObject : public Pt::System::Thread
        {
            public:
                AsyncObject()
                : Thread( Pt::System::Thread::Joinable)
                {
                    Pt::System::Thread::setRunnable(_loop);
                    connect(_loop.event, *this, AsyncObject::processEvent);
                }

                AsyncObject()
                {
                    _loop.exit();
                    Pt::System::Thread::wait();
                }

                // callback to handle events
                void processEvent(Pt& Event& ev);

            private:
                EventLoop _loop;
        };
        @endcode

        Each thread gets its own stack, which size is determinated by the
        operating system. A thread can be forced to terminate by calling
        Thread::terminate(), however, doing so is dangerous and discouraged.
        Thread also provides platform independent sleep function.
        Thread::start begins the execution by calling the reimplemented
        Thread::run member function. If the Thread is Joinable, it can be
        waited on by calling Thread::wait on it.
        A thread can give up CPU time either by calling Thread::yield() or
        Thread::sleep() to stop for a specified periode of time.
    */
    class PT_SYSTEM_API Thread : public NonCopyable
    {
        friend class ThreadImpl;

        private:
            class ThreadImpl* _impl;

        public:
            enum State
            {
                Ready    = 0,
                Running  = 1,
                Finished = 2
            };

            enum Mode
            {
                Joinable = 0,
                Detached = 1
            };

        public:
            /** @brief Contruct a thread with a runnable and mode.

                Constructs a thread object to execute the \a runnable. The
                Thread is not started on construction, but when Thread::start()
                is called. The \a mode can either be Detached or Joinable.
            */
            Thread(Runnable& runnable, Mode mode = Joinable);

            /** @brief Destructor

                Deleting a running joinable Thread (i.e. state is Running )
                will block until the thread ha finished. You can wait()
                on a thread to make sure that it has finished. Detached
                Threads do not depend on the Thread object lifetime.
            */
            virtual ~Thread();

            //! @brief Returns the current mode of the thread.
            Mode mode() const;

            //! @brief Returns the current state of the thread.
            State state() const;

            //! @brief Returns true if thread is detached.
            bool detached() const;

            //! @brief Returns true if thread is joinable.
            bool joinable() const;

            /** @brief Starts the thread

                This starts the execution of the thread. This means Thread::run()
                will be called, which needs to be overriden in derived classes.
                Returns a self reference for error checking
            */
            void start();

            /** @brief Wait until a joinable thread has exited.
            */
            void wait();

            /** @brief Terminates the thread.

                Terminates the thread without respect for any allocated resources.
                Use with caution.
            */
            void terminate();

            /** @brief Detaches a joinable thread.

                Detached threads can not be waited on and become independent of the lifetime
                of the Thread object, since no resources need to be reclaimed for them.
            */
            void detach();

            /** @brief Exits a joinable thread.

                This function is meant to be called from within a thread. Thread::exit()
                is implicitly called when Thread::run() returns.
            */
            static void exit();

            /** @brief Yield CPU time

                This function is meant to be called from within a thread.
            */
            static void yield();

            /** @brief Sleep for some time

                The calling thread sleeps for \a ms milliseconds.
            */
            static void sleep(unsigned int ms);

        protected:
            /** @brief Constructor a joinable or detached thread
            */
            Thread(Mode mode = Joinable);

            /** @brief Thread entry point

                This function needs to be overridden by derived classes.
                Starting the thread causes the object's run method to be called
                in that separately executing thread.
            */
            virtual void run();

        private:
            Runnable* _runnable;
    };

} // !namespace System

} // !namespace Pt

#endif // PT_Thread_H
