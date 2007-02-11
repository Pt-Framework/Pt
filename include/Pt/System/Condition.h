/***************************************************************************
 *   Copyright (C) 2006 by PTV AG                                          *
 *                                                                         *
 ***************************************************************************/
#ifndef PT_SYSTEM_CONDITION_H
#define PT_SYSTEM_CONDITION_H

#include <Pt/System/Api.h>
#include <Pt/NonCopyable.h>
#include <cstddef>


namespace Pt {

namespace System {
    class Mutex;

    //! @ingroup Pt-System
    //! @brief This class is used to control concurrent access.
    /**
        The Condition class is used to control concurrent access in a queued
        manner. The Condition class supports two types of signalling events,
        manual reset and automatic reset.
        Manual resets cause all blocked callers to be released. This can be
        understood as some kind of broadcast to signal all blocked callers at
        once. Manual resets are triggered by a call to signal().
        Automatic resets cause only a single blocked caller to be released.
        So this can be seen as some kind of wait queue where only the topmost
        is signaled. Automatic resets are signaled by a call to broadcast().

        In the example below there are 3 threads running. The main thread
        controls the signalling of the condition known by all three of them.
        Thread 1 waits infinite on the condition instance. Thread 2 waits
        1000 ms on the condition. If the condition is signaled before the
        timeout occures it will continue doing some more tasks. If the signal
        does not arrive in time thread 2 will terminate.
        @code
            //known by all threads
            Condition cond;

            <MAIN_THREAD>
                ...
                if(signalOnce)
                {
                    cond.signal();
                }
                else
                {
                    cond.broadcast();
                }
                ...
            </MAIN_THREAD>

            <THREAD 1>
                ...
                Mutex mtx1;
                doMySomething();
                cond.wait(mtx1);
                ...
            </THREAD 1>

            <THREAD 2>
                ...
                Mutex mtx2;
                doMySomething();
                if(cond.wait(mtx2, 1000))
                {
                    doAnotherThing();
                }
                else
                {
                    terminate();
                }
                ...
            </THREAD 2>
        @endcode

     */
    class PT_SYSTEM_API Condition : public NonCopyable {
        public:
            //! @brief Default Constructor.
            Condition();

            //! @ brief Destructor.
            ~Condition();

            //! @brief Wait until condition becomes signalled.
            /**
                Causes the caller to be suspended until the condition will be
                signaled. The given mutex will be unlocked before the caller
                is suspended.

                @param mtx the mutex to be unlocked before the caller will be
                suspended and which will be locked again if the caller is
                unblocked.
             */
            void wait( Mutex& mtx);

            //! @brief Wait until condition becomes signalled. Returns true if successful,
            //! @brief false if a timeout occurred.
            /**
                Causes the caller to be suspended until the condition will be
                signaled. The given mutex will be unlocked before the caller
                is suspended. The suspension takes at maximum ms milliseconds.

                @param mtx the mutex to be unlocked before the caller will be
                suspended and which will be locked again if the caller is
                unblocked.
                @param ms the maximum time to wait in milliseconds
                @return true if the caller was unblocked by a signal, false if
                a timeout occurred
             */
            bool wait( Mutex& mtx, unsigned int ms);

            //! @brief Unblock a single blocked thread.
            void signal();

            //! @brief Unblock all blocked threads.
            void broadcast();

        private:
            class ConditionImpl* _impl;

        friend class ConditionImpl;
    };

} // !namespace System

} // !namespace Pt

#endif
