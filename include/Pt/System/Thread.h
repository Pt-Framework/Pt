/*
 * Copyright (C) 2006-2013 Marc Boris Duerner
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

#ifndef PT_SYSTEM_THREAD_H
#define PT_SYSTEM_THREAD_H

#include <Pt/System/Api.h>
#include <Pt/NonCopyable.h>
#include <Pt/Callable.h>
#include <Pt/Function.h>
#include <Pt/Method.h>

namespace Pt {

namespace System {

class EventLoop;

/** @brief Platform independent threads

    This is a thread base class, which is flexible, but hard to use. Try
    to use either an AttachedThread or a DetachedThread instead.

    A %Thread represents a separate thread of control within the program.
    It shares data with all the other threads within the process but
    executes independently in the way that a separate program does on a
    multitasking operating system. Each thread gets its own stack, which
    size is determinated by the operating system.

    The execution of a thread starts either by calling the start() which
    calls the thread entry object passed to the constructor. Threads can
    either be joined, so you can wait for them, or be detached, so they
    run indepentently. A thread can be forced to terminate by calling
    terminate(), however, doing so is dangerous and discouraged.

    Thread also provides a platform independent sleep function. A thread
    can give up CPU time either by calling Thread::yield() or sleep() to
    stop for a specified period of time.
*/
class PT_SYSTEM_API Thread : protected NonCopyable
{
    public:
        //! @brief Status of a thread
        enum State
        {
            Ready    = 0, //!< Thread was not started yet
            Running  = 1, //!< Thread was started
            Joined   = 2, //!< Thread was joined
            Detached = 3  //!< Thread was detached
        };

    public:
        /** @brief Default Constructor

            Constructs a thread object without a thread entry. Use
            the init() method to set a callable. The thread will
            terminate immediately, if no thread entry is set.
        */
        Thread();

        /** @brief Constructs a thread with a thread entry

            Constructs a thread object to execute the %Callable \a cb.
            The Thread is not started on construction, but when start()
            is called.
        */
        explicit Thread(const Callable<void>& cb);

        /** @brief Constructs a thread with an event loop

            Constructs a thread object to run the event loop \a loop in
            a separate thread. The Thread is not started on construction,
            but when start() is called.
        */
        explicit Thread(EventLoop& loop);

        /** @brief Initialize with a thread entry

            The callable \a cb will be used as the thread entry. If
            another thread entry was set previously it will be replaced.
        */
        void init(const Callable<void>& cb);

        /** @brief Destructor

            The thread must either be joined or detached before the
            destructor is called.
        */
        virtual ~Thread();

        //! @brief Returns the current state of the thread.
        State state() const
        { return _state; }

        bool isJoinable() const
        { return _state == Running; }

        /** @brief Starts the thread

            This starts the execution of the thread by calling the thread
            entry. Throws a SystemError on failure.
        */
        void start();

        //! @brief Detaches the thread
        void detach();

        //! @brief Wait for the thread to finish execution.
        void join();

        /** @brief Exits athread.

            This function is meant to be called from within a thread to
            leave the thread at once. Implicitly called when the thread
            entry is left. Throws a SystemError on failure.
        */
        static void exit();

        /** @brief Yield CPU time

            This function is meant to be called from within a thread to
            give up the CPU to other threads. Throws a SystemError on
            failure.
        */
        static void yield();

        /** @brief Sleep for some time

            The calling thread sleeps for \a ms milliseconds. Throws a
            SystemError on failure.
        */
        static void sleep(unsigned int ms);

    protected:
        //! @internal
        bool joinNoThrow();

    private:
        //! @internal
        Thread::State _state;

        //! @internal
        bool _detach;

        //! @internal
        class ThreadImpl* _impl;
};

/** @brief A Joinable thread.

    %AttachedThreads are threads, which are managed by the creator,
    and are normally created on the stack. The creator must wait,
    until the thread terminates either explicitly by calling join()
    or implicitly by the destructor. The life-time of the callable
    object must exceed the life-time of the thread. Mind the order
    of destruction if the %AttachedThread is a member variable of
    a class.

    Example:
    \code
    struct Operation
    {
        void run()
        {
            // implement, whatever needs to be done in parallel
        }
    };

    int main()
    {
        Operation op;
        AttachedThread thread( Pt::callable(op, &Operation::run) );
        thread.start();

        // the thread runs and we can do something else in parallel

        doMoreWork();

        // AttachedThread's destructor joins the thread
        return 0;
    }
    \endcode
*/
class AttachedThread : public Thread
{
    public:
        /** @brief Default Constructor

            Constructs a thread object without a thread entry. Use
            the init() method to set a callable.
        */
        AttachedThread()
        : Thread()
        {}

        /** @brief Constructs a thread with a thread entry

            Constructs a thread object to execute the %Callable \a cb.
            The Thread is not started on construction, but when start()
            is called.
        */
        explicit AttachedThread(const Callable<void>& cb)
        : Thread(cb)
        {}

        /** @brief Constructs a thread with an event loop

            Constructs a thread object to run the event loop \a loop in
            a separate thread. The Thread is not started on construction,
            but when start() is called.
        */
        explicit AttachedThread(EventLoop& loop)
        : Thread(loop)
        {}

        //! @brief Joins the thread, if not already joined.
        ~AttachedThread()
        {
            Thread::joinNoThrow();
        }
};


/** @brief A detached thread.

    A detached thread runs just for its own. The user does not need
    (actually can not even) wait for the thread to stop. The object
    is normally created on the heap.

    Example:

    \code
    class MyThread : public Pt::System::::DetachedThread
    {
        protected:
            void run();
    };

    void MyThread::run()
    {
        // implement, whatever needs to be done in parallel
    }

    void someFunc()
    {
        MyThread *thread = new MyThread();
        thread->start();

        // here the thread runs and the program can do something
        // else in parallel. It continues to run even after this
        // function returns. The object is automatically destroyed,
        // when the thread has finished.
    }
    \endcode

*/
class DetachedThread : public Thread
{
    typedef void (*FuncPtrT)();

    public:
        explicit DetachedThread(FuncPtrT fp)
        : Thread( callable(fp) )
        {
            Thread::detach();
        }

    protected:
        /** @brief Constructs a detached thread.

            Constructs a thread object to execute the virtual method
            run() when start() is called. %DetachedThreads are always
            destructed by the virtual method destroy(). If objects
            of this class are created by new, destroy() must be
            overloaded ti call delete.
        */
        DetachedThread()
        : Thread()
        {
            Thread::init( callable(*this, &DetachedThread::exec) );
            Thread::detach();
        }

        /** @brief Destroys a detached thread.

            This method is called after the thread has finished. The
            default implementation uses delete to destruct this object.
        */
        virtual void destroy()
        { delete this; }

        /** @brief Thread entry method.

            This method is executed in a separate thread once start()
            is called. Override this method to implement a thread.
        */
        virtual void run()
        {}

    private:
        //! @internal
        void exec()
        {
            this->run();
            this->destroy();
        }
};

} // namespace System

} // namespace Pt

#endif // PT_SYSTEM_THREAD_H
