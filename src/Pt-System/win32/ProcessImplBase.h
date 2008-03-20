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

#include <cstdlib>
#include <sstream>
#include <stdlib.h>
#include <vector>
using namespace std;


namespace Pt {

namespace System {

    class ProcessImplBase
    {
        public:
            ProcessImplBase(const string& command,
                            bool suppStdIn,
                            bool suppSdtOut,
                            bool suppStdErr);

            static void sleep(unsigned int milliSec)
            { ::Sleep(milliSec); }

            const std::string& command();

            void setArgs(const std::string& strArgs);

            const std::string& args();

			void setInput(  IODevice& dev){ m_devIn  = &dev;  }
			void setOutput( IODevice& dev){ m_devOut = &dev; }
            void setErrput( IODevice& dev){ m_devErr = &dev; }
            
            void start();

            void kill();

            void wait();

    private:
        STARTUPINFO m_startUp;
        PROCESS_INFORMATION m_pid;
        std::vector<TCHAR> m_buffer;
        
        string m_command;
        
        bool m_suppStdStream[3];
        
        IODevice* m_stdIn;
        IODevice* m_stdOut;
        IODevice* m_stdErr;
        string m_args;
    };

} // namespace System

} // namespace Pt

#endif
