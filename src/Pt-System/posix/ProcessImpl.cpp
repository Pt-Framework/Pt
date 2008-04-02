#include "ProcessImpl.h"
#include "IODeviceImpl.h"
#include <cstdlib>
#include <iostream>
#include <vector>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>


namespace Pt {

namespace System {

ProcessImpl::ProcessImpl(const std::string& strCommand,
                         bool suppStdIn,
                         bool suppStdOut,
                         bool suppStdErr)
    :m_command(strCommand)
    ,m_devIn(0)
    ,m_devOut(0)
    ,m_devErr(0)
{
    m_suppStdStream[0] = suppStdIn;
    m_suppStdStream[1] = suppStdOut;
    m_suppStdStream[2] = suppStdErr;
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
    m_pid = fork();
        
    if( m_pid < 0 )
    {
        m_pid = -1;
        throw SystemError("System call FORK() Failed!", PT_SOURCEINFO);
    }
        
    if( m_pid == 0)    // child Process
    {
        // --- standard in
        if( m_devIn)
            dup2( m_devIn->impl()->fd(), STDIN_FILENO);
        else
        {
            if( !m_suppStdStream[0])
                fclose( stdin);
        }

        // --- standard out
        if( m_devOut)
            dup2( m_devOut->impl()->fd(), STDOUT_FILENO);
        else
        {
            if( !m_suppStdStream[1])
                fclose( stdout);
        }

        // --- standard err
        if( m_devErr)
            dup2( m_devErr->impl()->fd(), STDERR_FILENO);
        else
        {
            if( !m_suppStdStream[2])
                fclose( stderr);
        }

        // split m_command and args in anrray of pointers
        // we don't use strtok for problems with trailing spaces
        std::string strCommArgs = m_command + " " + m_args;
        std::vector<char> buffer( strCommArgs.length() );
        std::copy( strCommArgs.begin(), strCommArgs.end(), buffer.begin() );
        buffer.push_back('\0');

        char* cpArgs[ buffer.size() ];

        bool fetch = false;
        unsigned j = 0;
        for( unsigned int i = 0; i < buffer.size(); ++i)
        {
            if( buffer[i] != ' ' && buffer[i] != '\0')
            {
                if( !fetch)
                {
                    fetch = true;
                    cpArgs[j++] = &buffer[i];
                }
            }
            else
            {
                if( fetch)
                {
                    fetch = false;
                    buffer[i] = '\0';
                }
            }
        }
        cpArgs[j] = 0;

        // call exec
        if( 0 > execvp(cpArgs[0], cpArgs))
        {
            throw SystemError("System call EXECVP() Failed!",PT_SOURCEINFO);
            std::exit(-1);
        }
    }
        
    // Parent Process
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

} // namespace Pt

} //namespace System

