/***************************************************************************
 *   Copyright (C) 2006 by PTV AG                                          *
 *                                                                         *
 ***************************************************************************/

#include "Pt/Api.h"
#include "Pt/System/Thread.h"

#include <windows.h>

#ifndef _WIN32_WCE
    #include <process.h>
#endif

namespace Pt {

namespace System {

#ifdef _WIN32_WCE
    typedef DWORD threadid_t;
#else
    typedef  unsigned threadid_t;
#endif

    class ThreadImpl 
	{
        public:
            ThreadImpl(Thread& obj, Thread::Mode mode);

            ~ThreadImpl();

            Thread::Mode mode() const
            { return _mode; }

            Thread::State state() const
            { return _state; }

            void start(Thread::Mode mode);

            void detach();

            void wait();

            static void exit();

            void terminate();

            static void yield();

            static void sleep(unsigned int ms);

        public:
            static threadid_t WINAPI entry(void* arg)
            {
                ThreadImpl* impl = (ThreadImpl*)arg;
                impl->_thread.run();
                impl->_state = Thread::Finished;
                return 0;
            }

        protected:
            void close();

        private:
            Thread& _thread;
            HANDLE  _handle;
            threadid_t _id;
            Thread::State _state;
            Thread::Mode _mode;
    };

} // namespace System

} // namespace Pt



