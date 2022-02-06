/*
 * Copyright (C) 2006-2008 Marc Boris Duerner
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

#include "Pt/WinVer.h"
#include "Pt/System/Api.h"
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
            ThreadImpl();

            ~ThreadImpl();

            void init(const Callable<void>& cb);

            const Callable<void>* cb()
            {
                return _cb;
            }

            void start();

            void detach();

            void join();

        public:
            static void exit();

            static void yield();

            static void sleep(unsigned int ms);

        public:
            static threadid_t WINAPI entry(void* arg)
            {                
                const Callable<void>* cb = static_cast< const Callable<void>* >(arg);

                if( ! cb ) 
                    return 0;
                    
                try
                {
                    cb->call();
                    delete cb;
                }
                catch (...)
                {
                    delete cb;
                    throw;
                }
                
                return 0;
            }

        private:
            const Callable<void>* _cb;
            HANDLE  _handle;
            threadid_t _id;
    };

} // namespace System

} // namespace Pt



