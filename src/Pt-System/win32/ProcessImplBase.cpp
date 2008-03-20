#include "ProcessImplBase.h"
#include "win32.h"
#include <vector>

#include <iostream>
using namespace std;


namespace Pt {

namespace System {

ProcessImplBase::ProcessImplBase(const string& command,
                            bool suppStdIn,
                            bool suppSdtOut,
                            bool suppStdErr)
  : m_command(command)
  , m_devIn(0)
  , m_devOut(0)
  , m_devErr(0)
{
    m_suppStdStream[0] = suppStdIn;
    m_suppStdStream[1] = suppStdOut;
    m_suppStdStream[2] = suppStdErr;    
}


const std::string& ProcessImplBase::command()
{
    return m_command;
}


void ProcessImplBase::setArgs(const string& strArgs)
{
    m_args = strArgs;
}


const std::string& ProcessImplBase::args()
{
    return m_args;
}


void ProcessImplBase::start()
{
    ZeroMemory( &m_startUp, sizeof(m_startUp) );
    m_startUp.cb = sizeof(m_startUp);
    ZeroMemory( &m_pid, sizeof(m_pid) );


#ifdef _WIN32_WCE
    std::basic_string<TCHAR> tcommand = win32::fromMultiByte( m_command  );
    std::basic_string<TCHAR> targs = win32::fromMultiByte( m_args );
    BOOL ret = CreateProcess( tcommand.c_str(), targs.c_str(), NULL, NULL,
                              0, 0, NULL, NULL, &m_startUp, &m_pid);
#else
    std::basic_string<TCHAR> tcommand = win32::fromMultiByte( m_command + " " + m_args  );
    m_buffer.assign( tcommand.begin(), tcommand.end() );
    m_buffer.push_back(0);
    BOOL ret = CreateProcess( NULL, &m_buffer[0], NULL, NULL,
                              true, 0, NULL, NULL, &m_startUp, &m_pid);
#endif

    if( !ret )
    {
        throw SystemError("System call CreateProcess() Failed!",PT_SOURCEINFO);
    }
}


void ProcessImplBase::kill()
{
    if( 0 == TerminateProcess(m_pid.hProcess, -1) )
    {
        throw SystemError("System call TerminateProcess() Failed!", PT_SOURCEINFO);
    }
}


void ProcessImplBase::wait()
{
    if( WAIT_FAILED == WaitForSingleObject(m_pid.hProcess, INFINITE) )
    {
        throw SystemError("System call WaitForSingleObject() Failed!",PT_SOURCEINFO);
    }
}

} // namespace Pt

} //namespace System
