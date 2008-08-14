/***************************************************************************
 *   Copyright (C) 2006-2007 Marc Boris Duerner                            *
 *   Copyright (C) 2006-2007 Sebastian Pieck                               *
 *   Copyright (C) 2006-2007 PTV AG                                        *
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

#ifndef PT_MUTEXIMPL_H
#define PT_MUTEXIMPL_H

#include "Pt/Api.h"
#include "Pt/System/Mutex.h"
#include <windows.h>


namespace Pt {

namespace System {

    //! @brief MS Windows specific mutex class
    /**
        This class represents the MS Windows specific implementation
        of the Mutex class. It is used as delegate from the common
        Mutex class.
    */
    class MutexImpl {
        public:
            //! @brief Default Constructor
            MutexImpl(Mutex& mutex, Mutex::Mode mode);

            //! @brief Destructor
            ~MutexImpl();

            //! @brief MS Windows specific implementation of lock()
            /**
                @see Mutex#lock()
            */
            void lock();

            //! @brief MS Windows specific implementation of tryLock()
            /**
                @see Mutex#tryLock()
            */
            bool tryLock();

            //! @brief MS Windows specific implementation of unlock()
            /**
                @see Mutex#unlock()
            */
            void unlock();

        private:
            Mutex& _mutex;
            HANDLE _handle;
    };

} // namespace System

} // namespace Pt

#endif
