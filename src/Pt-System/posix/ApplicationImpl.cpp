#include "ApplicationImpl.h"
#include "Pt/System/Pipe.h"
#include "Pt/System/Selector.h"
#include "Pt/System/Application.h"
#include "Pt/System/SystemError.h"
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <iostream>

namespace {

    Pt::System::Pipe* pt_signal_pipe = 0;
    static char _signalBuffer[128];
    //int signalPipe[2] = {-1, -1};

    void initSignalPipe()
    {
        if( ! pt_signal_pipe )
        {
            pt_signal_pipe = new Pt::System::Pipe(Pt::System::Pipe::Async);
        }

        /*if (signalPipe[0] == -1)
        {
            if (pipe(signalPipe) == -1)
            {
                throw std::runtime_error("error creating signal pipe");
            }

            int flags = ::fcntl(signalPipe[0], F_GETFL);
            if(-1 == flags)
                throw std::runtime_error("Could not get pipe flags." + PT_SOURCEINFO);

            int ret = ::fcntl(signalPipe[0], F_SETFL, flags|O_NONBLOCK);
            if(-1 == ret)
                throw std::runtime_error("Could not set pipe to non-blocking." + PT_SOURCEINFO);

            flags = ::fcntl(signalPipe[1], F_GETFL);
            if(-1 == flags)
                throw std::runtime_error("Could not get pipe flags." + PT_SOURCEINFO);

            ret = ::fcntl(signalPipe[1], F_SETFL, flags|O_NONBLOCK);
            if(-1 == ret)
                throw std::runtime_error("Could not set pipe to non-blocking." + PT_SOURCEINFO);
        }*/
    }

}


extern "C" void pt_system_application_sighandler(int sigNo)
{
    if(pt_signal_pipe)
    {
        pt_signal_pipe->output().write( (char*)&sigNo, sizeof(sigNo) );
    }

    //if (signalPipe[1] != -1)
    //    write(signalPipe[1], &sigNo, sizeof(sigNo));
}

namespace Pt {

namespace System {

ApplicationImpl::ApplicationImpl()
{
    ::initSignalPipe();
}


ApplicationImpl::~ApplicationImpl()
{
}


void ApplicationImpl::init(SelectorBase& s)
{
    pt_signal_pipe->input().setSelector(&s);
    connect(pt_signal_pipe->input().inputReady, &ApplicationImpl::onSystemSignal);
    pt_signal_pipe->input().beginRead( _signalBuffer, sizeof(_signalBuffer) );
}


void ApplicationImpl::onSystemSignal(IODevice& device)
{
    try
    {
        size_t n = device.endRead();

        int sigNo = 0;
        char* it = _signalBuffer;
        char* last = &_signalBuffer[ n- sizeof(sigNo) ];
        while(it <= last)
        {
            memcpy(&sigNo, it, sizeof(sigNo));
            Pt::System::Application::instance().systemSignal.send(sigNo);
            it += sizeof(sigNo);
        }

        device.beginRead( _signalBuffer, sizeof(_signalBuffer) );
    }
    catch(...)
    {
        device.beginRead( _signalBuffer, sizeof(_signalBuffer) );
        throw;
    }
}


bool ApplicationImpl::catchSystemSignal(int sig)
{
    if (sig > 0 && sig < NSIG)
    {
        struct sigaction act;

        act.sa_handler = pt_system_application_sighandler;
        sigemptyset(&act.sa_mask);
        act.sa_flags = SA_RESTART;

        if (-1 == ::sigaction(sig, &act, NULL))
        {
            throw SystemError("sigaction failed", PT_SOURCEINFO);
        }
		
		return true;
    }

	return false;
}


bool ApplicationImpl::raiseSystemSignal(int sig)
{
    if (sig > 0 && sig < NSIG)
    {
		if( 0 != ::raise(sig) )
        {
            throw SystemError("sigaction failed", PT_SOURCEINFO);
        }

		return true;
	}
	
	return false;
}


/*int ApplicationImpl::signalFd() const
{
    return signalPipe[0];
}*/

} // namespace System

} // namespace Pt
