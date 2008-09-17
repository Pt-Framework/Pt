#include "ProcessImpl.h"
#include "IODeviceImpl.h"

#include <cstdlib>
#include <vector>
#include <signal.h>
#include <errno.h>
#include <cstring> // strerror()
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>

namespace Pt {

namespace System {

ProcessImpl::ProcessImpl(const ProcessInfo& procInfo)
: _procInfo(procInfo)
{
}


ProcessImpl::~ProcessImpl()
{
}


void ProcessImpl::start()
{
    m_pid = fork();

    if( m_pid < 0 )
    {
        m_pid = -1;
        throw SystemError("System call FORK() Failed!", PT_SOURCEINFO);
    }

    if( m_pid == 0)    // child Process
    {
        if( _procInfo.stdinClosed() )
        {
            fclose(stdin);
        }
        else if(_procInfo.stdin() )
        {
            dup2(_procInfo.stdin()->ioimpl().fd(), STDIN_FILENO);
        }

        if( _procInfo.stdoutClosed() )
        {
            fclose( stdout);
        }
        else if( _procInfo.stdout() )
        {
            dup2(_procInfo.stdout()->ioimpl().fd(), STDOUT_FILENO);
        }

        if( _procInfo.stderrClosed() )
        {
            fclose(stderr);
        }
        else if( _procInfo.stderr() )
        {
            dup2(_procInfo.stderr()->ioimpl().fd(), STDERR_FILENO);
        }

        std::vector< std::vector<char> > args;

        const std::string& c = _procInfo.command();
        std::vector<char> cmd( c.begin(), c.end() );
        cmd.push_back('\0');
        args.push_back(cmd);

        for( unsigned i = 0; i < _procInfo.argCount(); i++)
        {
            const std::string& a = _procInfo.arg(i);
            std::vector<char> arg(a.begin(), a.end());
            arg.push_back('\0');
            args.push_back(arg);
        }

        std::vector<char*> argptrs;
        for( unsigned n = 0; n < args.size(); n++)
        {
            std::vector<char>& a = args[n];
            argptrs.push_back( &a[0] );
        }
        argptrs.push_back( 0 );

        if( 0 > execvp(argptrs[0], &argptrs[0]))
        {
            throw SystemError("System call EXECVP() Failed!",PT_SOURCEINFO);
            std::exit(-1);
        }
    }

    return;
}


void ProcessImpl::kill()
{
    if( 0 > ::kill(m_pid,SIGINT) )
    {
        throw SystemError(std::strerror(errno),PT_SOURCEINFO);
    }
    if( m_pid != ::wait(NULL) )
    {
        throw SystemError(std::strerror(errno),PT_SOURCEINFO);
    }
}


int ProcessImpl::wait()
{
    int iStatus;
    if( 0 > waitpid(m_pid,&iStatus,WUNTRACED) )
    {
        throw SystemError(std::strerror(errno),PT_SOURCEINFO);
    }
    return iStatus;
}


bool ProcessImpl::tryWait(int& status)
{
    pid_t ret = waitpid(m_pid, &status, WUNTRACED|WNOHANG);
    if( 0 > ret)
    {
        throw SystemError(std::strerror(errno), PT_SOURCEINFO);
    }

    return ret != 0;
}


void ProcessImpl::setEnvVar(const std::string& name, const std::string& value)
{
    if( 0 > setenv(name.c_str(),value.c_str(),1) )
    {
        throw SystemError("not Enough Memory in Environment!",PT_SOURCEINFO);
    }
}


void ProcessImpl::unsetEnvVar(const std::string& name)
{
    unsetenv(name.c_str());
}


std::string ProcessImpl::getEnvVar(const std::string& name)
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


void ProcessImpl::sleep(size_t milliSec)
{
    usleep(milliSec*1000);
}


unsigned long ProcessImpl::usedMemory()
{
    struct rusage usage;
    int r =  getrusage(RUSAGE_SELF, &usage);
    if( r == -1)
        throw SystemError("getrusage failed", PT_SOURCEINFO);

    return usage.ru_idrss;
}

} // namespace Pt

} //namespace System

