/***************************************************************************
 *   Copyright (C) 2005 by Marc Boris Dürner                               *
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
#include <bitset>

namespace Pt {

namespace System {

    class ProcessImplBase
    {
        public:
            ProcessImplBase(const std::string& command);
            ProcessImplBase(const ProcessInfo& procInfo);

            static void sleep(unsigned int milliSec)
            { ::Sleep(milliSec); }

            const std::string& command();

            void start();

            void kill();

            int wait();

    private:
		PROCESS_INFORMATION m_pid;
	
        std::string m_command;
        
        std::bitset<3> m_mask;
        
        IODevice* m_devIn;
        IODevice* m_devOut;
        IODevice* m_devErr;
		
        std::string m_args;
    };

} // namespace System

} // namespace Pt

#endif
