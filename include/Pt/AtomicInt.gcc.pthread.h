/***************************************************************************
 *   Copyright (C) 2006 by PTV AG                                          *
 *   Copyright (C) 2006 by Marc Boris Dürner                               *
 *   Copyright (C) 2006 by Aloysius Indrayanto                             *
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
#ifndef PT_ATOMICINT_GCC_PTHREAD_H
#define PT_ATOMICINT_GCC_PTHREAD_H

#warning "Using fallback implementation of AtomicInt (using pthread) !!!"


#include <csignal>
#include <pthread.h>


namespace Pt {

    typedef std::sig_atomic_t atomic_t;

    class AtomicInt
    {
        public:
            inline AtomicInt(atomic_t value = 0)
            : _value(value)
            {}

            inline atomic_t value() const
            { return _value;  }

            inline void operator+=(atomic_t n)
            {
                pthread_mutex_lock(&_mutex);
                _value += n;
                pthread_mutex_unlock(&_mutex);
            }

            inline void operator-=(atomic_t n)
            {
                pthread_mutex_lock(&_mutex);
                _value -= n;
                pthread_mutex_unlock(&_mutex);
            }

            inline void operator=(atomic_t n)
            {
                pthread_mutex_lock(&_mutex);
                _value = n;
                pthread_mutex_unlock(&_mutex);
            }

            inline bool compareExchange(atomic_t oldval, atomic_t newval)
            {
                bool ret = false;

                pthread_mutex_lock(&_mutex);
                if(_value == oldval) {
                    _value = newval;
                    ret = true;
                }
                pthread_mutex_unlock(&_mutex);

                return ret;
            }

        private:
            pthread_mutex_t   _mutex;
            volatile atomic_t _value;
    };

} // namespace Pt

#endif
