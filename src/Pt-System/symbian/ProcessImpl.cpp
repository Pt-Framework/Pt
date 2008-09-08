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
#include "ProcessImpl.h"
#include <f32file.h>
#include <collate.h>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>

namespace Pt {

namespace System {


ProcessImpl::ProcessImpl(const std::string& strCommand)
: m_command(strCommand)
{
}

ProcessImpl::ProcessImpl(const ProcessInfo& procInfo)
{
    m_command = procInfo.command();
    
    for( unsigned i = 0; i < procInfo.argCount(); i++)
        m_args += procInfo.getArgument( i);
}

ProcessImpl::~ProcessImpl()
{
}


const std::string& ProcessImpl::command()
{
    return m_command;
}


void ProcessImpl::start()
{
    if (m_command.length() > (unsigned)KMaxPath)
    {
        throw std::logic_error("Command path too long");        
    }
    
    TPtrC8 ptrCommand(reinterpret_cast<const TUint8*>(m_command.c_str()));
    TBuf<KMaxPath> descriptorCommand;
    descriptorCommand.Copy(ptrCommand);

    TParse parser;
    if (parser.SetNoWild(descriptorCommand, 0, 0) == KErrNone) 
    {
        if (parser.PathPresent())
        {
            _LIT(sysPath, "\\sys\\bin");
            TCollationMethod cm = *Mem::CollationMethodByIndex( 0 ); // default collation method
            cm.iFlags |= TCollationMethod::EFoldCase;
            // case insensitive comparison for path
            if (parser.Path().CompareC( sysPath, 0, &cm ) != 0)
            {
                throw std::logic_error("On Symbian path information must be empty or sys\\bin.");
            }
        }
    }
    
    // apparently KMaxPath has got nothing to do with the arguments
    // but still we need some maximum for a limit
    enum
    {
        KMaxCmdLine = KMaxPath*4
    };
    
    if (m_args.length() > (unsigned)KMaxCmdLine)
    {
        throw std::logic_error("Arguments too long");        
    }

    TPtrC8 ptrArgs(reinterpret_cast<const TUint8*>(m_args.c_str()));
    TBuf<KMaxCmdLine> descriptorArgs;
    descriptorArgs.Copy(ptrArgs);

    if (m_process.Create(descriptorCommand, descriptorArgs) != KErrNone)
    {
        throw SystemError("System call RProcess::Create() Failed!", PT_SOURCEINFO);
    }
    
    // execute
    m_process.Resume();
    
}


void ProcessImpl::kill()
{    
    m_process.Terminate(0);
}


int ProcessImpl::wait()
{
    // wait for process to exit busy loop style
    while (m_process.ExitType() == EExitPending)
    {
        User::After(1000);
    }

    //FIXME: return exit code
    return 0;
}


bool ProcessImpl::tryWait(int& status)
{
    // wait for process to exit busy loop style
    if(m_process.ExitType() == EExitPending)
    {
        return false;
    }

    //FIXME: return exit code
    status = 0;
    return true;
}

} // namespace Pt

} //namespace System

