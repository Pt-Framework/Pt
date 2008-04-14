/***************************************************************************
 *   Copyright (C) 2006 by Marc Boris Dürner                               *
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
#ifndef PT_wince_ProcessImpl_h
#define PT_wince_ProcessImpl_h

#include "../win32/ProcessImplBase.h"
#include "../win32/win32.h"


namespace Pt {

namespace System {

class ProcessImpl : public ProcessImplBase
{
    public:
        ProcessImpl(const string& command);
		
		ProcessImpl(const ProcessInfo& procInfo);

        static std::string getEnvVar(const string& name);
        
        static void setEnvVar(const string& name, const string& value);
        
        static void unsetEnvVar(const string& name)
        {
            ProcessImpl::setEnvVar(name, "");
        }
};

} // namespace System

} // namespace Pt

#endif