/***************************************************************************
 *   Copyright (C) 2005 by Christian Prochnow                              *
 *   cproch@seculogix.de                                                   *
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

#include "Pt/Api.h"
#include "Pt/System/SystemError.h"

#include <pthread.h>
#include <errno.h>


namespace Pt {

namespace System {

    class PT_API ThreadKeyImpl {
        public:
            ThreadKeyImpl() throw(SystemError)
            {
                pthread_key_t h;
                int ret = pthread_key_create(&h, 0);
                if(ret != 0)
                    throw SystemError("Could not create thread-key", PT_SOURCEINFO);
            
                _handle = h;
            }

            ~ThreadKeyImpl() throw()
            {
                pthread_key_delete(_handle);
            }
    
            void set(void* ptr) throw(SystemError)
            {
                pthread_setspecific(_handle, ptr);
            }

            void* get() const throw(SystemError)
            {
                return pthread_getspecific(_handle);
            }

        private:
            pthread_key_t _handle;
    };

} // namespace System

} // !namespace Pt
