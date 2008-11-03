/***************************************************************************
 *   Copyright (C) 2005-2006 by Marc Boris Dürner                          *
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
#include "Pt/System/Thread.h"
#include <pthread.h>
#include <sched.h>
#include <unistd.h>

namespace Pt {

namespace System {

class ThreadImpl
{
    public:
        ThreadImpl()
        : _cb(0)
        , _id(0)
        { }

        ~ThreadImpl()
        { delete _cb; }

        void init(const Callable<void>& cb);

        void detach();

        void start();

        void join();

        void terminate();

        static void exit()
        {
            ::pthread_exit( NULL );
        }

        static void yield()
        {
            ::sched_yield();
        }

        static void sleep(unsigned int ms)
        {
            usleep(ms * 1000);
        }

        const Callable<void>* cb()
        { return _cb; }

    private:
        const Callable<void>* _cb;
        pthread_t _id;
};

}

}
