/*
 * Copyright (C) 2006-2013 Marc Boris Dürner
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

#ifndef PT_SEMAPHOREIMPL_H
#define PT_SEMAPHOREIMPL_H

#include "Pt/WinVer.h"
#include "Pt/System/Api.h"
#include <windows.h>


namespace Pt {

namespace System {

    //! @brief MS Windows specific semaphore class
    /**
        This class represents the MS Windows specific implementation
        of the Semaphore class. It is used as delegate from the common
        Semaphore class.
    */
    class SemaphoreImpl {
        public:
            //! @brief Default Constructor
            SemaphoreImpl(unsigned int initial = 0);

            //! @brief Destructor
            ~SemaphoreImpl();

            //! @brief MS Windows specific implementation of wait()
            /**
                @see Semaphore#wait()
            */
            void wait();

            //! @brief MS Windows specific implementation of tryWait()
            /**
                @see Semaphore#tryWait()
            */
            bool tryWait();

            //! @brief MS Windows specific implementation of post()
            /**
                @see Semaphore#post()
            */
            void post();

        private:
            HANDLE _handle;
    };

} // namespace System

} // namespace Pt

#endif
