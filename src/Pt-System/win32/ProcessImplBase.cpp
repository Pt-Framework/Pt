/*
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
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

namespace {

    void checkExitCode(DWORD exitCode)
    {
        switch(exitCode)
        {
            case 0xC0000005: // access violation
            case 3:          // abort()
                throw Pt::System::ProcessFailed();

            default:
                break;
        }
    }

}

namespace Pt {

namespace System {

ProcessImplBase::ProcessImplBase(const ProcessInfo& procInfo)
: _procInfo(procInfo)
, _state(Process::Ready)
, _stdInput(0)
, _stdOutput(0)
, _stdError(0)
, _stdinPipe(0)
, _stdoutPipe(0)
, _stderrPipe(0)
{
}


ProcessImplBase::~ProcessImplBase()
{
    delete _stdinPipe;
    delete _stdoutPipe;
    delete _stderrPipe;
}


void ProcessImplBase::start()
{
    if (_state == Process::Running)
        throw std::runtime_error("invalid state in process start");

    _state = Process::Failed;

	STARTUPINFO m_startUp;

    ZeroMemory( &m_startUp, sizeof(m_startUp) );
    m_startUp.cb = sizeof(m_startUp);
    ZeroMemory( &m_pid, sizeof(m_pid) );

#ifndef _WIN32_WCE

    delete _stdinPipe;
    _stdinPipe = 0;
    _stdInput = 0;

    delete _stdoutPipe;
    _stdoutPipe = 0;
    _stdOutput = 0;

    delete _stderrPipe;
    _stderrPipe = 0;
    _stdError = 0;

	m_startUp.hStdInput = INVALID_HANDLE_VALUE;
	m_startUp.hStdOutput = INVALID_HANDLE_VALUE;
	m_startUp.hStdError = INVALID_HANDLE_VALUE;

    // Standard Input

    if( _procInfo.stdInputMode() == ProcessInfo::Capture )
    {
        _stdinPipe = new Pipe();
        _stdInput = &_stdinPipe->in();
        SetHandleInformation( _stdinPipe->out().ioimpl().deviceHandle(),
                              HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT);
        m_startUp.hStdInput = _stdinPipe->out().ioimpl().deviceHandle();
    }
    else if( _procInfo.stdInputMode() == ProcessInfo::Close )
    {
        m_startUp.hStdInput = INVALID_HANDLE_VALUE;
    }
    else if( _procInfo.stdInput() )
    {
        SetHandleInformation( _procInfo.stdInput()->ioimpl().deviceHandle(),
                              HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT);
        m_startUp.hStdInput = _procInfo.stdInput()->ioimpl().deviceHandle();
    }

    // Standard Output

    if( _procInfo.stdOutputMode() == ProcessInfo::Capture )
    {
        _stdoutPipe = new Pipe();
        _stdOutput = &_stdoutPipe->out();

        SetHandleInformation( _stdoutPipe->in().ioimpl().deviceHandle(),
                              HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT);
        m_startUp.hStdOutput = _stdoutPipe->in().ioimpl().deviceHandle();
    }
    else if( _procInfo.stdOutputMode() == ProcessInfo::Close )
    {
        m_startUp.hStdOutput = INVALID_HANDLE_VALUE;
    }
    else if( _procInfo.stdOutput() )
    {
        SetHandleInformation( _procInfo.stdOutput()->ioimpl().deviceHandle(),
                              HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT);
        m_startUp.hStdOutput = _procInfo.stdOutput()->ioimpl().deviceHandle();
    }

    // Standard Error

    if( _procInfo.stdErrorMode() == ProcessInfo::Capture )
    {
        _stderrPipe = new Pipe();
        _stdError = &_stderrPipe->out();

        SetHandleInformation( _stderrPipe->in().ioimpl().deviceHandle(),
                              HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT);
        m_startUp.hStdError = _stderrPipe->in().ioimpl().deviceHandle();
    }
    else if( _procInfo.stdErrorMode() == ProcessInfo::Combine )
    {
        m_startUp.hStdError = m_startUp.hStdOutput;
    }
    else if( _procInfo.stdErrorMode() == ProcessInfo::Close )
    {
        m_startUp.hStdError = INVALID_HANDLE_VALUE;
    }
    else if( _procInfo.stdError() )
    {
        SetHandleInformation( _procInfo.stdError()->ioimpl().deviceHandle(),
                              HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT);
        m_startUp.hStdError = _procInfo.stdError()->ioimpl().deviceHandle();
    }

    // TODO ???
    // if (_procInfo.detach())
    // {
    // }

    m_startUp.dwFlags |= STARTF_USESTDHANDLES;

    std::basic_string<TCHAR> tcmd;
	win32::fromMultiByte( _procInfo.command(), tcmd );
    for( unsigned i = 0; i < _procInfo.argCount(); i++)
    {
		std::basic_string<TCHAR> targ;
        win32::fromMultiByte( " " + _procInfo.arg(i), targ );
		tcmd += targ;
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

    std::basic_string<TCHAR> tcmd;
    win32::fromMultiByte( _procInfo.command(), tcmd );
    std::basic_string<TCHAR> targs;
    win32::fromMultiByte( args, targs );

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
    if( 0 == TerminateProcess(m_pid.hProcess, 1) )
    {
        throw SystemError( PT_ERROR_MSG("TerminateProcess failed") );
    }
}


int ProcessImplBase::wait()
{
    if(_state != Process::Running)
        return -1;

    if( WAIT_FAILED == WaitForSingleObject(m_pid.hProcess, INFINITE) )
    {
        _state = Process::Failed;
        throw SystemError( PT_ERROR_MSG("WaitForSingleObject Failed!") );
    }

    DWORD exitCode;
    GetExitCodeProcess( m_pid.hProcess, &exitCode);
    _state = Process::Finished;

    checkExitCode(exitCode);

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

        checkExitCode(exitCode);

        return true;
    }

    throw SystemError( PT_ERROR_MSG("WaitForSingleObject failed") );
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
