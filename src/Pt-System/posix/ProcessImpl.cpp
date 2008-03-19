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

	ProcessImpl::ProcessImpl(const std::string& strCommand)
 :m_command(strCommand)
 ,m_devIn( 0)
 ,m_devOut( 0)

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

	void ProcessImpl::setInput( IODevice* dev)
	{
	  m_devIn = dev;
	}

	void ProcessImpl::setOutput( IODevice* dev)
	{
	  m_devOut = dev;
	}

	void ProcessImpl::start()
	{
	  m_devOut->write( "cucu", 4);
	  printf( "written in m_devOut\n");
	  m_devOut->sync();

	  m_pid = fork();

	  if( m_pid < 0 )
	  {
        m_pid=-1;
        throw SystemError("System call FORK() Failed!",PT_SOURCEINFO);
	  }

	  if( m_pid == 0)    // child Process
	  {
		if( m_devOut)
		{

		  int fd = m_devOut->impl()->fd();
		  dup2( fd, 1);
		}

		if( m_devIn)
		{
		  int fd = m_devIn->impl()->fd();
		  dup2( fd, 0);
		}

		std::vector<char> args( m_args.length() );
		std::copy( m_args.begin(), m_args.end(), args.begin() );
		args.push_back('\0');
		   
		char* p = &args[0];
		if( 0 > execvp(m_command.c_str(), &p))
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


	  void ProcessImpl::wait()
	  {
		int iStatus;
		if( 0 > waitpid(m_pid,&iStatus,WUNTRACED) )
		{
		  throw SystemError(std::strerror(errno),PT_SOURCEINFO);
		}
	  }

	} // namespace Pt

  } //namespace System

