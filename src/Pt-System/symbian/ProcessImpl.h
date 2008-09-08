/***************************************************************************
 *   Copyright (C) 2006 Marc Boris Duerner                                 *
 *   Copyright (C) 2008 Peter Barth                                        *
 *   Copyright (C) 2006-2008 PTV AG                                        *
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
#if !defined(PT_ProcessImpl_h)
#define PT_ProcessImpl_h

#include <cstdlib>
#include <sstream>
#include <unistd.h>
#include <e32base.h>

#include "Pt/System/Process.h"


namespace Pt {

namespace System {

class PT_API ProcessImpl
{
    public:
        ProcessImpl(const std::string& command);

        ProcessImpl(const ProcessInfo& procInfo);

        ~ProcessImpl();

        static void setEnvVar(const std::string& name, const std::string& value)
        {
            if( 0 > setenv(name.c_str(),value.c_str(),1) )
            {
                throw SystemError("not Enough Memory in Environment!",PT_SOURCEINFO);
            }
        }

        static void unsetEnvVar(const std::string& name)
        {
            unsetenv(name.c_str());
        }

        static std::string getEnvVar(const std::string& name)
        {
            std::string ret;
            const char* cp = std::getenv(name.c_str());
            if( NULL == cp )
            {
                return ret;
            }
            ret = cp;
            return ret;
        }

        static void sleep(size_t milliSec){
            User::After(milliSec*1000);
        }

        const std::string& command();

        void start();

        void kill();

        int wait();

        bool tryWait(int& status);

        static unsigned long usedMemory()
        { return 0;}

    private:
        RProcess m_process;
        std::string m_command;
        std::string m_args;
};

} // namespace System

} // namespace Pt

#endif // PT_ProcessImpl_h
