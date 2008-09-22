#include "ProcessImplBase.h"
#include "IODeviceImpl.h"
#include "Pt/System/SystemError.h"
#include "win32.h"
#include <sstream>
#include <vector>
#include <string>

#ifndef _WIN32_WCE
    #include <psapi.h>
#endif

namespace Pt {

namespace System {

ProcessImplBase::ProcessImplBase(const ProcessInfo& procInfo)
: _procInfo(procInfo)
, _state(Process::Ready)
{
}


void ProcessImplBase::start()
{
    _state = Process::Failed;

	STARTUPINFO m_startUp;

    ZeroMemory( &m_startUp, sizeof(m_startUp) );
    m_startUp.cb = sizeof(m_startUp);
    ZeroMemory( &m_pid, sizeof(m_pid) );

#ifndef _WIN32_WCE
    if( _procInfo.stdInputClosed() )
    {
        m_startUp.hStdInput = INVALID_HANDLE_VALUE;
    }
    else if( _procInfo.stdInput() )
    {
        SetHandleInformation( _procInfo.stdInput()->ioimpl().deviceHandle(), 
                              HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT);
        m_startUp.hStdInput = _procInfo.stdInput()->ioimpl().deviceHandle();
    }

    if( _procInfo.stdOutputClosed() )
    { 
        m_startUp.hStdOutput = INVALID_HANDLE_VALUE;
    }
    else if( _procInfo.stdOutput() )
    {
        SetHandleInformation( _procInfo.stdOutput()->ioimpl().deviceHandle(), 
                              HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT);
        m_startUp.hStdOutput = _procInfo.stdOutput()->ioimpl().deviceHandle();
    }

    if( _procInfo.stdErrorClosed())
    {
        m_startUp.hStdError = INVALID_HANDLE_VALUE;
    }
    else if( _procInfo.stdError() )
    {
        SetHandleInformation( _procInfo.stdError()->ioimpl().deviceHandle(), 
                              HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT);
        m_startUp.hStdError = _procInfo.stdError()->ioimpl().deviceHandle();
    }

    m_startUp.dwFlags |= STARTF_USESTDHANDLES;

    std::basic_string<TCHAR> tcmd = win32::fromMultiByte( _procInfo.command() );
    for( unsigned i = 0; i < _procInfo.argCount(); i++)
    {
        tcmd += win32::fromMultiByte( " " + _procInfo.arg(i) );
    }
    
    std::vector<TCHAR> m_buffer( tcmd.begin(), tcmd.end() );
    m_buffer.push_back(0);

    BOOL ret = CreateProcess( NULL, &m_buffer[0], NULL, NULL,
                              true, 0, NULL, NULL, &m_startUp, &m_pid);
#else

    std::string args;
    for( unsigned i = 0; i < _procInfo.argCount(); i++)
    {
        if(i != 0) args += ' ';
        args += _procInfo.arg(i);
    }

    std::basic_string<TCHAR> tcmd  = win32::fromMultiByte( _procInfo.command() );
    std::basic_string<TCHAR> targs = win32::fromMultiByte( args );

    BOOL ret = CreateProcess( tcmd.c_str(), targs.c_str(), NULL, NULL,
                              0, 0, NULL, NULL, &m_startUp, &m_pid);
#endif

    if(ret)
    {
        _state = Process::Running;
    }
}


void ProcessImplBase::kill()
{
    if( 0 == TerminateProcess(m_pid.hProcess, -1) )
    {
        throw SystemError("System call TerminateProcess() Failed!", PT_SOURCEINFO);
    }
}


int ProcessImplBase::wait()
{
    if(_state != Process::Running)
        return -1;

    if( WAIT_FAILED == WaitForSingleObject(m_pid.hProcess, INFINITE) )
    {
        _state = Process::Failed;
        throw SystemError("System call WaitForSingleObject() Failed!",PT_SOURCEINFO);
    }

    DWORD exitCode;
    GetExitCodeProcess( m_pid.hProcess, &exitCode);
    _state = Process::Finished;
    return exitCode;
}


bool ProcessImplBase::tryWait(int& status)
{
    if(_state != Process::Running)
        return false;

    DWORD ret = WaitForSingleObject(m_pid.hProcess, 0);

    if(WAIT_TIMEOUT == ret)
        return false;

    if(WAIT_OBJECT_0 == ret)
    {
        DWORD exitCode;
        GetExitCodeProcess( m_pid.hProcess, &exitCode);
        status = exitCode;
        _state = Process::Finished;
        return true;
    }

    throw SystemError("System call WaitForSingleObject() Failed!",PT_SOURCEINFO);
    return false;
}


unsigned long ProcessImplBase::usedMemory()
{
#ifndef _WIN32_WCE
    PROCESS_MEMORY_COUNTERS pmc;

    if(GetProcessMemoryInfo( GetCurrentProcess(), &pmc, sizeof(pmc)))
    {
        return (unsigned long)(pmc.PagefileUsage / 1024);
    }
    else
    {
        return 0;
    }
#else
    return 0;
#endif
}

} // namespace Pt

} //namespace System
