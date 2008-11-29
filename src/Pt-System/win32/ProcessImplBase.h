/***************************************************************************
 *   Copyright (C) 2005 by Marc Boris D�rner                               *
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
#ifndef PT_ProcessImplBase_h
#define PT_ProcessImplBase_h

#include "Pt/System/Process.h"
#include <windows.h>
#include <stdlib.h>
#include <cstdlib>
#include <sstream>
#include <vector>

namespace Pt {

namespace System {

    class ProcessImplBase
    {
        public:
            ProcessImplBase(const ProcessInfo& procInfo);

            static void sleep(unsigned int milliSec)
            { ::Sleep(milliSec); }

            const ProcessInfo& procInfo() const
            { return _procInfo; }

            void start();

            void kill();

            int wait();

            Process::State state() const
            { return _state; }
            
            bool tryWait(int& status);

            static unsigned long usedMemory();

    private:
        PROCESS_INFORMATION m_pid;
        ProcessInfo _procInfo;
        Process::State _state;
    };

} // namespace System

} // namespace Pt

#endif
