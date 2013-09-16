/*
 * Copyright (C) 2006-2013 by Marc Boris Duerner
 * 
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

#include "ProcessImpl.h"
#include "win32.h"

#ifndef _WIN32_WCE
#include "PipeImpl.h"
#endif

#include <Pt/System/SystemError.h>
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

ProcessImpl::ProcessImpl(const ProcessInfo& procInfo)
: _procInfo(procInfo)
, _state(Process::Ready)
, _stdinPipe(0)
, _stdoutPipe(0)
, _stderrPipe(0)
{}


ProcessImpl::~ProcessImpl()
{
    delete _stdinPipe;
    delete _stdoutPipe;
    delete _stderrPipe;
}


void ProcessImpl::start()
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

    delete _stdoutPipe;
    _stdoutPipe = 0;

    delete _stderrPipe;
    _stderrPipe = 0;

    m_startUp.hStdInput = INVALID_HANDLE_VALUE;
    m_startUp.hStdOutput = INVALID_HANDLE_VALUE;
    m_startUp.hStdError = INVALID_HANDLE_VALUE;

    // Standard Input

    if( _procInfo.stdInputRedirected() )
    {
        _stdinPipe = new Pipe();

        SetHandleInformation( _stdinPipe->impl()->out().handle(),
                              HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT);
        m_startUp.hStdInput = _stdinPipe->impl()->out().handle();
    }
    else if( _procInfo.stdInputClosed() )
    {
        m_startUp.hStdInput = INVALID_HANDLE_VALUE;
    }

    // Standard Output

    if( _procInfo.stdOutputRedirected() )
    {
        _stdoutPipe = new Pipe();

        SetHandleInformation( _stdoutPipe->impl()->in().handle(),
                              HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT);
        m_startUp.hStdOutput = _stdoutPipe->impl()->in().handle();
    }
    else if( _procInfo.stdOutputClosed() )
    {
        m_startUp.hStdOutput = INVALID_HANDLE_VALUE;
    }

    // Standard Error

    if( _procInfo.stdErrorRedirected() )
    {
        _stderrPipe = new Pipe();

        SetHandleInformation( _stderrPipe->impl()->in().handle(),
                              HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT);
        m_startUp.hStdError = _stderrPipe->impl()->in().handle();
    }
    else if( _procInfo.stdErrorAsOutput() )
    {
        m_startUp.hStdError = m_startUp.hStdOutput;
    }
    else if( _procInfo.stdErrorClosed() )
    {
        m_startUp.hStdError = INVALID_HANDLE_VALUE;
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


void ProcessImpl::kill()
{
    if( 0 == TerminateProcess(m_pid.hProcess, 1) )
    {
        throw SystemError( PT_ERROR_MSG("TerminateProcess failed") );
    }
}


int ProcessImpl::wait()
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


bool ProcessImpl::tryWait(int& status)
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


unsigned long ProcessImpl::usedMemory()
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


#ifdef _WIN32_WCE

std::string ProcessImpl::getEnvVar(const std::string& name)
{
    HKEY hk;

    long ret = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
                                _T("Software\\Pt\\environment"),
                                0,
                                KEY_QUERY_VALUE,
                                &hk );

    if(ret != ERROR_SUCCESS)
    {
        throw SystemError( PT_ERROR_MSG("Could not open Registry") );
    }

    DWORD type = REG_SZ;
    DWORD byteLength = MAX_PATH * sizeof(TCHAR);
    TCHAR data[MAX_PATH] = {0};
    std::basic_string<TCHAR> wname;
    win32::fromMultiByte(name, wname);

    ret = RegQueryValueEx(hk, wname.c_str(), NULL, &type, (LPBYTE)data, &byteLength);

    RegCloseKey(hk);

    if(ret != ERROR_SUCCESS)
    {
        throw SystemError( PT_ERROR_MSG("Could not query Registry") );
    }

    if( byteLength == 0 || data[0] == 0 )
        return "";

    return win32::toMultiByte( (LPCTSTR)data );
}


void ProcessImpl::unsetEnvVar(const std::string& name)
{
    ProcessImpl::setEnvVar(name, "");
}


void ProcessImpl::setEnvVar(const std::string& name, const std::string& value)
{
    HKEY hk;
    DWORD ret = 0;
    ret = RegCreateKeyEx( HKEY_LOCAL_MACHINE,
                            _T("Software\\Pt\\environment"),
                            0,
                            _T(""),
                            0,
                            0,
                            NULL,
                            &hk,
                            &ret );
    if(ret != ERROR_SUCCESS)
    {
        throw SystemError( PT_ERROR_MSG("Could not create Registry key") );
    }

    std::basic_string<TCHAR> wname;
    win32::fromMultiByte(name, wname);
    std::basic_string<TCHAR> wvalue;
    win32::fromMultiByte(value, wvalue);

    LPBYTE data = (LPBYTE)wvalue.c_str();
    DWORD size = wvalue.size() * sizeof(TCHAR);

    LONG lret = RegSetValueEx(hk, wname.c_str(), 0, REG_SZ, data, size);
    RegCloseKey(hk);

    if(lret != ERROR_SUCCESS)
        throw SystemError( PT_ERROR_MSG("Could not set Registry value") );
}

#else

void ProcessImpl::setEnvVar(const std::string& name, const std::string& value)
{
    if( 0 == SetEnvironmentVariable(name.c_str(), value.c_str()) )
    {
        throw SystemError("Set Environment Variable Error!");
    }
}


void ProcessImpl::unsetEnvVar(const std::string& name)
{
    if( 0 == SetEnvironmentVariable(name.c_str(), NULL) )
    {
        throw SystemError("UnSet Environment Variable Error!");
    }
}


std::string ProcessImpl::getEnvVar(const std::string& name)
{
    char cp[200];
    std::string ret;
    DWORD cnt;
    cnt = GetEnvironmentVariable(name.c_str(), cp, 200);
    if( 0 == cnt )
    {
        cnt = GetLastError();
        if( ERROR_ENVVAR_NOT_FOUND ==  cnt )
        {
            return ret;
        }
        throw SystemError("Get Environment Variable Error 1!");
    }
    if(cnt<200)
    {
        ret=cp;
    }
    else
    {
        char *cp2 = new char[cnt+1];
        cnt = GetEnvironmentVariable(name.c_str(), cp2, cnt);
        if( 0 == cnt )
        {
            delete[] cp2;
            throw SystemError("Get Environment Variable Error 2!");
        }

        ret = cp2;
        delete [] cp2;
    }

    return ret;
}

#endif

} // namespace Pt

} //namespace System
