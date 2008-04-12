#include "ProcessImplBase.h"
#include "IODeviceImpl.h"

#include "win32.h"
#include <sstream>
#include <vector>

namespace Pt {

namespace System {

ProcessImplBase::ProcessImplBase(const string& command)
    : m_command(command)
    , m_devIn(0)
    , m_devOut(0)
    , m_devErr(0)
{
}

ProcessImplBase::ProcessImplBase(const ProcessInfo& procInfo)
{
    m_mask = procInfo.mask();

	m_command = procInfo.command();
 
    m_devIn  = procInfo.getStdInput();
    m_devOut = procInfo.getStdOutput();
    m_devErr = procInfo.getStdError();

    for( unsigned i = 0; i < procInfo.argCount(); i++)
        m_args += procInfo.getArgument( i);
}


const std::string& ProcessImplBase::command()
{
    return m_command;
}

void ProcessImplBase::start()
{
	STARTUPINFO m_startUp;

    ZeroMemory( &m_startUp, sizeof(m_startUp) );
    m_startUp.cb = sizeof(m_startUp);
    ZeroMemory( &m_pid, sizeof(m_pid) );

#ifndef _WIN32_WCE
    // --- standard in
    if( (m_mask.test(0)) &&  m_devIn)
    {
        SetHandleInformation( m_devIn->impl()->deviceHandle(), 
							  HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT);
        m_startUp.hStdInput = m_devIn->impl()->deviceHandle();
    }

    if( (m_mask.test(1)) && m_devOut)
    {
		SetHandleInformation( m_devOut->impl()->deviceHandle(), 
							  HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT);
		m_startUp.hStdOutput = m_devOut->impl()->deviceHandle();
	}

	if( (m_mask.test(2)) && m_devErr)
	{
		SetHandleInformation( m_devErr->impl()->deviceHandle(), 
							  HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT);
		m_startUp.hStdError = m_devErr->impl()->deviceHandle();
	}

    m_startUp.dwFlags |= STARTF_USESTDHANDLES;

    std::basic_string<TCHAR> tcommand = win32::fromMultiByte( m_command + " " + m_args  );
	std::vector<TCHAR> m_buffer;
    m_buffer.assign( tcommand.begin(), tcommand.end() );
    m_buffer.push_back(0);
    BOOL ret = CreateProcess( NULL, &m_buffer[0], NULL, NULL,
                              true, 0, NULL, NULL, &m_startUp, &m_pid);
#else
    std::basic_string<TCHAR> tcommand = win32::fromMultiByte( m_command  );
    std::basic_string<TCHAR> targs = win32::fromMultiByte( m_args );
    BOOL ret = CreateProcess( tcommand.c_str(), targs.c_str(), NULL, NULL,
                              0, 0, NULL, NULL, &m_startUp, &m_pid);
#endif

    if( !ret )
    {
		DWORD errCode = GetLastError(); 
		std::ostringstream errorOut;
		errorOut << "System call CreateProcess() Failed! error code: " << errCode;
        throw SystemError(errorOut.str().c_str(),PT_SOURCEINFO);
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
    if( WAIT_FAILED == WaitForSingleObject(m_pid.hProcess, INFINITE) )
    {
        throw SystemError("System call WaitForSingleObject() Failed!",PT_SOURCEINFO);
    }

    DWORD exitCode;
    GetExitCodeProcess( m_pid.hProcess, &exitCode);
    return exitCode;
}

} // namespace Pt

} //namespace System
