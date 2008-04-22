#include "ProcessImpl.h"
#include <cstdlib>
#include <iostream>
#include <vector>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>


namespace Pt {

namespace System {


ProcessImpl::ProcessImpl(const std::string& strCommand)
:m_command(strCommand)
{
}


ProcessImpl::~ProcessImpl()
{
}


const std::string& ProcessImpl::command()
{
    return m_command;
}


void ProcessImpl::setArgs(const std::string& strArgs)
{
    m_args=strArgs;
}


const std::string& ProcessImpl::args()
{
    return m_args;
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


void ProcessImpl::wait()
{
    // wait for process to exit busy loop style
    while (m_process.ExitType() == EExitPending)
    {
        User::After(1000);
    }    
}

} // namespace Pt

} //namespace System

