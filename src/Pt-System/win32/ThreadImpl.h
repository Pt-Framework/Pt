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
            ThreadImpl()
			: _cb(0)
			, _handle(0)
			, _id(0)
			{}

            ~ThreadImpl();

			void init(const Callable<void>& cb);

            void start();

			void detach()
			{ this->close(); }

            void join();

            void terminate();

            static void exit();

			static void yield()
			{ sleep(0);	}

			static void sleep(unsigned int ms);

            const Callable<void>* cb()
            { return _cb; }

        public:
            static threadid_t WINAPI entry(void* arg)
            {
                ThreadImpl* impl = (ThreadImpl*)arg;
                const Callable<void>* cb = impl->cb();
				if(cb) cb->call();
                return 0;
            }

        protected:
            void close();

        private:
            const Callable<void>* _cb;
            HANDLE  _handle;
            threadid_t _id;
    };

} // namespace System

} // namespace Pt



