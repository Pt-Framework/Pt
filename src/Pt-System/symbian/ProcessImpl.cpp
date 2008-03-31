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
	throw SystemError("Process management not supported on Symbian",PT_SOURCEINFO);
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
//    m_pid = fork();
//
//    if( m_pid < 0 )
//    {
//        m_pid=-1;
//        throw SystemError("System call FORK() Failed!",PT_SOURCEINFO);
//    }
//
//    if( m_pid == 0)    // child Process
//    {
//        std::string strCommArgs = m_command + " " + m_args;
//        std::vector<char> buffer( strCommArgs.length() );
//        std::copy( strCommArgs.begin(), strCommArgs.end(), buffer.begin() );
//        buffer.push_back('\0');
//
//        //char cp[strCommArgs.length() + 1];
//        //strcpy(cp, strCommArgs.c_str());
//
//        char* cpArgs[ buffer.size() ];
//
//        unsigned int j = 0;
//		cpArgs[j++] = std::strtok(&buffer[0]," ");	// allocate the command to cpArgs
//
//		while( j < m_args.length()+1 && ( cpArgs[j++] = std::strtok(NULL," ") ) != NULL);	// allocate the arguments to cpArgs
//
//        if( 0 > execvp(cpArgs[0], cpArgs))
//        {
//            throw SystemError("System call EXECVP() Failed!",PT_SOURCEINFO);
//			std::exit(-1);
//        }
//    }
//    // Parent Process
//    return;
}


void ProcessImpl::kill()
{
//    if( 0 > ::kill(m_pid,SIGINT) )
//    {
//		throw SystemError(std::strerror(errno),PT_SOURCEINFO);
//    }
//    if( m_pid != ::wait(NULL) )
//    {
//		throw SystemError(std::strerror(errno),PT_SOURCEINFO);
//    }
}


void ProcessImpl::wait()
{
//    int iStatus;
//    if( 0 > waitpid(m_pid,&iStatus,WUNTRACED) )
//    {
//		throw SystemError(std::strerror(errno),PT_SOURCEINFO);
//    }
}

} // namespace Pt

} //namespace System

